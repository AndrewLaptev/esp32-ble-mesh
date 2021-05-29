#ifndef _ADAPT_BANDPASS_FILTER_H_
#define _ADAPT_BANDPASS_FILTER_H_

#include <math.h>

void init_buffer(int *raw_rssi_arr, int *buffer, int buffer_size);
int adapt_bandpass_filter(int rssi, int *buffer, int buffer_size, float a, float b);

#endif