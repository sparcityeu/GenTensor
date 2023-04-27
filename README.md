# Matrix-Generator

Work in progress, comprehensive readme will be added soon.

***

Use the command "gcc matgen.c -lm" to compile the program.

In this version, the executable takes precisely 6 command line arguments as input. These are row count, column count, density, standard deviation, random seed, and write-out flag. 

Here, density should be a value between 0 and 1. Note that the current state of the program does not support efficient dense matrix generation. So the value should be kept below 0.5. 

An example run command will be like the following:

"./a.out 10000 10000 0.01 50 23452656 1"

Here, we are asking the program to generate a 10000 x 10000 matrix with %1 density, and the standard deviation of the non-zero counts of rows will be 50. Note that the mean non-zero counts of rows are calculated using row and column counts and density information.

The last integer, the write-out flag, tells the program to whether store the generated matrix in a .mtx file or not. If it is 1, the program will generate a file named "Generated_Matrix_[id].mtx" where the [id] is the process id of the execution, if 0 it will just print the matrix to the console. 
