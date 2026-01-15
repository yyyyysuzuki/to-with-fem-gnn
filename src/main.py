#-----------------------------------------------
import optimization
import config as cfg
import time
#-----------------------------------------------
config_list = [[0,"b"],[1,"b"],[2,"b"],[0,"c"],[1,"c"],[2,"c"],[0,"d"],[1,"d"],[2,"d"],[0,"e"],[1,"e"],[2,"e"]]
for config in config_list:
    start = time.time()
    seed  = config[0]
    pcfg  = config[1]
    print(f"seed : {seed}    pcfg : {pcfg}")
    mymodel = optimization.GeneticAlgorithm( 
        seed,
        pcfg,
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
