#include <stdio.h>
#include <math.h>
#include "Kalman.h"

int count_row_file(const char* filename){
    FILE *ptr_file;
    int count_row = 0;
    int min_rssi;
    ptr_file = fopen(filename, "r");

    if(!ptr_file){
        printf("Can't open file for reading.\n");
    }else{
        while(1){
            if(fscanf(ptr_file, "%d", &min_rssi) == EOF){
                break;
            }
            count_row++;
        }
    }
    return count_row;
}

void read_num_to_arr(const char* filename, int *rssi_arr_ext, int arr_size){
    FILE *ptr_file;
    ptr_file = fopen(filename, "r");

    if(!ptr_file){
        printf("Can't open file for reading.\n");
    }else{
        for(int i = 0; i < arr_size; i++){
            fscanf(ptr_file, "%d", &rssi_arr_ext[i]);
        }
        fclose(ptr_file);
    }
}

float mean_rssi(int *rssi_arr, int arr_size){
    float rssi_mean = 0;
    for(int i = 0; i < arr_size; i++){
        rssi_mean += rssi_arr[i];
    }
    return rssi_mean/arr_size;
}

float variance_rssi(int *rssi_arr, int arr_size, float rssi_mean){
    float rssi_var = 0;
    for(int i = 0; i < arr_size; i++){
        rssi_var += pow((rssi_arr[i] - rssi_mean),2);
    }
    return rssi_var/(arr_size - 1);
}

#define FILENAME "rssi_1_5m.txt"
#define RSSI_IDEAL -68

int main(){
    int arr_size = count_row_file(FILENAME);
    int rssi_arr[arr_size];
    read_num_to_arr(FILENAME, rssi_arr, arr_size);

    float rssi_mean = mean_rssi(rssi_arr, arr_size);
    float rssi_variance = variance_rssi(rssi_arr, arr_size, rssi_mean);
    float rssi_stdev = sqrt(rssi_variance);
    float rssi_ideal = RSSI_IDEAL;

    float rssi_mean_abs_err = abs(rssi_ideal - rssi_mean);
    float rssi_mean_rel_err = abs((rssi_mean_abs_err/rssi_ideal) * 100);

    int filt_rssi_arr[arr_size];
    float filt_rssi_mean;
    float rssi_mean_dist; // distance between rssi_ideal and filt_rssi_mean (low -> best)
    float filt_rssi_variance;
    float filt_rssi_mean_abs_err;
    int opt_filt_rssi_arr[arr_size];
    float min_filt_rssi_mean = rssi_mean;
    float min_filt_rssi_variance = rssi_variance;
    float min_filt_rssi_mean_abs_err = rssi_mean_abs_err;
    float min_rssi_mean_dist = abs(rssi_ideal/10);

    float Qmin = 0.003;
    float Rmin = 1;
    float Pmin = 1;
    float Qmax = 3;
    float Rmax = 40;
    float Pmax = 4;
    float Qstep = 0.01;
    float Rstep = 1;
    float Pstep = 1;
    float Q = Qmin;
    float R = Rmin;
    float P = Pmin;

    struct filter_params{
        float Q;
        float R;
        float P;
    }optparams;

    printf("Calculate...\n");
    for(int i = 0; i <= (Pmax/Pstep) - (Pmin/Pstep); i++){
        for(int i = 0; i <= (Rmax/Rstep) - (Rmin/Rstep); i++){
            for(int i = 0; i <= (Qmax/Qstep) - (Qmin/Qstep); i++){
                Kalman filter1(Q,R,P,-71); // -71 because controller start detect beacon with 2 meters
                for(int i = 0; i < arr_size; i++){
                    filt_rssi_arr[i] = filter1.getFilteredValue(rssi_arr[i]);
                }
                filt_rssi_mean = mean_rssi(filt_rssi_arr, arr_size);
                rssi_mean_dist = abs(filt_rssi_mean - rssi_ideal);
                filt_rssi_variance = variance_rssi(filt_rssi_arr, arr_size, filt_rssi_mean);
                filt_rssi_mean_abs_err = abs(rssi_ideal - filt_rssi_mean);
                //printf("M:%f, V:%f, E:%f\n", filt_rssi_mean, filt_rssi_variance, filt_rssi_mean_abs_err);

                if(rssi_mean_dist < min_rssi_mean_dist && filt_rssi_variance < min_filt_rssi_variance && filt_rssi_mean_abs_err < min_filt_rssi_mean_abs_err){
                    min_filt_rssi_mean = filt_rssi_mean;
                    min_rssi_mean_dist = rssi_mean_dist;
                    min_filt_rssi_variance = filt_rssi_variance;
                    min_filt_rssi_mean_abs_err = filt_rssi_mean_abs_err;
                    optparams.Q = Q;
                    optparams.R = R;
                    optparams.P = P;
                    for(int i = 0; i < arr_size; i++){
                        opt_filt_rssi_arr[i] = filt_rssi_arr[i];
                    }
                }
                //printf("Q:%f\n",Q);
                Q += Qstep;
            }
            //printf("R:%f\n",R);
            Q = Qmin;
            R += Rstep;
        }
        //printf("P:%f\n",P);
        R = Rmin;
        P += Pstep;
    }
    printf("OPT_FILT_RSSI_ARR:\n");
    for(int i = 0; i < arr_size; i++){
        printf("RSSI:%d\n", opt_filt_rssi_arr[i]);
    }
    printf("IDEAL:%f\n", rssi_ideal);
    printf("MEAN:%f\n", rssi_mean);
    printf("VAR:%f\n", rssi_variance);
    printf("STDEV:%f\n", rssi_stdev);
    printf("MEAN_ABS_ERR:%f\n", rssi_mean_abs_err);
    printf("MEAN_REL_ERR:%f\n", rssi_mean_rel_err);
    printf("------------------------------------\n");
    printf("IDEAL:%f\n", rssi_ideal);
    printf("OPT_MEAN: %f\n", min_filt_rssi_mean);
    printf("OPT_VARIANCE: %f\n", min_filt_rssi_variance);
    printf("OPT_STDEV: %f\n", sqrt(min_filt_rssi_variance));
    printf("OPT_MEAN_ABS_ERR: %f\n", min_filt_rssi_mean_abs_err);
    printf("OPT_MEAN_REL_ERR: %f\n", abs((min_filt_rssi_mean_abs_err/rssi_ideal) * 100));
    printf("Q: %f\n", optparams.Q);
    printf("R: %f\n", optparams.R);
    printf("P: %f\n", optparams.P);
    return 0;
}