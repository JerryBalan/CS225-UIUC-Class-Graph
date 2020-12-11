import matplotlib.pyplot as plt
import os
import time

iterations = int(input("How many times would you like to run each algorithm?"))
time_s = []
time_p = []

# Run each algorithm the designated amount of times
for i in range(iterations):
    print ("testing " + str(i))
    start_serial = time.time()
    # Execute serial algorithm, including image drawing
    os.system("./main 0")
    end_serial = time.time()

    start_parallel = time.time()
    # Execute parallel algorithm, including image drawing
    os.system("./main 1")
    end_parallel = time.time()

    # Append runtime in seconds to each array
    time_s.append(end_serial - start_serial)
    time_p.append(end_parallel - start_parallel)

# Calculate serial and parallel averages
s_avg = round(sum(time_s) / len(time_s), 3)
p_avg = round(sum(time_p) / len(time_p), 3)

# Set plot variables
plt.plot(range(iterations), time_s, label="serial (avg=" + str(s_avg) + "s)")
plt.plot(range(iterations), time_p, label="parallel (avg="+ str(p_avg) + "s)")
plt.title("Time per execution")
plt.xlabel("Iteration")
plt.ylabel("Time (s)")
plt.ylim(ymin=0)

# Set plot legend 
plt.legend()

# Save plot
plt.savefig("compare.png")

# Display plot (depends on OS)
plt.show()





