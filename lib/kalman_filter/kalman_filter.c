#include "kalman_filter.h"

double Kalman_filter(double process_noise, double sensor_noise, double estimated_error, double initial_value, double measurement){
    double q = process_noise;
    double r = sensor_noise;
    double static p = 0; // estimated_error
    double static x = 0; // initial_value
    double k;            // Kalman gain
    char static i = 0;

    if(i == 0){
        p = estimated_error + q;
        k = p / (p + r);
        x = initial_value + k * (measurement - initial_value);
        p = (1 - k) * p;
        i++;
    }else{
        p = p + q;
        k = p / (p + r);
        x = x + k * (measurement - x);
        p = (1 - k) * p;
    }
    return x;
}