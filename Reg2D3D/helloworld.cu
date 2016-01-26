// This is the REAL "hello world" for CUDA!
// It takes the string "Hello ", prints it, then passes it to CUDA with an array
// of offsets. Then the offsets are added in parallel to produce the string "World!"
// By Ingemar Ragnemalm 2010
 
#include <stdio.h>
 
const int N = 16; 
const int blocksize = 16; 
 
__global__ 
void hello(char *a, int *b) 
{
	a[threadIdx.x] += b[threadIdx.x];
}
 

void cuda_hello(char *ac, int* bc, int n)
{
//	char a[N] = "Hello \0\0\0\0\0\0";
//	int b[N] = {15, 10, 6, 0, -11, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
 
	char *ad;
	int *bd;
	const int csize = n*sizeof(char);
	const int isize = n*sizeof(int);
 
	printf("C:%s --> ", ac);
 
	cudaMalloc( (void**)&ad, csize ); 
	cudaMalloc( (void**)&bd, isize ); 
	cudaMemcpy( ad, ac, csize, cudaMemcpyHostToDevice ); 
	cudaMemcpy( bd, bc, isize, cudaMemcpyHostToDevice ); 
	
	dim3 dimBlock( n, 1 );
	dim3 dimGrid( 1, 1 );
        hello<<<dimGrid, dimBlock>>>(ad, bd);
//	hello<<<N, 1>>>(ad, bd);
	cudaMemcpy( ac, ad, csize, cudaMemcpyDeviceToHost ); 
	cudaFree( ad );
	cudaFree( bd );
	
	printf("C: %s\n", ac);

}

