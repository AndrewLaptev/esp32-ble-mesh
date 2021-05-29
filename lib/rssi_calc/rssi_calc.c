#include "rssi_calc.h"
#include "esp_log.h"

void rssi_smooth(int *array, short int array_lengh, short int elem_ind){
    short int prev_el = elem_ind - 1;
    double a = 0.75;

    if(prev_el == -1){
        prev_el = array_lengh - 1;
    }
    array[elem_ind] = (int)((double)array[elem_ind] * a + (1 - a) * (double)array[prev_el]);
}

double rssi_distance_calculate(int rssi_mom, float measure_rssi_dbm, float consider){
    double distance = pow(10,((double)(measure_rssi_dbm - rssi_mom)/(double)(10 * consider)));
    return distance;
}