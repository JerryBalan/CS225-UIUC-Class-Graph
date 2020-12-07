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


s_avg = round(sum(time_s) / len(time_s), 3)
p_avg = round(sum(time_p) / len(time_p), 3)


plt.plot(range(10), time_s, label="serial (avg=" + str(s_avg) + "s )")
plt.plot(range(10), time_p, label="parallel (avg="+ str(p_avg) + "s )")
plt.xlabel("Iteration")
plt.ylabel("Time (s)")
plt.ylim(ymin=0)

plt.legend()

plt.savefig("compare.png")
plt.show()





