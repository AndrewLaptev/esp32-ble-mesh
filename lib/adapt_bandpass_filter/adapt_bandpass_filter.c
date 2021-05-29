#include "adapt_bandpass_filter.h"

int arr_min(int *array, int arr_size);
int arr_max(int *array, int arr_size);
float low_border(int *array, int arr_size, float a);
float high_border(int *array, int arr_size, float b);
void buffer_rssi_put(int *buffer, int buffer_size, int element);
void filter(int rssi, int *buffer, int buffer_size, float a, float b);
void init_buffer(int *raw_rssi_arr, int *buffer, int buffer_size);


int arr_min(int *array, int arr_size){
    int result = array[0];
    for(int i = 1; i < arr_size; i++){
        if(array[i] < result){
            result = array[i];
        }
    }
    return result;
}

int arr_max(int *array, int arr_size){
    int result = array[0];
    for(int i = 1; i < arr_size; i++){
        if(array[i] > result){
            result = array[i];
        }
    }
    return result;
}

float low_border(int *array, int arr_size, float a){
    return (float)arr_min(array, arr_size) / (1 + a);
}

float high_border(int *array, int arr_size, float b){
    return (float)arr_max(array, arr_size) * (1 + b);
}

void buffer_rssi_put(int *buffer, int buffer_size, int element){
    static int ind = 0;
    buffer[ind] = element;
    ind++;
    if(ind == buffer_size){
        ind = 0;
    }
}

void filter(int rssi, int *buffer, int buffer_size, float a, float b){
    float lb = low_border(buffer, buffer_size, a);
    float hb = high_border(buffer, buffer_size, b);
    if(rssi >= lb){
        buffer_rssi_put(buffer, buffer_size, (int)round(lb));
    }else if(rssi <= hb){
        buffer_rssi_put(buffer, buffer_size, (int)round(hb));
    }else{
        buffer_rssi_put(buffer, buffer_size, rssi);
    }
}

void init_buffer(int *raw_rssi_arr, int *buffer, int buffer_size){
    int rssi_mean = 0;
    for(int i = 0; i < buffer_size; i++){
        buffer_rssi_put(buffer, buffer_size, raw_rssi_arr[i]);
    }
    for(int i = 0; i < buffer_size; i++){
        for(int i = 0; i < buffer_size; i++){
            rssi_mean += buffer[i];
        }
        rssi_mean = (int)round((float)rssi_mean / (float)buffer_size);
        buffer_rssi_put(buffer, buffer_size, rssi_mean);
        rssi_mean = 0;
    }
}

int adapt_bandpass_filter(int rssi, int *buffer, int buffer_size, float a, float b){
    int rssi_mean = 0;
    filter(rssi, buffer, buffer_size, a, b);
    for(int i = 0; i < buffer_size; i++){
        rssi_mean += buffer[i];
    }
    return (int)round((float)rssi_mean) / buffer_size;
}