#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <omp.h>

#define ULLI unsigned long long int
#define USHI unsigned short int 
#define INTSIZE 268435456


/*
 Methods to generate normally distributed random variables are adapted from gennorm.c in
 https://cse.usf.edu/~kchriste/tools/gennorm.c 
 */

// double norm_box_muller(double mean, double stdev);
double norm_box_muller(double mean, double stdev, int seed_bm);
double calculate_std(int *arr, int arr_size, double mean);
void print_vec ( ULLI *array, int array_size);
void print_vec_double ( double *array, int array_size);
void *safe_malloc(size_t size);
void *safe_calloc(size_t count, size_t size);
void printusage();

int main(int argc, char *argv[])
{
	double time_start = omp_get_wtime();

	int input; 
	double density, density_slice, density_fiber, cv_fib_per_slc, cv_nz_per_fib;
	double density_requested, density_slice_requested, density_fiber_requested, cv_fib_per_slc_requested, cv_nz_per_fib_requested;
	
	double avg_fib_per_slc, std_fib_per_slc, avg_nz_per_fib, std_nz_per_fib;
	double avg_fib_per_slc_requested, std_fib_per_slc_requested, avg_nz_per_fib_requested, std_nz_per_fib_requested;
	
	ULLI slc_cnt_total, fib_cnt_total;
	ULLI nnz, nz_fib_cnt, nz_slc_cnt;
	ULLI nnz_requested, nz_fib_cnt_requested, nz_slc_cnt_requested;

	density = 0.001;
	density_fiber = 0.01;
	density_slice = 1.0;
	
	cv_fib_per_slc = cv_nz_per_fib = 0.5;
	
	int random_seed = 1;
	int outfile_entered=0;
	int print_header = 0;
	int print_debug = 0;
	int write_tensor = 1;

	char outfile[200];
	
	if (argc <= optind)
		printusage();
	
	int order = atoi(argv[1]);
	
	int *dim = (int *) safe_malloc(order * sizeof(int));
	
	for (int i = 0; i<order; i++){
		dim[i]= atoi(argv[i+2]);
	}
	
	
	while ((input = getopt(argc, argv, "d:s:f:c:v:r:o:h:p:w:")) != -1)
    {
		switch (input)
		{	
			
			case 'd': 	density = atof(optarg);
				break;
				
			case 's': 	density_slice = atof(optarg);
				break;
			  
			case 'f': 	density_fiber = atof(optarg);
				break;
			  
			case 'c': 	cv_fib_per_slc = atof(optarg);
				break;
				
			case 'v': 	cv_nz_per_fib = atof(optarg);
				break;
				
			case 'r':  random_seed = atoi(optarg);
				break;
		
			case 'o':	sprintf(outfile, "%s", optarg);
				outfile_entered = 1;
				break;
			
			case 'h':  print_header = atoi(optarg);
				break;
			
			case 'p':  print_debug = atoi(optarg);
				break;
				
			case 'w':  write_tensor = atoi(optarg);
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
	
	srand(random_seed);
	
	
	if (print_header){
		printf("name \t seed \t order \t ");
		
		for (int i = 0; i< order; i++){
			printf("dim_%d \t ", i);
		}
		
		printf("slc_type \t ");
		
		printf("REQST \t density_slc \t density_fib \t density \t cv_fib_per_slc \t cv_nz_per_fib \t std_fib_per_slc \t std_nz_per_fib \t avg_fib_per_slc \t avg_nz_per_fib \t SLC_TYPE \t slc_type \t ");
		printf("RESLT \t density_slc \t density_fib \t density \t cv_fib_per_slc \t cv_nz_per_fib \t std_fib_per_slc \t std_nz_per_fib \t avg_fib_per_slc \t avg_nz_per_fib \t ");
		printf("threads \t TIME \t time_slc \t time_fib \t time_nz \t time_nz_ind \t time_write \t time_total \n");
	}
	
	printf("%s \t %d \t %d \t ", outfile, random_seed, order);
	
	for (int i = 0; i< order; i++){
		printf("%d \t ", dim[i]);
	}
			
	int id_first = dim[order-2];
	int id_second = dim[order-1];
	
	slc_cnt_total = 1;
	
	for (int i = 0; i< order-2; i++){
		slc_cnt_total *= dim[i];
	}
	
	fib_cnt_total = slc_cnt_total * id_first;
	
	nnz = (ULLI) (density * id_second * fib_cnt_total );
	
	nz_slc_cnt = (ULLI) ( density_slice * slc_cnt_total );

	nz_fib_cnt = (ULLI) ( density_fiber * fib_cnt_total );
	
	avg_fib_per_slc = (nz_fib_cnt+0.0) / nz_slc_cnt ;
	std_fib_per_slc = cv_fib_per_slc * avg_fib_per_slc;

	avg_nz_per_fib = (nnz + 0.0) / nz_fib_cnt;
	std_nz_per_fib = cv_nz_per_fib * avg_nz_per_fib;

	nnz_requested = nnz;
	nz_fib_cnt_requested = nz_fib_cnt;
	nz_slc_cnt_requested = nz_slc_cnt;
	density_slice_requested = density_slice;
	density_fiber_requested = density_fiber;
	density_requested = density;
	cv_fib_per_slc_requested = cv_fib_per_slc;
	cv_nz_per_fib_requested = cv_nz_per_fib;
	avg_fib_per_slc_requested = avg_fib_per_slc;
	std_fib_per_slc_requested = std_fib_per_slc;
	avg_nz_per_fib_requested = avg_nz_per_fib;
	std_nz_per_fib_requested = std_nz_per_fib;
	
	
	if (density_slice > 0.97) {
		density_slice = 1.0;
		nz_slc_cnt = slc_cnt_total;
	}
	
	
	// if (std_fib_per_slc < 1){
		// std_fib_per_slc = 1.0;
	// }
	
	// if (std_nz_per_fib < 1){
		// std_nz_per_fib = 1.0;
	// }
	
	
	ULLI nz_slc_cnt_max = nz_slc_cnt;
	
	//for memory and number accuracy
	if (density_slice < 1.0 && density_slice > 0.1) {
		nz_slc_cnt_max *= 1.1;
	}
	
	
	int **nz_slc_inds = (int **) safe_malloc((order-2) * sizeof(int*));
	for (int i = 0; i < order-2; i++){
		nz_slc_inds[i] = (int *) safe_malloc(nz_slc_cnt_max * sizeof(int));
	}
	
	if (print_debug) printf(" \n ***SLICE_START \n ");
	

	//time for slice count
	double time_start1 = omp_get_wtime();
	
	USHI slc_category = 0;
	
	if (density_slice == 1.0) {	// meaning nz_slc_cnt = slc_cnt_total . assign slice indices in sorted order
		
		slc_category = 1;
		
		if (order == 3){
			
			#pragma omp parallel for
			for (ULLI j = 0; j < nz_slc_cnt; j++) {
				nz_slc_inds[0][j] = j;
			}
		}
		else if (order == 4){	
			
			int divider = dim [1];
			
			#pragma omp parallel for
			for (ULLI j = 0; j < nz_slc_cnt; j++) {
				nz_slc_inds[1][j] = j % divider;
				nz_slc_inds[0][j] = j / divider;
			}
		}
			
		else if (order == 5){
				
			int divider = dim[2];
			ULLI divider_big = (ULLI) dim[2] * dim[1];
			
			#pragma omp parallel for	
			for (ULLI j = 0; j < nz_slc_cnt; j++) {
				nz_slc_inds[2][j] = j % divider;
				nz_slc_inds[1][j] = j / divider;
				nz_slc_inds[0][j] = j / divider_big;
			}
		}
		
		else{	// valid for n-dim
			
			int divider = dim [order-3];
			
			#pragma omp parallel for
			for (ULLI j = 0; j < nz_slc_cnt; j++) {
				nz_slc_inds[order-3][j] = j % divider;
				nz_slc_inds[order-4][j] = j / divider;
			}
			
			ULLI divider_big = (ULLI) divider;

			for (int i = order-5; i >= 0; i--) {	
				divider_big *= dim [i+1];		
			
				#pragma omp parallel for
				for (ULLI j = 0; j < nz_slc_cnt; j++) {
					nz_slc_inds[i][j] = j / divider_big;
				}
			}
		}
	}
	
	
	else if (density_slice > 0.5) {		// assumes is_slc_empty of size slc_cnt_total can fit into memory
		
		slc_category = 2; 
		
		// determine the slice indices that are empty
		USHI *is_slc_empty = (USHI *) safe_calloc(slc_cnt_total, sizeof(USHI));	

		// select the empty slices instead of nonzeros because they are less
		ULLI empty_slc_cnt = (slc_cnt_total - nz_slc_cnt) * 1.03;
		
		// unsigned int mystate = random_seed * order + nz_slc_cnt;
		
		for (ULLI j = 0; j < empty_slc_cnt; j++) {	
			is_slc_empty [rand() % slc_cnt_total] = 1;
		}
		
		nz_slc_cnt = 0;
		
		if (order == 3){
			for (ULLI j = 0; j < slc_cnt_total; j++) {
				if (is_slc_empty [j] == 0){
					nz_slc_inds[0][nz_slc_cnt] = j;
					nz_slc_cnt++;
				}
			}
		}
		
		else if (order == 4){		

			int divider = dim [1];
			
			for (ULLI j = 0; j < slc_cnt_total; j++) {
				if (is_slc_empty [j] == 0){
					nz_slc_inds[1][nz_slc_cnt] = j % divider;
					nz_slc_inds[0][nz_slc_cnt] = j / divider;
					nz_slc_cnt++;
				}
			}
		}
		
		else if (order == 5){		
			
			int divider = dim [2];
			ULLI divider_big = (ULLI) dim [2] * dim [1];
			
			for (ULLI j = 0; j < slc_cnt_total; j++) {
				if (is_slc_empty [j] == 0 ){
					nz_slc_inds[2][nz_slc_cnt] = j % divider;
					nz_slc_inds[1][nz_slc_cnt] = j / divider;
					nz_slc_inds[0][nz_slc_cnt] = j / divider_big;
					nz_slc_cnt++;
				}
			}
		}
			
		else{	// valid for n-dim
			
			int divider = dim [order-3];
			
			for (ULLI j = 0; j < slc_cnt_total; j++) {
				if (is_slc_empty [j] == 0 ){
					nz_slc_inds[order-3][nz_slc_cnt] = j % divider;
					nz_slc_inds[order-4][nz_slc_cnt] = j / divider;
					nz_slc_cnt++;
				}
			}
			
			ULLI divider_big = (ULLI) divider;

			for (int i = order-5; i >= 0; i--) {	
				divider_big *= dim [i+1];		
			
				for (ULLI j = 0; j < slc_cnt_total; j++) {
					if (is_slc_empty [j] == 0 ){
						nz_slc_inds [i][nz_slc_cnt] = j / divider_big;
						nz_slc_cnt++;
					}
				}
			}
		}
		free (is_slc_empty);
	}
	
	else if (density_slice > 0.1) { 

		slc_category = 3;
		
		nz_slc_cnt = 0;
		
		if (order == 3){
			for (ULLI j = 0; j < slc_cnt_total; j++) {
				double random_number = (double)rand() / RAND_MAX;
				if (density_slice >= random_number) {
					nz_slc_inds[0][nz_slc_cnt] = j;
					nz_slc_cnt++;
				}
			}
		}
		
		else if (order == 4){
			int divider = dim [1];
			for (ULLI j = 0; j < slc_cnt_total; j++) {
				double random_number = (double)rand() / RAND_MAX;
				if (density_slice >= random_number) {
					nz_slc_inds[1][nz_slc_cnt] = j % divider;
					nz_slc_inds[0][nz_slc_cnt] = j / divider;
					nz_slc_cnt++;
				}
			}
		}
		
		else if (order == 5){		
			
			int divider = dim [2];
			ULLI divider_big = (ULLI) dim [2] * dim [1];
			
			for (ULLI j = 0; j < slc_cnt_total; j++) {
				double random_number = (double)rand() / RAND_MAX;
				if (density_slice >= random_number) {
					nz_slc_inds[2][nz_slc_cnt] = j % divider;
					nz_slc_inds[1][nz_slc_cnt] = j / divider;
					nz_slc_inds[0][nz_slc_cnt] = j / divider_big;
					nz_slc_cnt++;
				}
			}
		}
		
		else{	// valid for n-dim
			
			int divider = dim [order-3];
			
			for (ULLI j = 0; j < slc_cnt_total; j++) {
				double random_number = (double)rand() / RAND_MAX;
				if (density_slice >= random_number) {
					nz_slc_inds[order-3][nz_slc_cnt] = j % divider;
					nz_slc_inds[order-4][nz_slc_cnt] = j / divider;
					
					ULLI divider_big = (ULLI) divider;
					
					for (int i = order-5; i >= 0; i--) {	
						divider_big *= dim [i+1];		
						nz_slc_inds[i][nz_slc_cnt] = j / divider_big;	
					}
					nz_slc_cnt++;
				}
			}
		}
	}
	
	else{ // means density_slice < 0.1
	
		slc_category = 4;
		
		int curr_dim;
		
		//#pragma omp parallel for
		for (int i = 0; i < order-2; i++) {
			curr_dim = dim[i];
			for (ULLI j = 0; j < nz_slc_cnt; j++) {
				nz_slc_inds[i][j] = rand() % curr_dim;
			}
		}
		
	}
	
	double time_nz_slc = omp_get_wtime() - time_start1;
	
	density_slice = (double) nz_slc_cnt / slc_cnt_total;
	
	printf("%d \t ", slc_category);
	
	printf("nz_slc_cnt \t %llu \t %llu \t %g \t ", nz_slc_cnt_requested, nz_slc_cnt, (nz_slc_cnt+0.0)/nz_slc_cnt_requested );
	printf("density_slc \t %g \t %g ", density_slice_requested, density_slice);
		
	if (print_debug) printf(" \n ***SLICE_DONE \n ");
	
	
	//update with changed nz_slc_cnt
	avg_fib_per_slc = (nz_fib_cnt_requested + 0.0 ) / nz_slc_cnt ;
	std_fib_per_slc = cv_fib_per_slc * avg_fib_per_slc;

	int *fib_per_slice = safe_malloc(nz_slc_cnt * sizeof(int));
	int **nz_fib_inds = (int **)safe_malloc(nz_slc_cnt * sizeof(int *));

	time_start1 = omp_get_wtime();

	#pragma omp parallel
    {
		USHI *is_fib_nz = (USHI *) safe_malloc(id_first * sizeof(USHI));	
		#pragma omp for
		for (ULLI i = 0; i < nz_slc_cnt; i++) {
			// Generate a normally distributed rv
			int fib_curr_slice =  (int) floor (  norm_box_muller( avg_fib_per_slc, std_fib_per_slc, random_seed*(i+1) ) );
			

			if ( fib_curr_slice < 1 || fib_curr_slice > id_first){
				fib_curr_slice = 1;
			}
			
			for (int j = 0; j < id_first; j++){
				is_fib_nz [j] = 0;
			}
			
			unsigned int mystate = random_seed * (i+1) + fib_curr_slice;
			
			for (int j = 0; j < fib_curr_slice; j++) {
				is_fib_nz [rand_r(&mystate) % id_first] = 1;
				// is_fib_nz [rand() % dim_1] = 1;
				// is_fib_nz [ (int) floor (rand_val(0) * dim_1) ] = 1;
				// is_fib_nz [ rand_val_int(0, dim_1) ] = 1;
			}
			
			nz_fib_inds[i] = safe_malloc(fib_curr_slice * sizeof(int)); //which fibs are nz
			
			fib_curr_slice = 0;
			for (int j = 0; j < id_first; j++) {
				if (is_fib_nz [j]){
					nz_fib_inds[i][fib_curr_slice] = j;
					fib_curr_slice++;
				}
			}
			
			fib_per_slice[i] = fib_curr_slice;
		}
		free ( is_fib_nz );
	}
	
	
	double time_fib_per_slc = omp_get_wtime() - time_start1;

	
	ULLI *prefix_fib_per_slice = (ULLI *)safe_calloc(nz_slc_cnt+1 , sizeof(ULLI ));
	for (ULLI i = 0; i < nz_slc_cnt; i++) {
		prefix_fib_per_slice[i+1] = prefix_fib_per_slice[i] + fib_per_slice[i];
	}
	
	nz_fib_cnt = prefix_fib_per_slice[nz_slc_cnt];
	density_fiber = (nz_fib_cnt + 0.0) / fib_cnt_total;
	avg_fib_per_slc = (nz_fib_cnt + 0.0) / nz_slc_cnt;
	std_fib_per_slc = calculate_std(fib_per_slice, nz_slc_cnt, avg_fib_per_slc);
	cv_fib_per_slc = (double) std_fib_per_slc / avg_fib_per_slc;
		
	printf("nz_fib_cnt \t %llu \t %llu \t %g \t ", nz_fib_cnt_requested, nz_fib_cnt, (nz_fib_cnt+0.0)/nz_fib_cnt_requested );
	printf("density_fib \t %g \t %g \t ", density_fiber_requested, density_fiber);	
	printf("avg_fib_per_slc \t %g \t %g \t %g \t ", avg_fib_per_slc_requested, avg_fib_per_slc, (avg_fib_per_slc+0.0)/avg_fib_per_slc_requested );
	printf("std_fib_per_slc \t %g \t %g \t %g \t ", std_fib_per_slc_requested, std_fib_per_slc, (std_fib_per_slc+0.0)/std_fib_per_slc_requested );
	printf("cv_fib_per_slc \t %g \t %g \t %g \t ", cv_fib_per_slc_requested, cv_fib_per_slc, (cv_fib_per_slc+0.0)/cv_fib_per_slc_requested );
	
	if (print_debug) printf(" \n ***FIBER_DONE \n ");
	
	//update with changed nz_fib_cnt
	avg_nz_per_fib = (nnz + 0.0) / nz_fib_cnt;
	std_nz_per_fib = cv_nz_per_fib * avg_nz_per_fib;

	int **nz_indices_in_fib = (int **)safe_malloc(nz_fib_cnt * sizeof(int *));
	int *nz_per_fiber = (int *)safe_malloc(nz_fib_cnt * sizeof(int *));
	
	time_start1 = omp_get_wtime();
	
	#pragma omp parallel
    {
		USHI *is_nz_ind = (USHI*) safe_malloc(id_second * sizeof(USHI));
		#pragma omp for
		for (int j = 0; j < nz_fib_cnt; j++){
			int nz_curr_fib = (int) round ( norm_box_muller(avg_nz_per_fib, std_nz_per_fib, random_seed*(j+10)));
			
			if ( nz_curr_fib < 1 || nz_curr_fib > id_second){
				nz_curr_fib = 1;
			}

			for (int k = 0; k < id_second; k++){
				is_nz_ind [k] = 0;
			}
			
			unsigned int mystate = random_seed * (j+5) + nz_curr_fib;
			
			//randomly fill nonzero values
			for (int k = 0; k < nz_curr_fib; k++){
				is_nz_ind [rand_r(&mystate) % id_second] = 1;
				// is_nz_ind [rand() % dim_2] = 1;
				// is_nz_ind [ (int) floor (rand_val(0) * dim_2) ] = 1;
				// is_nz_ind [ rand_val_int(0, dim_2) ] = 1;
			}
			
			nz_indices_in_fib[j] = (int *)safe_calloc( nz_curr_fib , sizeof(int));
			
			nz_curr_fib = 0;
			for (int k = 0; k < id_second; k++) {
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
	

	ULLI *prefix_nz_per_fiber = (ULLI *)safe_calloc(nz_fib_cnt+1 , sizeof(ULLI ));
	for (ULLI j = 0; j < nz_fib_cnt; j++){
		prefix_nz_per_fiber[j+1] = prefix_nz_per_fiber[j] + nz_per_fiber[j];
	}
	
	nnz = prefix_nz_per_fiber [nz_fib_cnt];
	density =  ( (nnz + 0.0) / fib_cnt_total ) / id_second ;
	avg_nz_per_fib = (nnz + 0.0) / nz_fib_cnt;
	std_nz_per_fib = calculate_std(nz_per_fiber, nz_fib_cnt, avg_nz_per_fib);
	cv_nz_per_fib = (double) std_nz_per_fib / avg_nz_per_fib;
	
		
	printf("nnz \t %llu \t %llu \t %g \t ", nnz_requested, nnz, (nnz+0.0)/nnz_requested );
	printf("density \t %g \t %g \t ", density_requested, density);
	printf("avg_nz_per_fib \t %g \t %g \t %g \t ", avg_nz_per_fib_requested, avg_nz_per_fib, (avg_nz_per_fib+0.0)/avg_nz_per_fib_requested );
	printf("std_nz_per_fib \t %g \t %g \t %g \t ", std_nz_per_fib_requested, std_nz_per_fib, (std_nz_per_fib+0.0)/std_nz_per_fib_requested );
	printf("cv_nz_per_fib \t %g \t %g \t %g \t ", cv_nz_per_fib_requested, cv_nz_per_fib, (cv_nz_per_fib+0.0)/cv_nz_per_fib_requested );

	
	if (print_debug) printf(" \n ***NONZERO_DONE \n ");
	
	time_start1 = omp_get_wtime();
	
	
	int **ind = (int **) safe_malloc(order * sizeof(int*));
	for (int i = 0; i < order; i++){
		ind[i] = (int *) safe_malloc(nnz * sizeof(int));
	}
	
	#pragma omp parallel for
	for (ULLI i = 0; i < nz_slc_cnt; i++){	//for each nonzero slice
		int fib_curr_slice = fib_per_slice[i];
		ULLI prefix_fib_start = prefix_fib_per_slice[i];
		for (int j = 0; j < fib_curr_slice; j++) {	//for each fiber in curr slice
			ULLI curr_fib_global = prefix_fib_start + j;
			int local_nz_curr_fib = nz_per_fiber[curr_fib_global];
			ULLI prefix_nz_start = prefix_nz_per_fiber[curr_fib_global];
			for (int k = 0; k < local_nz_curr_fib; k++){	//for each nz in curr fiber
				ULLI curr_nz_global = k + prefix_nz_start;
				ind[order-2][curr_nz_global] = nz_fib_inds[i][j];
				ind[order-1][curr_nz_global] = nz_indices_in_fib[curr_fib_global][k];
				for (int m = 0; m <order-2; m++){
					ind[m][curr_nz_global] = nz_slc_inds[m][i];	// assign the indices of the current nz slice
				}
			}
		}
    }
	
	double time_nz_ind = omp_get_wtime() - time_start1;
	
	if (print_debug) printf(" \n ***IND_DONE \n ");
	
	
	time_start1 = omp_get_wtime();
	
	if (write_tensor){
		
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
		
		for (int n = 0; n < nnz; n++){
			// fprintf(fptr, "%d %d %d ", ind_0[n]+1, ind_1[n]+1, ind_2[n]+1);
			for (int i = 0; i < order; i++){
				fprintf(fptr, "%d ", ind[i][n]+1);
			}
			fprintf(fptr, "%.1f\n", (rand() % 9 + 1.0) / 10 );	// random numbers between 0.1 and 0.9 
			// fprintf(fptr, "%.1f\n", (double)rand() / RAND_MAX + 0.1 );
		}
		
		fclose(fptr);
	}

	double time_end = omp_get_wtime();
	
	
	printf("%d \t TIME \t %.7f \t %.7f \t %.7f \t %.7f \t %.7f \t %.7f \n ", omp_get_max_threads(), time_nz_slc, time_fib_per_slc, time_nz_per_fib, time_nz_ind, time_end - time_start1, time_end - time_start);
	

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


double calculate_std(int *arr, int arr_size, double mean)
{

    double sqr_sum = 0;
	
	// ULLI sum = 0;
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


void print_vec ( ULLI *array, int array_size)
{
	printf ("array (size:%d) : [ ", array_size);
	for (int i = 0; i<array_size; i++){
		printf ("%llu ", array[i]);
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

void *safe_malloc(size_t size)
{
    void *loc = malloc(size);
    if (loc == NULL)
    {
        printf(" genten.c : safe_malloc : Memory (%lu = %lu GB) could not be allocated\n", size, size/ INTSIZE);
        exit(1);
    }

    return loc;
}

void *safe_calloc(size_t count, size_t size)
{
    void *loc = calloc(count, size);
    if (loc == NULL)
    {
        printf(" genten.c : safe_calloc : Memory (%lu = %lu GB) could not be (c)allocated\n", size, size/ INTSIZE);
        exit(1);
    }

    return loc;
}


void printusage()
{
	printf("usage: ./genten order sizes[] [options] \n");
	
	printf("\t-d density : nonzero ratio\n");
	printf("\t-f density_fiber : nonzero fiber ratio for mode-(M) fibers \n");
	printf("\t-s density_slice : nonzero slice ratio for mode-(M-1,M) slices \n");
	printf("\t-c cv_fib_per_slc : coefficient of variation of fiber per slice values for mode-(M) fibers and mode-(M-1,M) slices\n");
	printf("\t-v cv_nz_per_fib : coefficient of variation of nonzero per fiber values for mode-(M) fibers\n");
	printf("\t-r random_seed : seed for randomness \n");
	printf("\t-o outfile : to print out the generated tensor \n");
	printf("\t-h print_header : to print the header names for the output values \n");
	printf("\t-p print_debug : to print at some main steps for debugging \n");
	printf("\t-w write_tensor : to write the generated tensor into a file in COO format \n");

	exit(1);
}