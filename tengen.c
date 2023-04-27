#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <omp.h>

#define PRINT_DEBUG 1

/*
 Methods to generate normally distributed random variables are adapted from gennorm.c in
 https://cse.usf.edu/~kchriste/tools/gennorm.c 
 */

// ***** Function prototypes *****

// For ND random number generation


double norm_box_muller(double mean, double std_slice);
double rand_val(int seed);
double calculate_std(long *arr, int arr_size, double mean);
void print_vec ( long *array, int array_size);
void print_vec_double ( double *array, int array_size);
void *safe_malloc(int size);
void *safe_calloc(int count, int size);
void printusage();


int main(int argc, char *argv[])
{
	double time_start = omp_get_wtime();
	
	// int debug_cnt = 0;
	
	int input; 
	double density, density_fiber, cv_slice, cv_fiber;
	density = density_fiber = cv_slice = cv_fiber = 0.02;
	double random_seed = 1.0;
	int outfile_entered=0;
	
	int order=3;
	int dim [10];
	
	char outfile[200];
	
	if (argc <= optind)
		printusage();
	
	int  dim_0 = atoi(argv[1]);
	int  dim_1 = atoi(argv[2]);
	int  dim_2 = atoi(argv[3]);
	
	dim[0] = dim_0;
	dim[1] = dim_1;
	dim[2] = dim_2;
	
	while ((input = getopt(argc, argv, "i:j:k:d:f:c:v:r:o:")) != -1)
    {
		switch (input)
		{	
			case 'i':  dim[3] = atoi(optarg);
				order++;
				break;
				
			case 'j':  dim[4] = atoi(optarg);
				order++;
				break;
			
			case 'k':  dim[5] = atoi(optarg);
				order++;
				break;
			
			case 'd': 	density = atof(optarg);
				break;
			  
			case 'f': 	density_fiber = atof(optarg);
				break;
			  
			case 'c': 	cv_slice = atof(optarg);
				break;
				
			case 'v': 	cv_fiber = atof(optarg);
				break;
				
			case 'r':  random_seed = atof(optarg);
				break;
		
			case 'o':	sprintf(outfile, "%s", optarg);
				outfile_entered = 1;
				break;

		}
	}
	
	
    if (outfile_entered==0)
    {
		sprintf(outfile, "%s", "generated_tensor_");
        pid_t pid = getpid();
        char pid_str[16];
        snprintf(pid_str, sizeof(pid_str), "%d", pid);
        strcat(strcat(outfile, pid_str), ".tns");
	}
		
	double avg_nz_slice = density;
	for (int i=1; i< order; i++){
		avg_nz_slice *= dim[i];
	}
	
	long nnz = (long) (dim_0 * avg_nz_slice);
	double std_slice = cv_slice * avg_nz_slice;
	
	double avg_fib_per_slice = density_fiber * dim_1 ;
	long tot_fib_cnt = (long) ( avg_fib_per_slice * dim_0 );
	
    srand((int)random_seed);
    printf("______________\n");
	printf("omp_max_threads : \t%d \n", omp_get_max_threads());
    printf("dim_0: %d, dim_1: %d, dim_2: %d, Density: %f\n", dim_0, dim_1, dim_2, density);
	printf("______________\n");
    printf("Requested features:\n StDev_slice: \t %f\n", std_slice);
    printf(" coeff_var_slice : \t %.3f \n", cv_slice);
	printf(" nz_fiber_count: \t %ld\n NNZ: \t %ld \n Avg_nz_per_slice: \t %f \n", tot_fib_cnt, nnz, avg_nz_slice);
	printf("______________\n");
	
	if (std_slice > avg_nz_slice){
		std_slice = avg_nz_slice;
		printf ("\n std_slice > avg_nz_slice => Possibly negative values. We set std_slice = avg_nz_slice = %f. \n", std_slice );
		printf("______________\n");
	}

    rand_val(random_seed);
	
	// if(PRINT_DEBUG){ debug_cnt++; printf("DEBUG OK %d\n", debug_cnt);}

    long *nz_per_slice = safe_malloc(dim_0 * sizeof(long));
	int *fib_per_slice = safe_malloc(dim_0 * sizeof(int));
	
	#pragma omp parallel for
    for (int i = 0; i < dim_0; i++)
    {
        // Generate a normally distributed rv
        double norm_rv = round ( norm_box_muller(avg_nz_slice, std_slice) );
		
		// printf("%.1f ", norm_rv);

		if ( norm_rv < 1 )
			norm_rv = 1;
		
        nz_per_slice[i] = (long) norm_rv;
        
		
		fib_per_slice[i] = (int) ( ( norm_rv / nnz) * tot_fib_cnt );
		
    }
	
	double actual_std = calculate_std(nz_per_slice, dim_0, avg_nz_slice);
	printf("Resulting fetures:\n StDev_slice : \t\t %.3f \n", actual_std);
	printf(" coeff_var_slice : \t %.3f \n", actual_std / avg_nz_slice);

 	int **nz_fib_inds = (int **)safe_malloc(dim_0 * sizeof(int *));
	int **nz_per_nz_fiber = (int **)safe_malloc(dim_0 * sizeof(int *));
		
	#pragma omp parallel for
    for (int i = 0; i < dim_0; i++)
    {
		int *is_fib_nz = safe_calloc(dim_1, sizeof(int));

		int nz_fib_curr_slice = fib_per_slice[i];
		
		for (int j = 0; j < (int) (nz_fib_curr_slice * 1.1); j++) {
			int random_index = rand() % dim_1;
			is_fib_nz [random_index] = 1;
		}
		
		// update nz_fib_curr_slice
		nz_fib_curr_slice = 0;
		for (int j = 0; j < dim_1; j++) {
			nz_fib_curr_slice += is_fib_nz [j];
		}
		
		nz_fib_inds[i] = safe_malloc(nz_fib_curr_slice * sizeof(int)); //which fibs are nz
		fib_per_slice[i] = nz_fib_curr_slice;
		
		int curr_ind = 0;
		for (int j = 0; j < dim_1; j++) {
			if (is_fib_nz [j]){
				nz_fib_inds[i][curr_ind] = j;
				curr_ind++;
			}
		}
		
		double avg_nz_curr_fib = (nz_per_slice[i] + 0.0) / nz_fib_curr_slice;
		double std_fiber = cv_fiber * avg_nz_curr_fib;
		
		// if (std_fiber > avg_nz_curr_fib){
			// printf ("slice %d,  std_fiber (%f) > avg_nz_curr_fib (%f) \n", i, std_fiber, avg_nz_curr_fib );
			// std_fiber = avg_nz_curr_fib;
		// }
		
		nz_per_nz_fiber[i] = safe_malloc(nz_fib_curr_slice * sizeof(int));
		
		for (int j = 0; j < nz_fib_curr_slice; j++) {
			double norm_rv = round ( norm_box_muller(avg_nz_curr_fib, std_fiber));
			
			if ( norm_rv < 1 )
				norm_rv = 1;
		
			nz_per_nz_fiber[i][j] = norm_rv ;	
		}
		
		free ( is_fib_nz );
		
		// double actual_std_fib = calculate_std(nz_per_nz_fiber, nz_fib_curr_slice, avg_nz_curr_fib);
		// printf("Slice %ld fibers -> Resulting std : %.3f, ", i, actual_std_fib);
		// printf("Resulting cv : %.3f \n", actual_std_fib / avg_nz_curr_fib);
		
	}

	long *prefix_fib_per_slice = (long *)safe_calloc(dim_0+1 , sizeof(long ));
	for (long i = 0; i < dim_0; i++) {
		prefix_fib_per_slice[i+1] = prefix_fib_per_slice[i] + fib_per_slice[i];
	}
	
	long total_nz_fib_cnt = prefix_fib_per_slice[dim_0];
	printf(" nz_fiber_count: \t %ld\n", total_nz_fib_cnt);
	
	int **nz_indices_in_fib = (int **)safe_malloc(total_nz_fib_cnt * sizeof(int *));
	
	// if(PRINT_DEBUG){ debug_cnt++; printf("DEBUG OK %d\n", debug_cnt);}
	
	// printf("size of long %ld int %ld lonlong %ld\n",sizeof(long),sizeof(int),sizeof(long long));
		
	#pragma omp parallel for	
	for (int i = 0; i < dim_0; i++){
		
		int *is_nz_ind = safe_malloc(dim_2 * sizeof(int));
		
		int nz_fib_curr_slice = fib_per_slice[i];
		long prefix_start = prefix_fib_per_slice[i];
		for (int jth = 0; jth < nz_fib_curr_slice; jth++) {	//for each fiber in curr slice
			
			int nz_curr_fib_alloc = (int) (1.01 * nz_per_nz_fiber[i][jth]);
			long curr_fib_global = prefix_start + jth;
			nz_indices_in_fib[curr_fib_global] = (int *)safe_calloc( nz_curr_fib_alloc , sizeof(int));
			
			for (int k = 0; k < dim_2; k++){
				is_nz_ind [k] = 0;
			}
			
			//randomly fill nonzero values
			for (int k = 0; k < nz_curr_fib_alloc; k++){
				is_nz_ind [rand() % dim_2] = 1;
			}
			
			int local_nz_counter = 0;
			for (int k = 0; k < dim_2; k++) {
				if (is_nz_ind [k]){
					nz_indices_in_fib[curr_fib_global][local_nz_counter] = k ;
					local_nz_counter++;
				}
			}
		
			//randomly fill nonzero values
			// double density_curr_fiber = ( expected_nz_curr_fib + 0.0) / dim_2;
			// for (int k = 0; k < dim_2; k++){
				// double random_number = (double)rand() / RAND_MAX;
				// if (density_curr_fiber > random_number){
					// if (local_nz_counter >= nz_curr_fib_alloc ){
						// printf("\n Random nz fill gets out of limit !!!!! \n");
						// printf("k : %d, fib : %d, slice : %d, alloc : %d, counter : %d \n", k, jth, i, nz_curr_fib_alloc, local_nz_counter);
						// break;
					// }
					// else{
						// nz_indices_in_fib[curr_fib_global][local_nz_counter] = k ;
						// local_nz_counter++;
					// }
				// }
			// }
			
			// printf("nz_per_nz_fiber[i][jth] aim : %d, result : %d, diff : %d \n", nz_per_nz_fiber[i][jth], local_nz_counter, nz_per_nz_fiber[i][jth]- local_nz_counter);
			nz_per_nz_fiber[i][jth] = local_nz_counter;
		}
		free (is_nz_ind);
    }
	
	// if(PRINT_DEBUG){ debug_cnt++; printf("DEBUG OK %d\n", debug_cnt);}
	


	FILE *fptr;
	fptr = fopen(outfile, "w");
	fprintf(fptr, "%d\n", order);
	for (int i = 0; i<order; i++){
		fprintf(fptr, "%d ", dim[i]);
	}
	fprintf(fptr, "\n");
	
	// if(PRINT_DEBUG){ debug_cnt++; printf("DEBUG OK %d\n", debug_cnt);}
	
	long global_nz_counter = 0;
	for (int i = 0; i < dim_0; i++){
		int nz_fib_curr_slice = fib_per_slice[i];
		long prefix_start = prefix_fib_per_slice[i];
		for (int jth = 0; jth < nz_fib_curr_slice; jth++) {	//for each fiber in curr slice
			long curr_fib_global = prefix_start + jth;
			int local_nz_curr_fib = nz_per_nz_fiber[i][jth];
			for (int kth = 0; kth < local_nz_curr_fib; kth++){
				fprintf(fptr, "%d ", i+1);
				fprintf(fptr, "%d ", nz_fib_inds[i][jth]+1);
				fprintf(fptr, "%d ", nz_indices_in_fib[curr_fib_global][kth]+1);
				fprintf(fptr, "%f\n", (double)rand() / RAND_MAX * 2 - 1);
				global_nz_counter++;
			}
		}
    }
	fclose(fptr);
	
    printf(" NNZ: \t\t\t %ld\n", global_nz_counter);
	printf(" Avg_nz_per_slice: \t %f \n", (global_nz_counter+0.0) / dim_0);
	
	double time_end = omp_get_wtime();
	printf("total generator time : \t %.7f \n", time_end - time_start);

    return 0;
}

