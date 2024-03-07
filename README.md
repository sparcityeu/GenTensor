# Tensor-Generator
The **genTen** program generates sparse tensors with several given features.
It adopts a combination of normal distribution and log-normal distribution.

To compile the program, use the command
```
./compile.sh
```

USAGE: 

```
./genten order sizes[] [options] 
		
	-d density : nonzero ratio [default: 0.001]
	-f density_fiber : nonzero fiber ratio for mode-(M) fibers [default: 0.01]
	-s density_slice : nonzero slice ratio for mode-(M-1,M) slices [default: 1.0]
	-c cv_fib_per_slc : coefficient of variation of fiber per slice values for mode-(M) fibers and mode-(M-1,M) slices [default: 0.5]
	-v cv_nz_per_fib : coefficient of variation of nonzero per fiber values for mode-(M) fibers [default: 0.5]
	-i imbal_fib_per_slc : imbalance of fiber per slice values for mode-(M) fibers and mode-(M-1,M) slices. ( imbalance = max/avg-1 ) [default: max = size]
	-b imbal_nz_per_fib : imbalance of nonzero per fiber values for mode-(M) fibers. ( imbalance = max/avg-1 ) [default: max = size]
	-r random_seed : seed for randomness [default: 1] 
	-h print_header : whether print the header names for explaining the output values (1:yes, 0:no[default])
	-p print_debug : whether print details at some steps for debugging (1:yes, 0:no[default]) 
	-w write_tensor : whether write the generated tensor into a file or not (1:yes[default], 0:no) 
	-o outfile : the name of the output file to print out the generated tensor 

```

Example run commands will be like the following:

```
./genten 3 100 100 100 -d 0.01 -f 0.1 -c 0.5 -v 0.5 -o generated_100_3D.tns
./genten 4 100 100 100 100 -d 0.0001 -f 0.1 -c 0.5 -v 0.5 -o generated_100_4D.tns
./genten 5 10 20 30 40 50 -d 0.0001 -f 0.001 -s 0.01 -c 0.5 -v 0.5 -o generated_5d.tns
./genten 3 10000 15000 20000 -d 2E-09 -f 0.0002 -v 23.1 -s 0.8 -c 13.0 -o sample_generated.tns;
```

The output of the program is in the form (which can be printed by typing "-h 1" option) :
```
name seed order dim_0 dim_1 dim_2 slc_type nz_slc_cnt requested result density_slc requested result ratio distr_type no max_fib_per_slc requested result ratio imbal_fib_per_slc requested result ratio std_fib_per_slc requested result ratio cv_fib_per_slc requested result ratio avg_fib_per_slc requested result ratio nz_fib_cnt requested result density_fib requested result ratio_prev ratio_last distr_type no max_nz_per_fib requested result ratio imbal_nz_per_fib requested result ratio std_nz_per_fib requested result ratio cv_nz_per_fib requested result ratio avg_nz_per_fib requested result ratio nnz requested result density requested result ratio_prev ratio_last threads TIME time_slc time_fib time_nz time_nz_ind time_write time_total
```

**generate_real_tensor_siblings.sh** contains a list of commands to generate tensors using genTen with the same features as real tensors (in FROSTT and HaTeN2).
**generate_example.sh** is an extendended version of this, which allows generating sibling tensors with different seeds.

The **genten_naive_rand** program generates simple naive random tensors with uniform diftribution, this is just for comparison.
