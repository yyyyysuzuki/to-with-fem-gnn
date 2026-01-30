m = 1e-3

#設計領域
P4X = 540*m
P4Y = 0.0*m
P5X = 540*m
P5Y = 540*m
P6X = 0.0*m
P6Y = 540*m
P7X = 40*m
P7Y = 0.0*m
P8X = 40*m
P8Y = 40*m
P9X = 0.0*m
P9Y = 40*m
P10X= 40*m
P10Y= 580*m
P11X= 40*m
P11Y= 620*m
P12X= 80*m
P12Y= 620*m
P13X= 80*m
P13Y= 580*m

# --- 材料設定 ---
ID_IRON = 0
ID_AIR  = 1
ID_COIL = 2

# --- GA の設定 ---
# DIM             = 64
# STEPSIZE        = 2.5
# GENERATION      = 10
# POPNUMBER       = 10
# PARENTNUMBER    = 2
# CHILDRENNUMBER  = 4
# FUNCTYPE        = 'FEM'
# MINVAL          = -5.12
# MAXVAL          = 5.12

DIM             = 64
STEPSIZE        = 2.5
GENERATION      = 100
POPNUMBER       = 1000
PARENTNUMBER    = 100
CHILDRENNUMBER  = 200
FUNCTYPE        = 'FEM'
MINVAL          = -5.12
MAXVAL          = 5.12

# --- 評価関数 の設定 ---
ALPHA = 1e+5

# --- 再解析 の設定 ---

POLICIES = {
    "a": 0.20,
    "b": 0.40,
    "c": 0.60,
    "d": 0.80,
    "e": 1.00
}


