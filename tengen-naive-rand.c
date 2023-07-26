#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <omp.h>

#define PRINT_DEBUG 1
#define AVG_SCALE 0.8
#define PRINT_HEADER 0


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
	double density;
	density = 0.02;
	int random_seed = 1;
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
				
			case 'r':  random_seed = atoi(optarg);
				break;
		
			case 'o':	sprintf(outfile, "%s", optarg);
				outfile_entered = 1;
				break;
		}
	}
		
    if (outfile_entered==0)
    {
		sprintf(outfile, "%s", "naive_rand_");
        pid_t pid = getpid();
        char pid_str[16];
        snprintf(pid_str, sizeof(pid_str), "%d", pid);
        strcat(strcat(outfile, pid_str), ".tns");
	}
		
	
	long total = 1;
	for (int i=0; i< order; i++){
		total *= dim[i];
	}
	
	long nnz = (long) (total * density);
	
	if (PRINT_HEADER){
		printf("name \t seed \t dim_0 \t dim_1 \t dim_2 \t ");
		printf("REQST \t density \t nnz \t ");
		printf("TIME \t time_write \t time_total \n");
	}
	
	printf("%s \t %d \t %d \t %d \t %d \t ", outfile, random_seed, dim_0, dim_1, dim_2);
	printf("REQST \t %g \t %ld \t ", density, nnz);
	
	srand(random_seed);

	// double* values = safe_malloc(nnz * sizeof(double));
	// int** indices = safe_malloc(nnz * sizeof(int*));
	
	// #pragma omp parallel for
    // for (int i = 0; i < nnz; i++)
    // {
		// indices[i] = safe_malloc(order * sizeof(int));
		
        // for (int j = 0; j < order; j++)
            // indices[i][j] = rand() % dim[j] +1;
		
		// values[i] = (double)rand() / RAND_MAX;
    // }
	
	double time_start1 = omp_get_wtime();
	
	FILE *fptr;
	fptr = fopen(outfile, "w");
	if( fptr == NULL ) {
		printf ("\n *** ERROR WHILE OPENING OUT FILE ! *** \n\n");
		exit(1);  
	}
	
	fprintf(fptr, "%d\n", order);
	for (int i = 0; i<order; i++){
		fprintf(fptr, "%d ", dim[i]);
	}
	fprintf(fptr, "\n");
	
	
	for (int i = 0; i < nnz; i++)
    {
        for (int j = 0; j < order; j++)
            fprintf(fptr, "%d ", rand() % dim[j] +1);

        fprintf(fptr, "%lf\n", (double)rand() / RAND_MAX);
    }
	
	fclose(fptr);
	

	double time_end = omp_get_wtime();
	

	printf("TIME \t %.7f \t %.7f \n ", time_end - time_start1, time_end - time_start);
	

    return 0;
}

//===========================================================================
//=  Function to generate normally distributed random variable using the    =
//=  Box-Muller method                                                      =
//=    - Input: mean and standard deviation                                 =
//=    - Output: Returns with normally distributed random variable          =
//===========================================================================
double norm_box_muller(double mean, double stdev, int seed_bm)
{
    double u, r, theta; // Variables for Box-Muller method
    double x;           // Normal(0, 1) rv
    double norm_rv;     // The adjusted normal rv
	
	unsigned int mystate = seed_bm * 10;

    // Generate u
    u = 0.0;
    while (u == 0.0){
        // u = rand_val(0);
		u = (double)rand_r(&mystate) / RAND_MAX;
		// u = (double)rand) / RAND_MAX;
	}

    // Compute r
    r = sqrt(-2.0 * log(u));
	
	mystate = floor(mean) * seed_bm;

    // Generate theta
    theta = 0.0;
    while (theta == 0.0){
        // theta = 2.0 * 3.14159265 * rand_val(0);
		// theta = 6.2831853 * rand_val(0);
		// theta = 6.2831853 * rand() / RAND_MAX;
		theta = 6.2831853 * rand_r(&mystate) / RAND_MAX;
	}

    // Generate x value
    x = r * cos(theta);

    // Adjust x value for specified mean and variance
    norm_rv = (x * stdev) + mean;

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
/*
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
*/

//=========================================================================
//= Multiplicative LCG for generating uniform(0.0, 1.0) random numbers    =
//=   - x_n = 7^5*x_(n-1)mod(2^31 - 1)                                    =
//=   - With x seeded to 1 the 10000th x value should be 1043618065       =
//=   - From R. Jain, "The Art of Computer Systems Performance Analysis," =
//=     John Wiley & Sons, 1991. (Page 443, Figure 26.2)                  =
//=========================================================================
/*
int rand_val_int(int seed, int limit)
{
    const long a = 16807;      // Multiplier
    const long m = 2147483647; // Modulus
    const long q = 127773;     // m div a
    const long r = 2836;       // m mod a
    static long x_int;             // Random int value
    long x_div_q;              // x_int divided by q
    long x_mod_q;              // x_int modulo q

    // Set the seed if argument is non-zero and then return zero
    if (seed > 0){
        x_int = seed;
        return (0.0);
    }

    // RNG using integer arithmetic
    x_div_q = x_int / q;
    x_mod_q = x_int % q;
    x_int = (a * x_mod_q) - (r * x_div_q);

    // Return a random value between 0.0 and 1.0
    return (int) floor ( (double)x_int / m * limit );
}
*/


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
	
	printf("\t-d density : nonzero ratio\n");
	printf("\t-f fiber_density : nonzero fiber ratio on mode-(0,1) fibers \n");
	printf("\t-c cv_fib_per_slc : coefficient of variation for fiber per slice on mode-(0,1) fibers and mode-0 slices\n");
	printf("\t-v cv_nz_per_fib : coefficient of variation for nonzero per fiber on mode-(0,1) fibers\n");
	printf("\t-r random_seed : seed for randomness \n");
	printf("\t-o outfile : to print out the generated tensor \n");

	exit(1);
}