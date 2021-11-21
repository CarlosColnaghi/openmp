#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<omp.h>

float* generate_random_numbers(float a, float b, int n, int thread_count){
    float* array = malloc(sizeof(float) * n);
    srand(time(0));
#   pragma omp parallel for num_threads(thread_count)
    for(int i = 0; i < n; i++){
        float scale = rand() / (float)RAND_MAX;
        array[i] = a + scale * (b-a);
    }
    return array;
}

int main(int argc, char* argv[]){
    int thread_count = strtol(argv[1], NULL, 10);

    generate_random_numbers(0, 10, 10, thread_count);

    return 0;
}