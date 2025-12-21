"===== 標準ライブラリ ====="
import os
import sys
import re
import json
import csv
import time
import math
import copy
import random
import pprint

"===== サードパーティ（外部ライブラリ） ====="
import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F
from torch_geometric.data import Data
from torch_geometric.loader import DataLoader
from sklearn.model_selection import train_test_split
from omegaconf import DictConfig
import matplotlib.pyplot as plt

"===== 自作モジュール ====="
import Cal
import toolbox as tb
import graph as gh
import config as cfg
from gene import Gene
from multiscale_operator.operators.gnn_perceiver import GraphPeceiverOperator


"出力桁数設定---------------"
np.set_printoptions(precision=8, floatmode='maxprec')

"定数設定-------------------"
NUM_ELITE = 0

class GeneticAlgorithm():
 #---------------------------------------------
    ##  1. Constructor
    def __init__(self, seed, pcfg, dim, stepsize, GenNumber, PopNumber, ParentNumber, ChildrenNumber, functype, minval, maxval):
        self.seed            = seed
        random.seed(self.seed)
        np.random.seed(self.seed)
        torch.manual_seed(self.seed)
        torch.cuda.manual_seed(self.seed)
        self.pcfg            = pcfg
        self.indnum          = 0
        self.numfem          = 0
        self.A_scaling       = 1e+6
        self.node            = None
        self.element         = None
        self.graph           = None
        self.index           = None
        self.mask_coil       = None
        self.mcoil           = None
        self.gaussian_matrix = None
        self.den             = None
        self.is_fem          = []
        self.list_id         = []
        self.list_absb       = []
        self.list_s          = []
        self.list_value      = []
        self.list_void       = []
        self.dim             = dim                                            # dimension
        self.stepsize        = stepsize                                       # stepsize for REXStar
        self.GenNumber       = GenNumber                                      # Number of generations
        self.PopNumber       = PopNumber                                      # Number of population
        self.ParentNumber    = ParentNumber                                   # Number of parents
        self.ChildrenNumber  = ChildrenNumber                                 # Number of children
        self.functype        = functype                                       # Objective function
        self.minval          = minval
        self.maxval          = maxval
        self.idx_tmp         = np.random.randint(0, PopNumber, ParentNumber)
        self.Gpar            = np.zeros(dim)
        self.Gbest           = np.zeros(dim)
        self.population      = ['pop']*PopNumber
        self.parent          = ['parent']*ParentNumber
        self.mirror          = ['reflection']*ParentNumber
        self.bestparents     = ['bestparent']*ParentNumber
        self.children        = ['children']*ChildrenNumber
        self.best            = [Gene(dim, minval, maxval)]
        for i in range(PopNumber):
            self.population[i] = Gene(dim, minval, maxval)
        
        for i in range(ParentNumber):
            self.parent[i]      = Gene(dim, minval, maxval)
            self.mirror[i]      = Gene(dim, minval, maxval)
            self.bestparents[i] = Gene(dim, minval, maxval)
        
        for i in range(ChildrenNumber):
            self.children[i] = Gene(dim, minval, maxval)

#-----------------------------------------------
    ##  3. Main
    def main(self):
        model,device = self.preset()
        
        self.Eval(self.population,0,model,device)
        self.minsort(self.population)
        
        countGen    = []
        avefitness  = []
        bestfitness = []
        fem         = []
        for GenLoop in range(self.GenNumber):
            self.indnum = 0
            self.numfem = 0
            self.Reproduction()
            self.Allzero(self.Gpar)
            self.Cal_grav(self.parent, self.Gpar)
            
            self.make_Mirror()
            self.Eval(self.mirror,GenLoop+1,model,device)
            self.BestParents()
            self.Allzero(self.Gbest)
            self.Cal_grav(self.bestparents, self.Gbest)
            
            self.REXstar()
            self.Eval(self.children,GenLoop+1,model,device)
            self.minsort(self.children)
            self.Replace()
            self.minsort(self.population)
            if(GenLoop == 0):
                self.best = copy.deepcopy(self.population[0])
            
            if(GenLoop > 0):
                if(Gene.fitness(self.population[0]) < Gene.fitness(self.best)):
                     self.best = copy.deepcopy(self.population[0])
            print(GenLoop, Gene.fitness(self.best))

            fem.append(self.numfem)
            countGen.append(GenLoop)
            bestfitness.append(Gene.fitness(self.best))
            avefitness.append(self.average())
            if (math.sqrt((Gene.fitness(self.best))**2) <= 1e-7):
                print(GenLoop)
                print(Gene.gene(self.best))
                break
            
            tb.write_three_lists_to_csv(avefitness, bestfitness, fem, f'../results/{self.pcfg}_{self.seed}/gen_fitness.csv', ('ave','best','is fem'))
            tb.write_six_lists_to_csv(self.list_id,self.is_fem,self.list_absb,self.list_s,self.list_value,self.list_void,f'../results/{self.pcfg}_{self.seed}/fitness.csv',('id','FEM?','ABSB','SheldS','fitness','void'))
            plt.figure()
            plt.plot(countGen, bestfitness,   color='black', linestyle='solid',   label='best')
            plt.plot(countGen, avefitness,    color='black', linestyle='dotted',  label='average')
            plt.xlabel('Generation[-]')
            plt.ylabel('Fitness[-]')
            plt.legend()
            plt.savefig('FitnessCurve.png')
            plt.clf()
 #-----------------------------------------------
    ##  4. reproduction
    def Reproduction(self):
        for i in range(self.ParentNumber):
            if(i < NUM_ELITE):
                idx = i                                                 
            else:
                idx = np.random.randint(0, self.PopNumber)
                while True:
                    judge = True
                    for j in range(i):
                        if(idx == self.idx_tmp[j]):
                            idx     = np.random.randint(0, self.PopNumber)
                            judge   = False
                    if(judge == True):
                        break
            self.parent[i]  = copy.deepcopy(self.population[idx])
            self.idx_tmp[i] = idx
