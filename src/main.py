#-----------------------------------------------
import optimization
import config as cfg
import time
#-----------------------------------------------
config_list = [[0,"b"],[1,"b"],[2,"b"]]
for config in config_list:
    start = time.time()
    seed  = config[0]
    p     = config[1]
    probs = cfg.POLICIES[p]
    
    print(f"seed : {seed}    p[%] : {probs * 100.}")
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
