#include "rssi_calc.h"

bool rssi_array_el_put(int *array, short int array_lengh, int elem){
    static short int i = 0;
    static bool arr_vol = 0;
    array = array + i;
    *array = elem;
    i++;
    if(i == array_lengh){
        i = 0;
        arr_vol = 1;
        return arr_vol;
    }
    return arr_vol;
}

int rssi_mean_calculate(int *array, short int array_lengh){
    int rssi_mean = 0;
    float rssi_tmp;
    for(int short i = 0; i < array_lengh; i++){
        rssi_mean += *array;
        array++;
    }
    rssi_mean = (int)roundf(rssi_tmp = (float)rssi_mean / (float)array_lengh);
    return rssi_mean;
}