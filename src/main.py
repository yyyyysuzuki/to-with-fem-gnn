#-----------------------------------------------
import optimization
import config as cfg
import time
#-----------------------------------------------
start = time.time()
mymodel = optimization.GeneticAlgorithm( cfg.DIM
                                        ,cfg.STEPSIZE
                                        ,cfg.GENERATION
                                        ,cfg.POPNUMBER
                                        ,cfg.PARENTNUMBER
                                        ,cfg.CHILDRENNUMBER
                                        ,cfg.FUNCTYPE
                                        ,cfg.MINVAL
                                        ,cfg.MAXVAL)
mymodel.main()
end = time.time()
print(f"time : {end - start} [s]")
print('Done')
