#ifndef _RSSI_CALC_H_
#define _RSSI_CALC_H_

#include <math.h>
#include <stdbool.h>

bool rssi_array_el_put(int *array, short int array_lengh, int elem); // if return 1 that array is full
int rssi_mean_calculate(int *array, short int array_lengh);

#endif