#-----------------------------------------------
    ##  5. calculate gravity center
    def Cal_grav(self, individual, gpoint):
        for i in range(len(individual)):
            gpoint += Gene.gene(individual[i])
        gpoint /= len(individual)
 #-----------------------------------------------
    ##  6. make mirror individuals
    def make_Mirror(self):
        for i in range(self.ParentNumber):
            gene = 2.0 * self.Gpar - Gene.gene(self.parent[i])
            Gene.set_gene(self.mirror[i], gene)
#-----------------------------------------------
    ##  7. evaluation
    def Eval(self, g, gen,model,device):
        "pyg-dataに変換-------------------------------"
        indnum_local = self.indnum
        list_data    = []
        list_s       = []
        print(f"gen : {gen}    -----------------------")
        for i in range(len(g)):
            id            = 'gen_' + str(gen) + 'ind_' + str(i + indnum_local)
            data,s        = gh.graph(id, g[i].gene(), self.node, self.element, self.graph, self.index, self.mask, self.mask_coil, self.gaussian_matrix, self.den)
            list_data.append(data)
            list_s.append(s)
            self.indnum += 1
            tb.write_list_to_csv(g[i].gene(), f"../results/{self.pcfg}_{self.seed}/w_{id}.csv", header=None)
        "推論------------------------------------------"
        eval_loader = DataLoader(list_data,batch_size=32,shuffle = False)
        OutList    = []
        OutListMSE = []
        IdList     = []
        outlist    = []
        outlistmse = []
        idlist     = [] 
        for i,eval_data in enumerate(eval_loader):
            is_last       = (i == len(eval_loader) - 1)
            out           = model(eval_data.to(device))
            out_per_graph = [out[eval_data.ptr[i]:eval_data.ptr[i+1]].detach().cpu() for i in range(len(eval_data.ptr)-1)]
            id_per_graph  = eval_data.individual_id
            idlist.append(id_per_graph)
            outlist.append(out_per_graph)
            if is_last:
                OutList.append(outlist)
                OutListMSE.append(outlistmse)
                IdList.append(idlist)
        IdList  = tb.flatten_list(IdList)
        OutList = tb.flatten_list(OutList) 
        "評価値算出--------------------------------------"
        for i,id in enumerate(IdList):
            used_fem_flag = False
            j, ind_num    = tb.get_i_from_id(id, indnum_local)
            A_rescaling   = OutList[i].numpy() / self.A_scaling
            bx,by         = tb.CalB_v2(A_rescaling,self.node,self.element)
            absb_pred     = tb.CalTargetBB(bx,by)
            fitness_pred  = list_s[j] + 1e+5*absb_pred

            # 2. FEM 再解析するか判断
            if self.should_run_fem(fitness_pred, self.pcfg, None):
                used_fem_flag = True
                a_reval       = Cal.Cal(str(id),g[j].gene())
                a_reval       = a_reval.reshape(-1, 1)
                bx,by         = tb.CalB_v2(a_reval,self.node,self.element)
                absb          = tb.CalTargetBB(bx,by)
                fitness       = list_s[j] + 1e+5*absb
                self.numfem   += 1
            else:
                absb    = absb_pred
                fitness = fitness_pred
            

            print("end val-------------------------------------")
            print(f"id      : {id}")
            print(f"FEM?    : {used_fem_flag}")
            print(f"|B|     : {absb}")
            print(f"S       : {list_s[j]}")
            print(f"fitness : {fitness}")
            self.is_fem.append(used_fem_flag)
            self.list_id.append(id)
            self.list_absb.append(absb)
            self.list_s.append(list_s[j])
            self.list_value.append(fitness)
            self.list_void.append(0)
            Gene.set_fitness(g[j], fitness)
        "重み保存------------------------------------------------"

    def BestParents(self):
        Parent_Mirror = self.parent + self.mirror
        self.minsort(Parent_Mirror)
        self.bestparents = copy.deepcopy(Parent_Mirror[:self.ParentNumber])
