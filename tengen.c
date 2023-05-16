#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <omp.h>

#define PRINT_DEBUG 1
#define AVG_SCALE 0.8

/*
 Methods to generate normally distributed random variables are adapted from gennorm.c in
 https://cse.usf.edu/~kchriste/tools/gennorm.c 
 */

double norm_box_muller(double mean, double std_fib_per_slc);
double rand_val(int seed);
double calculate_std(int *arr, int arr_size, double mean);
void print_vec ( long *array, int array_size);
void print_vec_double ( double *array, int array_size);
void *safe_malloc(int size);
void *safe_calloc(int count, int size);
void printusage();

int main(int argc, char *argv[])
{
	double time_start = omp_get_wtime();

	int input; 
	double density, density_fiber, cv_fib_per_slc, cv_nz_per_fib;
	density = density_fiber = cv_fib_per_slc = cv_nz_per_fib = 0.02;
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
			  
			case 'c': 	cv_fib_per_slc = atof(optarg);
				break;
				
			case 'v': 	cv_nz_per_fib = atof(optarg);
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
		
	double avg_nz_per_slc = density;
	for (int i=1; i< order; i++){
		avg_nz_per_slc *= dim[i];
	}
	
	long nnz = (long) (dim_0 * avg_nz_per_slc);
	
	double avg_fib_per_slc = density_fiber * dim_1 ;
	double std_fib_per_slc = cv_fib_per_slc * avg_fib_per_slc;
	
	long tot_fib_cnt = (long) ( avg_fib_per_slc * dim_0 );
	
	double avg_nz_per_fib = (nnz + 0.0) / tot_fib_cnt;
	double std_nz_per_fib = cv_nz_per_fib * avg_nz_per_fib;
	
	
    srand((int)random_seed);
	
	printf("INFO: name \t dim_0 \t dim_1 \t dim_2 \t density \t density_fiber \t cv_fib_per_slc \t cv_nz_per_fib \n");
	printf("INFO: %s \t %d \t %d \t %d \t %f \t %f \t %f \t %f\n", outfile, dim_0, dim_1, dim_2, density, density_fiber, cv_fib_per_slc, cv_nz_per_fib);
	// printf("dim_0: %d, dim_1: %d, dim_2: %d, density: %f, density_fiber: %f\n", dim_0, dim_1, dim_2, density, density_fiber, cv_fib_per_slc, cv_nz_per_fib);
    
	printf("omp_max_threads : \t%d \n", omp_get_max_threads());
	// printf("dim_0: %d, dim_1: %d, dim_2: %d, density: %f, density_fiber: %f\n", dim_0, dim_1, dim_2, density, density_fiber, cv_fib_per_slc, cv_nz_per_fib);
	printf("______________\n");
    // printf("Requested features:\n ");
    // printf(" cv_fib_per_slc : \t %.3f \n", cv_fib_per_slc);
	// printf(" std_fib_per_slc: \t %.3f\n", std_fib_per_slc);
	// printf(" avg_fib_per_slc: \t %.1f \n", avg_fib_per_slc);
	// printf(" nz_fiber_count: \t %ld\n NNZ: \t %ld \n", tot_fib_cnt, nnz);
	// printf("______________\n");

	printf("FEAT:  std_fib_per_slc \t avg_fib_per_slc \t tot_fib_cnt \t std_nz_per_fib \t avg_nz_per_fib \t nnz \n");
	printf("REQST: %.3f \t %.1f \t %ld \t %.3f \t %.1f \t %ld \n", std_fib_per_slc, avg_fib_per_slc, tot_fib_cnt, std_nz_per_fib, avg_nz_per_fib, nnz);
	
    rand_val(random_seed);

	int *fib_per_slice = safe_malloc(dim_0 * sizeof(int));
	int **nz_fib_inds = (int **)safe_malloc(dim_0 * sizeof(int *));

	double time_start1 = omp_get_wtime();

	#pragma omp parallel
    {
		int *is_fib_nz = safe_malloc(dim_1 * sizeof(int));	
		#pragma omp for
		for (int i = 0; i < dim_0; i++) {
			// Generate a normally distributed rv
			int fib_curr_slice =  (int) round (  norm_box_muller( avg_fib_per_slc, std_fib_per_slc ) );

			if ( fib_curr_slice < 1 ){
				fib_curr_slice = 1;
			}
			
			for (int j = 0; j < dim_1; j++){
				is_fib_nz [j] = 0;
			}
			
			for (int j = 0; j < fib_curr_slice; j++) {
				is_fib_nz [rand() % dim_1] = 1;
			}
			
			nz_fib_inds[i] = safe_malloc(fib_curr_slice * sizeof(int)); //which fibs are nz
			
			fib_curr_slice = 0;
			for (int j = 0; j < dim_1; j++) {
				if (is_fib_nz [j]){
					nz_fib_inds[i][fib_curr_slice] = j;
					fib_curr_slice++;
				}
			}
			
			fib_per_slice[i] = fib_curr_slice;
			// printf("%d %d \n" ,i, fib_curr_slice);
		}
		free ( is_fib_nz );
	}
	
	
	
	double time_fib_per_slc = omp_get_wtime() - time_start1;
	
	long *prefix_fib_per_slice = (long *)safe_calloc(dim_0+1 , sizeof(long ));
	for (long i = 0; i < dim_0; i++) {
		prefix_fib_per_slice[i+1] = prefix_fib_per_slice[i] + fib_per_slice[i];
	}
	
	tot_fib_cnt = prefix_fib_per_slice[dim_0];
	avg_fib_per_slc = (tot_fib_cnt + 0.0) / dim_0;
	std_fib_per_slc = calculate_std(fib_per_slice, dim_0, avg_fib_per_slc);
	
	// printf("Resulting fetures:\n StDev_fib_per_slice : \t\t %.3f \n", actual_std);
	// printf(" cv_fib_per_slice : \t %.3f \n", actual_std / avg_fib_per_slc);
	// printf(" nz_fiber_count: \t %ld\n", tot_fib_cnt);

	int **nz_indices_in_fib = (int **)safe_malloc(tot_fib_cnt * sizeof(int *));
	int *nz_per_fiber = (int *)safe_malloc(tot_fib_cnt * sizeof(int *));
	
	time_start1 = omp_get_wtime();
	
	#pragma omp parallel
    {
		int *is_nz_ind = safe_malloc(dim_2 * sizeof(int));
		#pragma omp for
		for (int j = 0; j < tot_fib_cnt; j++){
			int nz_curr_fib = (int) round ( norm_box_muller(avg_nz_per_fib, std_nz_per_fib));
				
			if ( nz_curr_fib < 1 ){
				nz_curr_fib = 1;
			}

			for (int k = 0; k < dim_2; k++){
				is_nz_ind [k] = 0;
			}
			
			//randomly fill nonzero values
			for (int k = 0; k < nz_curr_fib; k++){
				is_nz_ind [rand() % dim_2] = 1;
			}
			
			nz_indices_in_fib[j] = (int *)safe_calloc( nz_curr_fib , sizeof(int));
			
			nz_curr_fib = 0;
			for (int k = 0; k < dim_2; k++) {
				if (is_nz_ind [k]){
					nz_indices_in_fib[j][nz_curr_fib] = k ;
					nz_curr_fib++;
				}
			}
			nz_per_fiber[j] = nz_curr_fib;
		}
		free(is_nz_ind);
	}
	
	double time_nz_per_fib = omp_get_wtime() - time_start1;
	
	long *prefix_nz_per_fiber = (long *)safe_calloc(tot_fib_cnt+1 , sizeof(long ));
	for (int j = 0; j < tot_fib_cnt; j++){
		prefix_nz_per_fiber[j+1] = prefix_nz_per_fiber[j] + nz_per_fiber[j];
	}
	
	nnz = prefix_nz_per_fiber [tot_fib_cnt];
	avg_nz_per_fib = (nnz + 0.0) / tot_fib_cnt;
	std_nz_per_fib = calculate_std(nz_per_fiber, tot_fib_cnt, avg_nz_per_fib);
	
	int *ind_0 = safe_malloc(nnz * sizeof(int));
	int *ind_1 = safe_malloc(nnz * sizeof(int));
	int *ind_2 = safe_malloc(nnz * sizeof(int));
	
	time_start1 = omp_get_wtime();
	
	#pragma omp parallel for
	for (int i = 0; i < dim_0; i++){
		int fib_curr_slice = fib_per_slice[i];
		long prefix_fib_start = prefix_fib_per_slice[i];
		for (int j = 0; j < fib_curr_slice; j++) {	//for each fiber in curr slice
			long curr_fib_global = prefix_fib_start + j;
			int local_nz_curr_fib = nz_per_fiber[curr_fib_global];
			long prefix_nz_start = prefix_nz_per_fiber[curr_fib_global];
			for (int k = 0; k < local_nz_curr_fib; k++){
				long curr_nz_global = k + prefix_nz_start;
				ind_0[curr_nz_global] = i;
				ind_1[curr_nz_global] = nz_fib_inds[i][j];
				ind_2[curr_nz_global] = nz_indices_in_fib[curr_fib_global][k];
			}
		}
    }
	
	double time_nz_ind = omp_get_wtime() - time_start1;
	
	// printf(" NNZ: \t\t\t %ld\n", nnz);
	// printf(" Avg_nz_per_slice: \t %f \n", (nnz+0.0) / dim_0);
	
	printf("RESLT: %.3f \t %.1f \t %ld \t %.3f \t %.1f \t %ld \n", std_fib_per_slc, avg_fib_per_slc, tot_fib_cnt, std_nz_per_fib, avg_nz_per_fib, nnz);
	
	time_start1 = omp_get_wtime();
		
	FILE *fptr;
	fptr = fopen(outfile, "w");
	fprintf(fptr, "%d\n", order);
	for (int i = 0; i<order; i++){
		fprintf(fptr, "%d ", dim[i]);
	}
	fprintf(fptr, "\n");

	// #pragma omp parallel for
	for (int n = 0; n < nnz; n++){
		fprintf(fptr, "%d ", ind_0[n]+1);
		fprintf(fptr, "%d ", ind_1[n]+1);
		fprintf(fptr, "%d ", ind_2[n]+1);
		fprintf(fptr, "%f\n", (double)rand() / RAND_MAX * 2 - 1);
    }
	
	fclose(fptr);

	double time_end = omp_get_wtime();

	printf("______________\n");
	printf("time_fib_per_slc  : \t %.7f \n", time_fib_per_slc);
	printf("time_nz_per_fib  : \t %.7f \n", time_nz_per_fib);
	printf("time_nz_ind  : \t\t %.7f \n", time_nz_ind);
	printf("time_write  : \t\t %.7f \n", time_end - time_start1);
	printf("time_total  : \t\t %.7f \n", time_end - time_start);

    return 0;
}

//===========================================================================
//=  Function to generate normally distributed random variable using the    =
//=  Box-Muller method                                                      =
//=    - Input: mean and standard deviation                                 =
//=    - Output: Returns with normally distributed random variable          =
//===========================================================================
double norm_box_muller(double mean, double std_fib_per_slc)
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
    norm_rv = (x * std_fib_per_slc) + mean;

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



double calculate_std(int *arr, int arr_size, double mean)
{

    double sqr_sum = 0;
	
	// long sum = 0;
	// #pragma omp parallel for reduction(+ : sum)
	// for (int i = 0; i < arr_size; i++) {
        // sum += arr[i];
    // }
	// double mean = (sum+0.0) / arr_size;

	#pragma omp parallel for reduction(+ : sqr_sum)
    for (int i = 0; i < arr_size; i++) {
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
	printf("\t-c cv_fib_per_slc : coefficient of variation for slices\n");
	printf("\t-v cv_nz_per_fib : coefficient of variation for fibers\n");
	printf("\t-r random_seed : seed for randomness \n");
	printf("\t-o outfile : to print out the generated tensor \n");

	exit(1);
}