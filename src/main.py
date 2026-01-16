#-----------------------------------------------
import optimization
import config as cfg
import time
#-----------------------------------------------
config_list = [[0,"a"],[1,"b"]]
for config in config_list:
    start = time.time()
    seed  = config[0]
    p     = config[1]
    
    print(f"seed : {seed}    p[%] : {p * 100.}")
    mymodel = optimization.GeneticAlgorithm( 
        seed,
        p,
        cfg.DIM,
        cfg.STEPSIZE,
        cfg.GENERATION,
        cfg.POPNUMBER,
        cfg.PARENTNUMBER,
        cfg.CHILDRENNUMBER,
        cfg.FUNCTYPE,
        cfg.MINVAL,
        cfg.MAXVAL)
    mymodel.main()
    end = time.time()
    print(f"time : {end - start} [s]")
    print('Done')
