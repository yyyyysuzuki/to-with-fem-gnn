import numpy as np
import random
np.random.seed(1)
random.seed(1)
class Gene():
#-----------------------------------------------
    def __init__(self, dim, minval, maxval):
        self.Dim        = dim
        self.gid        = None
        self.garea      = None
        self.gabsb      = None
        self.gfem       = None
        self.Fitness    = 0.                                     
        self.Gene       = np.random.uniform(minval, maxval, dim)    
        self.minval     = minval
        self.maxval     = maxval
    #-------------------------------------------
    def gene(self):
        return self.Gene
    def geneAllZero(self):
        self.Gene = np.zeros(self.Dim)
    def set_gene(self, g):
        self.Gene = g
    #-------------------------------------------
    def fitness(self):
        return self.Fitness
    def fitAllzero(self):
        self.Fitness = 0.0
    def set_fitness(self, f):
        self.Fitness = f
    def set_id(self, id):
        self.gid = id
    def id(self):
        return self.gid
    def set_s(self, s):
        self.garea   = s
        self.Fitness = None
    def s(self):
        return self.garea
    def set_absb(self, absb):
        self.gabsb   = absb
        self.Fitness = None
    def set_fem(self, fem):
        self.gfem = fem
    def fem(self):
        return self.gfem
    