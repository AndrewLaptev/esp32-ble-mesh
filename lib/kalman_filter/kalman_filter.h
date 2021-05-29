#ifndef _KALMAN_FILTER_H_
#define _KALMAN_FILTER_H_

#include <math.h>

double Kalman_filter(double process_noise, double sensor_noise, double estimated_error, double initial_value, double measurement);

#endif