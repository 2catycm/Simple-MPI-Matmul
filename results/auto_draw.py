#%%
# 恢复数据
import os, subprocess
from pathlib import Path
import pandas as pd
import numpy as np
from tqdm import tqdm
import time
this_file = Path(__file__).resolve()
this_directory = this_file.parent
result_directory= this_directory/'results'
figure_directory = this_directory/'figures'
if not figure_directory.exists(): figure_directory.mkdir()
single_vm_dir = this_directory/'single_vm'
multi_vm_dir = this_directory/'multi_vm'

single_exp_times = pd.read_excel(single_vm_dir/'result_experiment_times.xlsx', index_col=0)
single_total_time = pd.read_excel(single_vm_dir/'result_times.xlsx', index_col=0)

multi_exp_times = pd.read_excel(multi_vm_dir/'result_experiment_times.xlsx', index_col=0)
multi_total_time = pd.read_excel(multi_vm_dir/'result_times.xlsx', index_col=0)

multi_total_time.head(6)
#%%
# 计算平均时间
single_avg_time = single_total_time / single_exp_times
multi_avg_time  = multi_total_time / multi_exp_times
multi_avg_time.head(6)

#%%
# 绘制下降趋势
import matplotlib.pyplot as plt

# plt.figure()
# for mat_size in multi_avg_time.index:
#     plt.scatter(multi_avg_time.columns, multi_avg_time.loc[mat_size, :], label=f"mat_size={mat_size}")
#     # plt.scatter(, fitness, c='lawngreen', marker='x')
#     # plt.plot(x, fitness, c='salmon')

# plt.xlabel(f"Variable{i}")
# plt.ylabel("fitness")
# if self.xnames is not None:
#     plt.xticks(x, self.xnames[i])
# plt.title(f"When vars={[i.item() for i in hh[0, :]]}\nExplores the influence of var{i} on fitness")
# plt.savefig(Path(self.draw_path)/f"{self.problem.name}_round{round}_var{i}{_current_time()}.png")     
# plt.close()

def draw(avg_time, figure_name = ''):
    plt.figure()
    for mat_size in avg_time.index:
        # plt.scatter(avg_time.columns, avg_time.loc[mat_size, :], label=f"mat_size={mat_size}")
        plt.scatter(avg_time.columns, avg_time.loc[mat_size, :])
        plt.plot(avg_time.columns, avg_time.loc[mat_size, :], label=f"mat_size={mat_size}")
    plt.legend()
    plt.ylabel("time/s")
    plt.xlabel("#processes")
    plt.xticks(avg_time.columns)
    
        
    plt.title("How time changes with increasing #processes under different matrix sizes")
    plt.savefig(figure_directory/f"{figure_name}.png")
    plt.close()
draw(single_avg_time, 'single_vm_plot')
draw(multi_avg_time, 'multi_vm_plot')

# 部分绘图
draw(single_avg_time.head(3), 'single_vm_plot_small')
draw(multi_avg_time.head(3), 'multi_vm_plot_small')
draw(single_avg_time.tail(3), 'single_vm_plot_big')
draw(multi_avg_time.tail(3), 'multi_vm_plot_big')

#%%
# 分析单机和多机两种方法的时间是否有显著差异
# 具体方法：使用 Wilcoxon signed-rank test
from scipy.stats import wilcoxon

single_avg_time_flatten = single_avg_time.values.flatten()
multi_avg_time_flatten = multi_avg_time.values[:, :-2].flatten()
single_avg_time_flatten.shape, multi_avg_time_flatten.shape

# res = wilcoxon(single_avg_time_flatten, multi_avg_time_flatten)
res = wilcoxon(single_avg_time_flatten[:-2], multi_avg_time_flatten[:-2])
print(res)
if res.pvalue<0.01:
    print("拒绝原假设，两种方法的时间有显著差异")

#%%
# 检验是否具有平稳性，从而证明scalability。
# 具体方法：矩阵大小x2，进程数x8， 时间应当具有平稳性, 因为O(n^3)的复杂度