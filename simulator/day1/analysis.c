#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>

#define AMOUNT_OF_GENERATED_VALUES 1000000

double uniform(){ // limitando entre (0,1]
    double u = rand() / ((double) RAND_MAX + 1);
    u = 1.0 - u;

    return(u);
}

double min(double a, double b){
    if(a > b)
        return b;
    return a;
}

int main(){
    int seed = time(NULL);
    srand(seed); // RNG Seed

    /* Input */

    double average_customer;
    printf("Enter the average time between customers (seconds): ");
    scanf("%lF", &average_customer);
    average_customer = 1.0/average_customer;

    double average_service;
    printf("Enter the average time to attend each customer (seconds): ");
    scanf("%lF", &average_service);
    average_service = 1.0/average_service;

    double simulation_time;
    printf("Enter the time period to be simulated (seconds): ");
    scanf("%lF", &simulation_time);

    /* Simulation */

    double current_time = 0.0;

    double arrival_time = (-1.0/average_customer) * log(uniform());
    double departure_time = DBL_MAX;
    // printf("%lF\n", departure_time);

    unsigned long int queue = 0;

    while(current_time < simulation_time){
        current_time = min(arrival_time, departure_time);
        // printf("arrival time: %lF\n", arrival_time);
        // printf("departure time: %lF\n", departure_time);
        // printf("current time: %lF\n", current_time);
        // printf("boolean: %d\n", current_time == arrival_time);
        // getchar();

        if(current_time == arrival_time){
            if(queue)
                queue++;
            else
                departure_time = current_time + (-1.0/average_service) * log(uniform());
            
            arrival_time = current_time + (-1.0/average_customer) * log(uniform());
        
        } else if(current_time == departure_time){
            if(queue){
                queue--;
                departure_time = current_time + (-1.0/average_service) * log(uniform());
            }else
                departure_time = DBL_MAX;
        }else{
            printf("Invalid time!\n");
            return(1);
        }
    }

    return 0;
}