#include "rssi_calc.h"

void rssi_anomaly_correct(int *array, short int array_lengh, short int elem_ind){    
    short int prev_el = elem_ind - 1;
    short int next_el = elem_ind + 1;

    if(prev_el == -1){
        prev_el = array_lengh - 1;
    }
    if(next_el == array_lengh){
        next_el = 0;
    }
    if(array[elem_ind] >= (array[prev_el] + (int)((double)array[prev_el] * 0.2)) && array[elem_ind] > (array[next_el] + (int)((double)array[next_el] * 0.2))){
        array[elem_ind] = (int)((double)(array[prev_el] + array[next_el]) / 2);
    }
}

void rssi_smooth(int *array, short int array_lengh, short int elem_ind){
    short int prev_el = elem_ind - 1;
    double a = 0.75;

    if(prev_el == -1){
        prev_el = array_lengh - 1;
    }
    array[elem_ind] = (int)((double)array[elem_ind] * a + (1 - a) * (double)array[prev_el]);
}

bool rssi_array_el_put(int *array, short int array_lengh, int elem, int *first_elem_ptr){
    static short int i = 0;
    static short int i1 = -1;
    static short int i2 = -2;
    static bool arr_vol = 0;

    array = array + i;
    *array = elem;
    if(arr_vol == 1){
        *first_elem_ptr = i;
    }
    i++;
    i1++;
    i2++;
    if(arr_vol == 0){
        if(i >= 2){
            rssi_anomaly_correct(array,array_lengh,i1);
        }
        if(i >= 3){
            rssi_smooth(array,array_lengh,i2);
        }
    }else{
        rssi_anomaly_correct(array,array_lengh,i1);
        rssi_smooth(array,array_lengh,i2);
    }
    if(i == array_lengh){
        i = 0;
        arr_vol = 1;
    }
    if(i1 == array_lengh){
        i1 = 0;
    }
    if(i2 == array_lengh){
        i2 = 0;
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

double rssi_distance_calculate(int rssi_mom, float measure_rssi_dbm, float consider){
    double distance = pow(10,((double)(measure_rssi_dbm - rssi_mom)/(double)(10 * consider)));
    return distance;
}