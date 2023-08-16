# Tensor-Generator


Use the command "./compile.sh" to compile the program.

Usage: 

./genten dim1 dim2 dim3 [options]         
	-i dim4 : 4th dimension (if any)
	-j dim5 : 5th dimension (if any) 
	-k dim6 : 6th dimension (if any)                                                                                                                       
	-d density : nz ratio                                                                                                                                         
	-f fiber_density : nz fiber ratio                                                                                                                             
	-c cv_slice : coefficient of variation for slices                                                                                                             
	-v cv_fiber : coefficient of variation for fibers                                                                                                             
	-r random_seed : seed for randomness                                                                                                                          
	-o outfile : to print out the generated tensor

An example run command will be like the following:

./genten 100 100 100 -d 0.01 -f 0.1 -c 0.5 -v 0.5 -o ../../../Desktop/tensor/tns/generated_100_3D.tns
./genten 100 100 100 -i 100 -d 0.0001 -f 0.1 -c 0.5 -v 0.5 -o ../../../Desktop/tensor/tns/generated_100_4D.tns
