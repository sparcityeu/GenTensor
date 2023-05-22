# Tensor-Generator


Use the command "./compile.sh" to compile the program.

Usage: 

./tengen dim1 dim2 dim3 [options]                                                                                                                                
	-d density : nz ratio                                                                                                                                         
	-f fiber_density : nz fiber ratio                                                                                                                             
	-c cv_slice : coefficient of variation for slices                                                                                                             
	-v cv_fiber : coefficient of variation for fibers                                                                                                             
	-r random_seed : seed for randomness                                                                                                                          
	-o outfile : to print out the generated tensor

An example run command will be like the following:

./tengen 23776223 22476 22476 -d 0.00000000237 -f 0.0000516 -c 0.05788 -v 0.00475 -o generated_1998darpa.tns
./tengen 1000 1000 1000 -d 0.0001 -f 0.01 -c 0.5 -v 0.5 -o generated_1000.tns
