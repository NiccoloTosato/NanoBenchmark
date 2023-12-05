#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/resource.h>
#include <immintrin.h>
#include <sys/time.h>
#if defined(__AVX512__)
//Size of the type, bit
#define VSIZEb 512
//Size in double
#define VDSIZE 8
//Size in float
#define VFSIZE 16

#elif defined(__AVX__) || defined(__AVX2__)

#define VSIZEb 256
#define VDSIZE 4
#define VFSIZE 8

#elif defined(__SSE4_1__) || defined(__SSE4_2__) || defined(__SSE3__) || defined(__SSE2__) || defined(__SSE__)

#define VSIZEb 128
#define VDSIZE 2
#define VFSIZE 4

#endif


// ------------------------------------------------------------------

#define CONCAT(x,y) x ## y
//Questo operatore concatena durante il preprocessing gli argomendi: es CONCAT(3,3) -> 33
// ─────────────────────────────────────────────────────────────────
// define the vector generator
//

#define DEFINE_VECT( T, N, NAME ) typedef T v##NAME __attribute__((vector_size( sizeof(T) * N))); typedef union { v##NAME vector; T s[N]; } u##NAME;

//Perform the experiments with FLOAT or DOUBLE
#if defined(FLOAT)
DEFINE_VECT( float, VFSIZE, d );
#define SIZE  VFSIZE
#elif defined(DOUBLE)
DEFINE_VECT( double, VDSIZE, d );
#define SIZE VDSIZE
#endif

//Define how many vectorial units are available
#ifndef AVX_COUNT
#define AVX_COUNT 1
#endif

/*
the macro DEFINE_VECT( double, 8, d) expands in:

typedef double vd __attribute__((vector_size( sizeof(double)*8 )));
typedef union {vd vector; double s[8];} ud;
*/
#include <cpuid.h>
//#include <sched.h>
#include <unistd.h>
#include <sys/syscall.h>

int get_cpu(){
  //thisc can be achieved using also the sched.h function
  int cpuid;
  if ( syscall( SYS_getcpu, &cpuid, NULL, NULL ) == -1 )
    return -1;
  else
    return cpuid;
}

int get_freq(int cpu) {
  char filename[70];
  sprintf(filename,"/sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq",cpu);
  FILE* file;
  file=fopen(filename,"r");
  int i = 0;
  fscanf (file, "%d", &i);
  fclose (file);
  return i;
  
}
int main ( int argc, char **argv ) {

  //int cpuInfo[4] = { 0, 0, 0, 0 };
  //  __cpuid( 0x16, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
  //  printf("Base freq: %d\n",cpuInfo[0]);
  printf("Running on core %d\n",get_cpu());
  printf("Cur freq: %d\n",get_freq(get_cpu()));

  int exp = (argc > 1 ? atoi(*(argv+1)) : 2 );
  size_t N = pow(10,exp);
  struct timeval start_time;
  struct timeval end_time;


  printf("Vector size appears to be %dbits\n"
         "Experimenting with %ld data points\nAVX_COUNT %d\n",
         VSIZEb, N,AVX_COUNT);
  //tipo vettoriale per inizializzare ! 
  register vd vec2_a = {0};
  register vd vec2_b = {1};
  register vd vec2_c = {3};
  register vd vec1_a = {4};
  register vd vec1_b = {5};
  register vd vec1_c = {6};
  //ud* udata = NULL;
  //udata = (ud*) &results;
  gettimeofday(&start_time, NULL);
  for(register size_t i = 0; i < N; ++i ) {
    //to exploit the first AVX
    vec1_c=vec1_a+vec1_b;
    //and exploit also the second units ! if we comment out the next line, the execution time doesn't change
#if AVX_COUNT == 2
    vec2_c=vec2_a+vec2_b;
#endif
  }
  printf("Cur freq: %f Ghz\n",get_freq(get_cpu())/1E6);

  gettimeofday(&end_time, NULL);
  double elapsed= (end_time.tv_sec - start_time.tv_sec) +
    (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
  printf("Elapsed time: %f s\n",elapsed);
  //calculate the true performance 
  double flops= (AVX_COUNT*SIZE*N)/elapsed;
  printf("GFLOPs %f\n", flops/(1E9));
  printf("FLOP per cycle: %d\n",AVX_COUNT*SIZE);
  //try to estimate the current peak performance
  int cpu=get_cpu();
  float current_freq_ghz=get_freq(cpu)/1E6;
  printf("Estimated max ppef %f GFLOPs\n",current_freq_ghz*AVX_COUNT*SIZE);
  return 0;
}
