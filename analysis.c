#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define AMOUNT_OF_GENERATED_VALUES 1000000

double uniform(){ // limitando entre (0,1]
    double u = rand() / ((double) RAND_MAX + 1);
    u = 1.0 - u;

    return(u);
}

int main(){
    double l;
    double sum = 0.0;
    int seed = time(NULL);
    
    srand(seed); // RNG Seed

    printf("Enter the average time (seconds): ");     // Na Exponencial, E[X] = 1/l
    scanf("%lF", &l);                                   // E[X] = 5
                                                        // 1/l = 5
    l = 1.0 / l;                                        // l = 1/5 = 0.2
    
    for(int i=0; i < AMOUNT_OF_GENERATED_VALUES; i++){
        double value = (-1.0/l) * log(uniform());
        // printf("%lF\n", value);
        // getchar();
        sum += value;
    }
    printf("Mean: %lF\n", (sum/AMOUNT_OF_GENERATED_VALUES));

    return 0;
}