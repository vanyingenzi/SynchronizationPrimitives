# Project 1 - Operating Systems [LINFO1252]
## Description 
For the course LINFO1252 - Operating Systems given at UCLouvain to undergraduate Computer Scientists, we are were asked to do a study project on synchronisation primitives, mutexes and semaphores. The details about the study project are detailed in the report PDF, this README is to discuss the strcture of the code. I recommend to first read the report then the code, for better understanding. 

## Folder Structure
```
|csv/
|graphs/
|headers/
|python/
|src/
```

* `csv/`        : Contains all the CSV files that are the outputs of the metrics measurements made by the **Makefile**.
* `graphs/`     : Contains all the graphs that are going to be plotted by the **Makefile**.
* `headers/`    : This directory holds user defined headers needed by the compiler when compiling executables.
* `python/`     : Holds the python script that are going to generate the plots.
* `src/`        : This directory contains the C Programming code files.

### File Naming

Source files and executables have the following naming pattern, <implementation_primitives>_<problem>.<optional_extension>. For example, when compiling for the philosopher's problem you going to have the three following executables : 

- tas_philosopher
- posix_philosopher
- tatas_philosopher

Where **_tas_** stand for Test-And-Set and **_tatas_** stands for Test-And-Test-And-Set. 


## Makefile
Below you will find a table with all the possible **Makefile** commands and their corresponding functionalities.

| command          | functionalities                                                                                               |
| :--------------- | :------------------------------------------------------------------------------------------------------------ |
| make             | Compiles all the known executables in the `src` files.                                                        |
| make clean       | Cleans all the known executables in the root directory and all the objects in the `src/` directory.           |
| make benchmark   | Runs all the synchronisation problems and the various implementations, then outputs the executime in a CSV file in the `csv/` directory that holds the same name as the problem. |
| make `-B` graphs | Generates graphs for the each CSV files in the `csv/`, and saves the graph as png files in `graphs` directory. And also generates the comparison graphs if their respective CSV files are available. The `-B` is a flag to force the command to execute. |

## Chart Generation
One can easily generate graphs and put them in `graphs/` directory by using the python script `compare.py` in `python/` directory. The command takes as command line arguments to csv that have the two columns, **_nthreads_** and **_real_time_**. The python script requires **matplotlib**, **numpy** and **pandas** as modules.

`python compare <file_1> <file_2> ... <file_3>`

## Changing the code

Be aware when changing source files, cause there's some preprocessing macros that depend on the order of imports and which headers are include. This is the case for our own primitives implementation, and this was done to minimize code reduction in some way.

