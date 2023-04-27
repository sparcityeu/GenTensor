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

./tengen 23776223 22476 22476 -d 0.0000002 -f 0.00516 -c 1.195986 -v 372.1 -o small_darpa.tns
