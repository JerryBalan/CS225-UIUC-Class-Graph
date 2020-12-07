import matplotlib.pyplot as plt
import os
import time

time_s = []
time_p = []
for i in range(10):
    print ("testing " + str(i))
    start_serial = time.time()
    os.system("./main 0")
    end_serial = time.time()

    start_parallel = time.time()
    os.system("./main 1")
    end_parallel = time.time()



    time_s.append(end_serial - start_serial)

    time_p.append(end_parallel - start_parallel)


plt.plot(range(10), time_s, label="serial")
plt.plot(range(10), time_p, label="parallel")
plt.legend()

plt.show()
plt.savefig("compare.png")





