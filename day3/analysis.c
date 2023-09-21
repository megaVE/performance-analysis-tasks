#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>

typedef struct{
    double average_customer;
    double average_service;
    double simulation_time;
} paramethers;

typedef struct{
    unsigned long int events_number;
    double previous_time;
    double sum_areas;
} little;

#define AMOUNT_OF_GENERATED_VALUES 1000000

void initLittle(little * little_init){ // Starts the Little's structure
    little_init->events_number;
    little_init->previous_time;
    little_init->sum_areas;
}

void setParamethers(paramethers * params){ // Reads the input paramethers
    printf("Enter the average time between customers (seconds): "); // ex: 0.5
    scanf("%lF", &params->average_customer);
    params->average_customer = 1.0/params->average_customer;

    printf("Enter the average time to attend each customer (seconds): "); // ex: 0.45
    scanf("%lF", &params->average_service);
    params->average_service = 1.0/params->average_service;

    printf("Enter the time period to be simulated (seconds): "); // ex: 100000
    scanf("%lF", &params->simulation_time);
}

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

    paramethers params;
    setParamethers(&params);

    /* Simulation */

    double current_time = 0.0;
    double arrival_time = (-1.0/params.average_customer) * log(uniform());
    double departure_time = DBL_MAX;
    // printf("%lF\n", departure_time);
    unsigned long int queue = 0;
    int max_queue = 0;

    double occupation_sum = 0.0;
    
    little e_n;
    initLittle(&e_n);
    
    little e_w_arrival;
    initLittle(&e_w_arrival);
    
    little e_w_departure;
    initLittle(&e_w_departure);

    while(current_time < params.simulation_time){
        current_time = min(arrival_time, departure_time);
        // printf("current time: %lF\n", current_time);
        // printf("arrival time: %lF\n", arrival_time);
        // printf("departure time: %lF\n", departure_time);
        // printf("boolean: %d\n", current_time == arrival_time);
        // getchar();

        if(current_time == arrival_time){
            if(!queue){
                double service_time = (-1.0/params.average_service) * log(uniform());
                departure_time = current_time + service_time;
                occupation_sum += service_time;
            }
            queue++;
            max_queue = queue > max_queue ? queue : max_queue;

            arrival_time = current_time + (-1.0/params.average_customer) * log(uniform());
            
            // Little's calculation (E[N])
            e_n.sum_areas += (current_time - e_n.previous_time) * e_n.events_number;
            e_n.events_number++;
            e_n.previous_time = current_time;

            // Little's calculation (E[W]: arrival)
            e_w_arrival.sum_areas += (current_time - e_w_arrival.previous_time) * e_w_arrival.events_number;
            e_w_arrival.events_number++;
            e_w_arrival.previous_time = current_time;
        } else if(current_time == departure_time){
            queue--;
            if(queue){
                double service_time = (-1.0/params.average_service) * log(uniform());
                departure_time = current_time + service_time;
                occupation_sum += service_time;
            }else
                departure_time = DBL_MAX;
            
            // Little's calculation (E[N])
            e_n.sum_areas += (current_time - e_n.previous_time) * e_n.events_number;
            e_n.events_number--;
            e_n.previous_time += current_time;

            // Little's calculation (E[W]: departure)
            e_w_departure.sum_areas += (current_time - e_w_departure.previous_time) * e_w_departure.events_number;
            e_w_departure.events_number++;
            e_w_departure.previous_time = current_time;
        }else{
            printf("Invalid event!\n");
            return(1);
        }
    }
    e_w_arrival.sum_areas += (current_time - e_w_arrival.previous_time) * e_w_arrival.events_number;
    e_w_departure.sum_areas += (current_time - e_w_departure.previous_time) * e_w_departure.events_number;

    printf("occupation: %lF\n", occupation_sum / current_time);
    printf("max queue: %d\n", max_queue);
    
    double e_n_calc = e_n.sum_areas / current_time;
    printf("E[N] %lF\n", e_n_calc);

    double e_w_calc = (e_w_arrival.sum_areas - e_w_departure.sum_areas) / e_w_arrival.events_number;
    printf("E[W] %lF\n", e_w_calc);

    double lambda = e_w_arrival.events_number / current_time;
    double little_error = e_n_calc - lambda * e_w_calc; 
    printf("Little's error: %lF\n", little_error);

    return 0;
}