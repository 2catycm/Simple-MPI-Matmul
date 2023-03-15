#%%
import os, subprocess
from pathlib import Path
import pandas as pd
import numpy as np
from tqdm import tqdm
import time
this_file = Path(__file__).resolve()
this_directory = this_file.parent
out_directory= this_directory

processes = [2**i for i in range(8)] # 一直到2^7 = 128
mat_sizes = [125*2**i for i in range(6)]

times = np.zeros((len(mat_sizes), len(processes)))
experiment_times = np.zeros((len(mat_sizes), len(processes)))

times = times.flatten()
experiment_times = experiment_times.flatten()
#%%
with open(out_directory/'25633.out') as f:
    lines = f.readlines()
lines = list(filter(lambda x:x.startswith('在矩阵大小'), lines))

for i, line in enumerate(lines):
    exp_times = float(line.split('下运行成功，跑了')[1].split('次')[0])
    mat_time = float(line.split('矩阵计算用时')[1])
    experiment_times[i] = exp_times
    times[i] = mat_time
    

#%%
times = times.reshape((len(mat_sizes), len(processes)))
experiment_times = experiment_times.reshape((len(mat_sizes), len(processes)))
df = pd.DataFrame(times, index=mat_sizes, columns=processes)
df.to_excel('result_times.xlsx')
df = pd.DataFrame(experiment_times, index=mat_sizes, columns=processes)
df.to_excel('result_experiment_times.xlsx')
# %%
