# Tensor-Generator


Use the command "./compile.sh" to compile the program.

Usage: 

./genten sizes[] [options]                                                                                                                               
	-d density : nonzero ratio                                                                                                                                         
	-f density_fiber : nonzero fiber ratio for mode-(M) fibers                                                                                                                             
	-s density_slice : nonzero slice ratio for mode-(M-1,M) slices
	-c cv_fib_per_slc : coefficient of variation of fiber per slice values for mode-(M) fibers and mode-(M-1,M) slices 	
	-v cv_nz_per_fib : coefficient of variation of nonzero per fiber values for mode-(M) fibers  
	-i imbal_fib_per_slc : imbalance of fiber per slice values for mode-(M) fibers and mode-(M-1,M) slices. ( imbalance = max/avg-1 )
	-b imbal_nz_per_fib : imbalance of nonzero per fiber values for mode-(M) fibers. ( imbalance = max/avg-1 )	
	-r random_seed : seed for randomness                                                                                                                          
	-o outfile : to print out the generated tensor
	-h print_header : to print the header names for the output values 
	-p print_debug : to print at some main steps for debugging
	-w write_tensor : to write the generated tensor into a file

An example run command will be like the following:

./genten 3 100 100 100 -d 0.01 -f 0.1 -c 0.5 -v 0.5 -o ../../../Desktop/tensor/tns/generated_100_3D.tns
./genten 4 100 100 100 100 -d 0.0001 -f 0.1 -c 0.5 -v 0.5 -o ../../../Desktop/tensor/tns/generated_100_4D.tns
./genten 5 10 20 30 40 50 -d 0.0001 -f 0.001 -s 0.01 -c 0.5 -v 0.5 -o ../../../Desktop/tensor/tns/generated_5d.tns

./genten 3 10000 15000 20000 -d 2E-09 -f 0.0002 -v 23.1 -s 0.8 -c 13.0 -o ../../../Desktop/tensor/tns/sample_generated.tns;

A list of commands to generate tensors with same features of real tensors is in file generate_example.sh.
