import os, subprocess
from pathlib import Path
import pandas as pd
import numpy as np
from tqdm import tqdm
import time
this_file = Path(__file__).resolve()
this_directory = this_file.parent
out_directory= this_directory/'out'
if not out_directory.exists():
    out_directory.mkdir()

method_name = "mpi_matmul_peer_pattern"

source = this_directory / f"{method_name}.cpp"
processes = [2**i for i in range(6)]
mat_sizes = [125*2**i for i in range(6)]

# mpicxx = "mpic++" 
# mpicxx = "mpiicpc" 
mpicxx = "mpigxx" 

times = np.zeros((len(mat_sizes), len(processes)))
experiment_times = np.zeros((len(mat_sizes), len(processes)))
errors = np.zeros((len(mat_sizes), len(processes)))
# 在所有的矩阵大小和进程数下运行mpi程序
for i, mat_size in tqdm(enumerate(mat_sizes)):
    # 先编译相应矩阵大小的程序
    executable = this_directory/'out' / f"{method_name}_{mat_size}.exe"
    if not executable.exists():
        compile_command = f"{mpicxx} -O3 -o {executable.as_posix()} {source.as_posix()} -DMAT_SIZE={mat_size} -std=c++17"
        if os.system(compile_command)!=0:
            raise ValueError(f"Compile failed, command was {compile_command}")
    for j, process in enumerate(processes):
        command = f"mpiexec -np {process} {executable.as_posix()} {mat_size}"
        # max_time = 1
        max_time = 100 # 预计一小时跑完
        start_time = time.time()
        while time.time()-start_time<max_time:
            pro = subprocess.run([command], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
            outputs = pro.stderr.decode("UTF-8")
            # with open("running_log.txt", "a") as f:
            #     f.write(outputs)
            #     f.write(pro.stdout.decode("UTF-8"))
            # 取出运行时间
            try:
                lines = outputs.splitlines()
                line = list(filter(lambda x:x.startswith("Done in") and x.endswith('seconds.'), lines))
                items = line[0].split()
                times[i, j] += float(items[2])
                # print(f"item[2] = {items[2]}")
                # 取出误差
                line = list(filter(lambda x:x.startswith("Error:"), lines))
                items = line[0].split()
                error = float(items[1])
                errors[i, j] += error
                experiment_times[i,j]+=1
            except Exception as e:
                print(f"在矩阵大小{mat_size}和进程数{process}下运行失败，输出为{outputs}")
                print(e)
                break
        # print(f"本次运行了{time.time()-start_time}s, 实现了{experiment_times[i,j]}次实验, 矩阵计算用时{times[i, j]}。")
        
# df = pd.DataFrame(data, index=mat_sizes, columns=processes)
df = pd.DataFrame(times, index=mat_sizes, columns=processes)
df.to_excel('result_times.xlsx')
df = pd.DataFrame(errors, index=mat_sizes, columns=processes)
df.to_excel('result_errors.xlsx')
df = pd.DataFrame(experiment_times, index=mat_sizes, columns=processes)
df.to_excel('result_experiment_times.xlsx')

        
