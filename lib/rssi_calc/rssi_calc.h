#ifndef _RSSI_CALC_H_
#define _RSSI_CALC_H_

#include <math.h>

void rssi_smooth(int *array, short int array_lengh, short int elem_ind);
double rssi_distance_calculate(int rssi_mom, float measure_rssi_dbm, float consider);

#endif