//===========================================================================
//=  Function to generate normally distributed random variable using the    =
//=  Box-Muller method                                                      =
//=    - Input: mean and standard deviation                                 =
//=    - Output: Returns with normally distributed random variable          =
//===========================================================================
double norm_box_muller(double mean, double std_slice)
{
    double u, r, theta; // Variables for Box-Muller method
    double x;           // Normal(0, 1) rv
    double norm_rv;     // The adjusted normal rv

    // Generate u
    u = 0.0;
    while (u == 0.0)
        u = rand_val(0);

    // Compute r
    r = sqrt(-2.0 * log(u));

    // Generate theta
    theta = 0.0;
    while (theta == 0.0){
        // theta = 2.0 * 3.14159265 * rand_val(0);
		theta = 6.2831853 * rand_val(0);
	}

    // Generate x value
    x = r * cos(theta);

    // Adjust x value for specified mean and variance
    norm_rv = (x * std_slice) + mean;

    // Return the normally distributed RV value
    return (norm_rv);
}

//=========================================================================
//= Multiplicative LCG for generating uniform(0.0, 1.0) random numbers    =
//=   - x_n = 7^5*x_(n-1)mod(2^31 - 1)                                    =
//=   - With x seeded to 1 the 10000th x value should be 1043618065       =
//=   - From R. Jain, "The Art of Computer Systems Performance Analysis," =
//=     John Wiley & Sons, 1991. (Page 443, Figure 26.2)                  =
//=========================================================================
double rand_val(int seed)
{
    const long a = 16807;      // Multiplier
    const long m = 2147483647; // Modulus
    const long q = 127773;     // m div a
    const long r = 2836;       // m mod a
    static long x;             // Random int value
    long x_div_q;              // x divided by q
    long x_mod_q;              // x modulo q
    long x_new;                // New x value

    // Set the seed if argument is non-zero and then return zero
    if (seed > 0)
    {
        x = seed;
        return (0.0);
    }

    // RNG using integer arithmetic
    x_div_q = x / q;
    x_mod_q = x % q;
    x_new = (a * x_mod_q) - (r * x_div_q);
    if (x_new > 0)
        x = x_new;
    else
        x = x_new + m;

    // Return a random value between 0.0 and 1.0
    return ((double)x / m);
}



