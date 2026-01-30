#-----------------------------------------------
import optimization
import config as cfg
import time
#-----------------------------------------------
config_list = [[0,"e"],[1,"e"],[2,"e"],[0,"d"],[1,"d"],[2,"d"],[0,"a"],[1,"a"],[2,"a"],[0,"b"],[1,"b"],[2,"b"],[0,"c"],[1,"c"],[2,"c"]]
for config in config_list:
    start = time.time()
    seed  = config[0]
    p     = config[1]
    
    print(f"seed : {seed}    p[%] : {cfg.POLICIES[p] * 100.}")
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
