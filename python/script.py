import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys
import os 
from pathlib import Path
from typing import List, Dict, Tuple

def csv_to_dict(file: str) -> Dict:
  """ Transforms csv file into a Dict.
  """
  df = pd.read_csv(file)
  dic = {}
  set_of_threads = set(df["nthreads"])
  for n in set_of_threads:
    dic[n] = []

  for i in range(len(df)) : 
    dic[df.iloc[i, 0]].append(df.iloc[i, 1]) 
  return dic      

def plot_errorplot(plt, n_thread: List, mean : List, std : List, args: Dict) -> None:
  """ Adds an error plot to the plt argument.
  """
  (_, caps, _) = plt.errorbar(n_thread, mean, std, **args)
  for cap in caps:
    cap.set_markeredgewidth(2)
  
  current_xmin, current_xmax = plt.xlim() 
  current_ymin, current_ymax = plt.ylim() 
  plt.xlim( 
            min( current_xmin, min(n_thread)),
            max( current_xmax, max(n_thread))
          ) 
  plt.ylim( 
            0, 
            max( current_ymax, max(mean) + (max(std)))
          ) 
  return None

def data_processing(dict_: Dict) -> Tuple[List, List, List] :
  """ Computes the avg, and the std 
  """
  n_thread, mean, std = [], [], []

  for key in sorted(dict_.keys()):
    n_thread.append(key)
    values = dict_[key]
    mean.append(np.mean(values))
    std.append(np.std(values))
  return n_thread, mean, std

def plot_csv_to_errplot(path_to_csv, plt, args):
  """ Extract, information and computes the metrics needed then plots an errorplot using the plt argument
      and the args given.
  """
  dict_ = csv_to_dict(path_to_csv)
  x, y, std = data_processing(dict_)
  plot_errorplot(plt, x, y, std, args)

if __name__ == "__main__":
  path_to_file = sys.argv[1]
  path_parent = Path(path_to_file).parent.parent
  filename = os.path.basename(path_to_file)
  plt.style.use('seaborn')
  plot_csv_to_errplot(path_to_file, plt, { 'fmt': 'o', 'linestyle':':', 'color':'b', 'markersize':8, 'capsize':3, 'ecolor':'r'})

  targeted_problem = filename.split('.')[0]

  plt.title(f"Average and Standard Deviation time per thread ({targeted_problem})", pad=20)
  plt.xlabel("Number of threads")
  plt.ylabel("Average Execution Time (sec)")
  plt.legend([targeted_problem])
  path_to_graphs = os.path.join(path_parent, 'graphs')
  plt.savefig(os.path.join(path_to_graphs, targeted_problem+'.png'))