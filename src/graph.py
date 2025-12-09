import numpy as np
import config as cfg
import toolbox as tb
import copy
import torch
from torch_geometric.data import Data


def graph(id, g, node_np, element_np, graph, index, mask, mask_coil, gmat, den):
    "全体像-------------------"
    "g->ガウス関数計算->要素の材料番号更新->グラフのテンプレートを読む->要素の材料番号をノードにあてはめる->pygのdataにする"
    f_mask              = (gmat @ g) / den                                                          # (Ne_mask,)
    material            = np.full(mask.shape[0], cfg.ID_AIR, dtype=int)
    material[mask]      = np.where(f_mask >= 0, cfg.ID_IRON, cfg.ID_AIR)
    material[mask_coil] = cfg.ID_COIL
    mask_core           = np.zeros_like(mask, dtype=bool)
    mask_core[mask]     = (f_mask >= 0)
    element_renew       = np.concatenate((material.reshape(mask.shape[0],1),element_np[:,1:]),axis=1)
    # material_map = {cfg.ID_AIR: 0, cfg.ID_IRON: 1, cfg.ID_COIL: 2}  # それ以外は 3
    # tb.write_material_vtk("../results/material.vtk", node, element_new, material_map)
    node            = copy.deepcopy(graph['nodes'])
    receivers       = graph['receivers']
    senders         = graph['senders']
    pos_list        = node_np
    list_iron       = []
    "ノードへの材料番号割り当て操作----------------"
    node_onehot5, class_id, list_iron = build_node_onehot5_from_ragged(
        index_list=index,                 # ← いまのラグ配列
        material_elem=material,           # ← 要素ごとの基底材料ID (0/1/2)
        one_based=False                   # ← 1始まりなら True に
    )
    nodes_base = np.asarray(node,dtype=np.float32)[:, 0, :]  
    x_np = np.hstack([nodes_base, node_onehot5.astype(np.float32)]) 
    "torchのtensorに変換する操作------------------"
    x   = torch.from_numpy(x_np)
    y          = torch.empty(x.shape[0],1)                      #ダミー真値
    edge_index = torch.tensor([senders,receivers],dtype=torch.long)
    pos        = torch.tensor(pos_list,dtype=torch.float)
    pos        = pos.squeeze(1)
      # min-max正規化
    x_min             = x.min(dim=0, keepdim=True).values
    x_max             = x.max(dim=0, keepdim=True).values
    denom             = x_max - x_min
    denom[denom == 0] = 1.0
    x_normed          = (x - x_min) / denom
    "pygのdataに変換する操作-----------------------"
    data                 = Data(x=x_normed, edge_index=edge_index, y=y, pos = pos)
    sample_data          = data
    data.individual_id   = id
    data.iron_node_index = list_iron
    "鉄要素の面積算出------------------------------"
    delta = tb.cal_delta(node_np, element_np)
    delta_target = delta[mask_core].sum()

    return data,delta_target

def build_node_onehot5_from_ragged(index_list, material_elem, one_based=False):
    IRON, AIR, COIL = cfg.ID_IRON, cfg.ID_AIR, cfg.ID_COIL
    CLASS_LEN = 5

    N = len(index_list)

    lengths            = np.fromiter((len(lst) for lst in index_list), count=N, dtype=np.int64)
    rows               = np.repeat(np.arange(N, dtype=np.int64), lengths)
    cols               = np.concatenate(index_list).astype(np.int64, copy=False)
    base_ids           = material_elem[cols].astype(np.int64)
    node_base_presence = np.zeros((N, 3), dtype=np.int8)
    np.maximum.at(node_base_presence, (rows, base_ids), 1) #iron,air,coilのone-hot
    b_iron       = node_base_presence[:, 0]
    b_air        = node_base_presence[:, 1]
    b_coil       = node_base_presence[:, 2]
    code3        = (b_iron << 0) + (b_air << 1) + (b_coil << 2) #3bitフラグ
    lut          = np.full(8, -1, dtype = np.int8) #lutはlook up tableの意らしい.3bitフラグから5次元のone-hotへの対応表．
    lut[1]       = 0; lut[2] = 1; lut[4] = 2; lut[3] = 3; lut[6] = 4
    class_id     = lut[code3]
    node_onehot5 = np.zeros((N, CLASS_LEN), dtype = np.int8)
    node_onehot5[np.arange(N), class_id.astype(np.int64)] = 1
    list_iron    = np.flatnonzero(class_id == 0).tolist()

    return node_onehot5, class_id, list_iron