double calculate_std(long *arr, int arr_size, double mean)
{

    double sqr_sum = 0;

#pragma omp parallel for reduction(+ : sqr_sum)
    for (int i = 0; i < arr_size; i++)
    {
        // if(arr[i]==0) continue;	// TT: already all entries nonzero !
        double mean_diff = arr[i] - mean;
        sqr_sum += mean_diff * mean_diff;
    }
	
    return sqrt(sqr_sum / arr_size);
}


void print_vec ( long *array, int array_size)
{
	printf ("array (size:%d) : [ ", array_size);
	for (int i = 0; i<array_size; i++){
		printf ("%ld ", array[i]);
	}
	printf ("] \n");
	
}

void print_vec_double ( double *array, int array_size)
{
	printf ("array (size:%d) : [ ", array_size);
	for (int i = 0; i<array_size; i++){
		printf ("%.1f ", array[i]);
	}
	printf ("] \n");
	
}

void *safe_malloc(int size)
{
    void *loc = malloc(size);
    if (loc == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    return loc;
}

void *safe_calloc(int count, int size)
{
    void *loc = calloc(count, size);
    if (loc == NULL)
    {
        printf("Memory (c)allocation failed.\n");
        exit(1);
    }

    return loc;
}



void printusage()
{
	printf("usage: tengen dim1 dim2 dim3 [options] \n");
	
	printf("\t-d density : nz ratio\n");
	printf("\t-f fiber_density : nz fiber ratio\n");
	printf("\t-c cv_slice : coefficient of variation for slices\n");
	printf("\t-v cv_fiber : coefficient of variation for fibers\n");
	printf("\t-r random_seed : seed for randomness \n");
	printf("\t-o outfile : to print out the generated tensor \n");

	exit(1);
}