#-----------------------------------------------
##  10. sort
    def minsort(self, Individual):
        fitness = np.zeros(len(Individual))
        for i in range(len(Individual)):
            fitness[i]  = Gene.fitness(Individual[i])
        sortind         = np.argsort(fitness)
        Individualtmp   = ['individual']*len(Individual)
        for i in range(len(Individual)):
            Individualtmp[i] = copy.deepcopy(Individual[sortind[i]])
        for i in range(len(Individual)):
            Individual[i] = copy.deepcopy(Individualtmp[i])
 #-----------------------------------------------
##  11. REXstar
    def REXstar(self):
        for i in range(self.ChildrenNumber):
            xi_t    = np.random.uniform(0.0, self.stepsize, self.dim)
            val_1   = xi_t * (self.Gbest - self.Gpar)
            val_2   = np.zeros(self.dim)
            for parent in self.parent:
                xi      = np.random.uniform(-1.0*math.sqrt(3.0/self.ParentNumber), math.sqrt(3.0/self.ParentNumber))
                val_2   += xi * (Gene.gene(parent) - self.Gpar)
            gene = self.Gpar + val_1 + val_2
            Gene.set_gene(self.children[i], gene)

 #-----------------------------------------------
    ##  12. replace population with children
    def Replace(self):
        for i in range(self.ParentNumber):
            self.population[self.idx_tmp[i]] = copy.deepcopy(self.children[i])
#-----------------------------------------------
    def average(self):
        aveval = 0.
        for i in range(self.PopNumber):
            aveval += Gene.fitness(self.population[i]) / self.PopNumber
        return aveval
 #-----------------------------------------------
    def Allzero(self, X):
        for i in range(len(X)):
            X[i] = 0.

    def preset(self):
        DIM          = 64
        MIN_VAL      = -5.12
        MAX_VAL      = 5.12
        PTH_MESH     = "../data/template_mesh.json"
        PTH_GRAPH    = "../data/template_graph.json"
        PTH_GAUSSIAN = "../data/gaussian_center.csv"
        PTH_INDEX    = "../data/element_index.csv"
        PTH_DATA     = "../data"
        ID_SHEILD    = "sample"

        "一括事前算出------------------------------------"
        mesh                 = tb.read_Json(PTH_MESH)
        self.graph           = tb.read_Json(PTH_GRAPH)
        self.index           = tb.read_csv(PTH_INDEX)
        gene_ex01            = Gene(DIM, MIN_VAL, MAX_VAL)
        self.node,self.element         = np.array(mesh['Node'])[:,:2],np.array(mesh['Element'])
        gx, gy               = tb.cal_gravity(self.node, self.element)
        m1                   = (gx >  0)   & (gx <  cfg.P5X) & (gy >  cfg.P8Y) & (gy <  cfg.P6Y)
        m2                   = (gx >  cfg.P7X)& (gx <  cfg.P4X) & (gy >  cfg.P7Y) & (gy <  cfg.P8Y)
        self.mask_coil       = (gx >  cfg.P10X) & (gx < cfg.P12X) & (gy > cfg.P10Y) & (gy < cfg.P12Y)
        self.mask            =  m1 | m2  
        gaussian_centers     = np.asarray(tb.read_csv_float(PTH_GAUSSIAN))
        self.gaussian_matrix, self.den = tb.precompute_G0(gx, gy, self.mask, gaussian_centers, dtype=np.float32)
        data_sample,_        = gh.graph(ID_SHEILD, gene_ex01.gene(), self.node, self.element, self.graph, self.index, self.mask, self.mask_coil,self.gaussian_matrix, self.den)
        "パラメータロード----------------------------------"
        device               = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
        print(f"device : {device}")
        pth_w                = os.listdir(PTH_DATA)
        pth_w                = [f for f in pth_w if f.endswith('.pth')]
        if not pth_w:
            raise FileNotFoundError(f"重みファイル（.pth）が {PTH_DATA} にありません")
        elif len(pth_w) > 1:
            raise RuntimeError("重みファイルが複数存在します。1つだけにしてください。")
        pth_w   = os.path.join(PTH_DATA, pth_w[0])
        dictcfg = {
                    'mlp_hidden_layer':3,
                    'num_message_passing':5,
                }
        model = GraphPeceiverOperator(DictConfig(dictcfg))
        model.init_shapes(data_sample)
        model = model.to(device)
        model.load_state_dict(torch.load(pth_w))
        model.eval()

        return model,device
    
    def should_run_fem(self, srore_pred: float, policy_name: str, rng: random.Random | None = None) -> bool:
        """
        GNN の評価値 sore_pred と，ポリシー名 ('a' ~ 'e') を受け取り，
        FEM を実行するかどうかを確率分布に従って決定し bool を返す．
        """
        if rng is None:
            rng = random

        probs   = cfg.POLICIES[policy_name] # 8要素のリスト
        bin_idx = tb.get_bin_index(srore_pred, cfg.BINS)
        p       = probs[bin_idx] # 0.0 ~ 1.0

        return rng.random() < p
