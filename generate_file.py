import numpy as np

np.random.seed(2023)

N_DATA = 200000
MAX_VAL = 1000000

random_array = np.random.randint(MAX_VAL, size = N_DATA)

f = open("merge_sort_input.txt", "w")

for i in range(N_DATA):
    f.write(str(random_array[i]))

    if (i != N_DATA-1):
        f.write("\n")
f.close()