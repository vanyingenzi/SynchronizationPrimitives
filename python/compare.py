import matplotlib.pyplot as plt
import sys
import os 
from pathlib import Path
import script

__ecolors    = [ 'teal',  'k',   'orange']
__colors     = [ 'r',     'g',   'b']
__linestyles = [ ':', '-.',   '--']
__fmts       = [ 'o',   '^']
 
if __name__ == "__main__":
  try :
    paths = sys.argv[1:len(sys.argv)]
    path_to_parent = Path(paths[0]).parent.parent
    filenames = [ os.path.basename(path) for path in paths ]
    plt.style.use('seaborn')

    styles = [
                { 
                  'fmt'     : __fmts[(i*i)%len(__fmts)]     ,'linestyle'    :__linestyles[i%len(__linestyles)],
                  'color'   :__colors[i%len(__colors)]      , 'markersize'  :8                          , 
                  'capsize' :3                              , 'ecolor'      :__ecolors[i%len(__ecolors)]
                }

                for i in range(0, len(sys.argv))   
            ] 
    
    for idx in range(len(paths)):
      script.plot_csv_to_errplot(paths[idx], plt, styles[idx])
    
    target_problems = [ filename.split('.')[0] for filename in filenames ]
    plt.title(f"Average and Standard Deviation time per thread \n"
              f"( { str([ target_problem for target_problem in target_problems ])[1:-1] } )", 
              pad=20 )
    plt.xlabel("Number of threads")
    plt.ylabel("Average Execution Time (sec)")

    plt.legend(target_problems)
    path_to_graphs = os.path.join(path_to_parent, 'graphs')
    graph_name = str([ target_problem for target_problem in target_problems ])[1:-1]
    graph_name = graph_name.replace(", ", "-vs-").replace("'", "")
    plt.savefig(os.path.join(path_to_graphs, graph_name +'.png'))
  except Exception as _:
    pass