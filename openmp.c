#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<omp.h>

float* generate_histogram_data(float a, float b, int n, int thread_count){
    float* array = malloc(sizeof(float) * n);
    srand(time(0));
#   pragma omp parallel for num_threads(thread_count)
    for(int i = 0; i < n; i++){
        float scale = rand() / (float)RAND_MAX;
        array[i] = a + scale * (b-a);
    }
    return array;
}

float* generate_histogram_subdivision(float a, float b, int n, int c, int thread_count){
    int difference_between_intervals = b-a;
    float difference_each_subdivision = (float)difference_between_intervals/(float)c;
   
    float* limits_each_subdvision = malloc(sizeof(float)*c+1);
    limits_each_subdvision[0] = a;
    limits_each_subdvision[c] = b;

//#   pragma omp parallel for num_threads(thread_count)
    for(int i = 1; i < c; i++){
        limits_each_subdvision[i] = limits_each_subdvision[i-1] + difference_each_subdivision;
    }
    return limits_each_subdvision;
}

int* generate_histogram_frequency(float* data, int* histogram, float a, float b, int n, int c){
    int my_rank = omp_get_thread_num();
    int thread_count = omp_get_num_threads();

    float* limits_each_subdvision = generate_histogram_subdivision(a, b, n, c, thread_count);

    int amount_data_each_subdivision = n/thread_count;
    if((n%thread_count != 0) && (my_rank == 0)){
        amount_data_each_subdivision += n%thread_count;
    }

    for(int i = amount_data_each_subdivision * my_rank; i < (amount_data_each_subdivision * my_rank) + amount_data_each_subdivision; i++){
        for(int j = 0; j < c; j++){
            if(data[i] >= limits_each_subdvision[j] && data[i] < limits_each_subdvision[j+1]){
#               pragma omp critical
                histogram[j] += 1;
            }
        }
    }
    return histogram;
}


int main(int argc, char* argv[]){
    int thread_count = strtol(argv[1], NULL, 10); 

    // float a = 0, b = 5;
    // int c = 5, n = 20;

    float a, b;
    int c, n;

    printf("a = ");
    scanf("%f", &a);
    printf("b = ");
    scanf("%f", &b);
    printf("c = ");
    scanf("%d", &c);
    printf("n = ");
    scanf("%d", &n);
    printf("\n");
    
    float* data = generate_histogram_data(a, b, n, thread_count);

    printf("::DADOS::\n\n");
    for(int i = 0; i < n; i++){
        printf("%.2f ", data[i]);
    }
    printf("\n\n");

    int* histogram = malloc(sizeof(int)*c);

#   pragma omp parallel for num_threads(thread_count)
    for (int i = 0; i < c; i++){
        histogram[i] = 0;
    }

#   pragma omp parallel num_threads(thread_count)
    histogram = generate_histogram_frequency(data, histogram, a, b, n, c);

    float* histogram_subdivision = generate_histogram_subdivision(a, b, n, c, thread_count);

    printf("::HISTOGRAMA::\n\n");
    for(int i = 0; i < c; i++){
        printf("[%.2f .. %.2f] %d\n", histogram_subdivision[i], histogram_subdivision[i+1], histogram[i]);
    }

    return 0;
}
