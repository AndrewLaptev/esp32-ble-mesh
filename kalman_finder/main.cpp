#include <stdio.h>
#include <math.h>
#include "Kalman.h"

#define FILENAME1 "rssi_1m.txt"
#define FILENAME1_5 "rssi_1_5m.txt"
#define FILENAME2 "rssi_2m.txt"
#define RSSI_IDEAL1 -64.5
#define RSSI_IDEAL1_5 -68
#define RSSI_IDEAL2 -71
#define RSSI_FILTERING_INIT -71           // equal 2 meters
#define QKF 0.006967                      //
#define RKF 94.666664                     // for Kalman_filter()
#define PKF 21.333334                     //

struct coeff_finder_initial_params{
    float Qmin = 0.0003;
    float Rmin = 1;
    float Pmin = 1;
    float Qmax = 3;
    float Rmax = 100;
    float Pmax = 40;
    float Qstep = 0.01;
    float Rstep = 1;
    float Pstep = 1;
}init_params;

struct opt_filter_params{
    float Q;
    float R;
    float P;
};

///////////////////////////////////////DECLARATION////////////////////////////////////////////

int count_row_file(const char* filename);
void read_num_to_arr(const char* filename, int *rssi_arr_ext, int arr_size);
float mean_rssi(int *rssi_arr, int arr_size);
float variance_rssi(int *rssi_arr, int arr_size, float rssi_mean);
void Kalman_coeff_finder(const char *filename, float rssi_ideal, int filtering_init_rssi, coeff_finder_initial_params *init_params, opt_filter_params *optparams);
void Kalman_filter(const char *filename, int filtering_init_rssi, float Q, float R, float P);
void Kalman_mean_coeff_finder(void);

///////////////////////////////////////////MAIN///////////////////////////////////////////////

int main(){
    opt_filter_params optparams_1m;
    Kalman_coeff_finder(FILENAME1, RSSI_IDEAL1, RSSI_FILTERING_INIT, &init_params, &optparams_1m);
    //Kalman_mean_coeff_finder();
    //Kalman_filter(FILENAME1_5, RSSI_FILTERING_INIT, QKF, RKF, PKF);
    return 0;
}

/////////////////////////////////////////DEFINTION////////////////////////////////////////////

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

void Kalman_coeff_finder(const char *filename, float rssi_ideal, int filtering_init_rssi, coeff_finder_initial_params *init_params, opt_filter_params *optparams){
    int arr_size = count_row_file(filename);
    int rssi_arr[arr_size];
    read_num_to_arr(filename, rssi_arr, arr_size);

    float rssi_mean = mean_rssi(rssi_arr, arr_size);
    float rssi_variance = variance_rssi(rssi_arr, arr_size, rssi_mean);
    float rssi_stdev = sqrt(rssi_variance);

    float rssi_mean_abs_err = abs(rssi_ideal - rssi_mean);
    float rssi_mean_rel_err = abs((rssi_mean_abs_err/rssi_ideal) * 100);

    int filt_rssi_arr[arr_size];
    float filt_rssi_mean;
    float filt_rssi_variance;
    float filt_rssi_mean_abs_err;
    int opt_filt_rssi_arr[arr_size];
    float min_filt_rssi_mean = rssi_mean;
    float min_filt_rssi_variance = rssi_variance;
    float min_filt_rssi_mean_abs_err = rssi_mean_abs_err;

    float Q = init_params->Qmin;
    float R = init_params->Rmin;
    float P = init_params->Pmin;

    printf("Calculate...\n");
    for(int i = 0; i <= (init_params->Pmax/init_params->Pstep) - (init_params->Pmin/init_params->Pstep); i++){
        for(int i = 0; i <= (init_params->Rmax/init_params->Rstep) - (init_params->Rmin/init_params->Rstep); i++){
            for(int i = 0; i <= (init_params->Qmax/init_params->Qstep) - (init_params->Qmin/init_params->Qstep); i++){
                Kalman filter1(Q, R, P, filtering_init_rssi);
                for(int i = 0; i < arr_size; i++){
                    filt_rssi_arr[i] = (int)round(filter1.getFilteredValue(rssi_arr[i]));
                }
                filt_rssi_mean = mean_rssi(filt_rssi_arr, arr_size);
                filt_rssi_variance = variance_rssi(filt_rssi_arr, arr_size, filt_rssi_mean);
                filt_rssi_mean_abs_err = abs(rssi_ideal - filt_rssi_mean);
                //printf("M:%f, V:%f, E:%f\n", filt_rssi_mean, filt_rssi_variance, filt_rssi_mean_abs_err);

                if(filt_rssi_variance < min_filt_rssi_variance && filt_rssi_mean_abs_err < min_filt_rssi_mean_abs_err){
                    min_filt_rssi_mean = filt_rssi_mean;
                    min_filt_rssi_variance = filt_rssi_variance;
                    min_filt_rssi_mean_abs_err = filt_rssi_mean_abs_err;
                    optparams->Q = Q;
                    optparams->R = R;
                    optparams->P = P;
                    for(int i = 0; i < arr_size; i++){
                        opt_filt_rssi_arr[i] = filt_rssi_arr[i];
                    }
                }
                //printf("Q:%f\n",Q);
                Q += init_params->Qstep;
            }
            //printf("R:%f\n",R);
            Q = init_params->Qmin;
            R += init_params->Rstep;
        }
        //printf("P:%f\n",P);
        R = init_params->Rmin;
        P += init_params->Pstep;
    }
    /*printf("OPT_FILT_RSSI_ARR:\n");
    for(int i = 0; i < arr_size; i++){
        printf("RSSI:%d\n", opt_filt_rssi_arr[i]);
    }*/
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
    printf("Q: %f\n", optparams->Q);
    printf("R: %f\n", optparams->R);
    printf("P: %f\n", optparams->P);
}

void Kalman_filter(const char *filename, int filtering_init_rssi, float Q, float R, float P){
    int arr_size = count_row_file(filename);
    int rssi_arr[arr_size];
    read_num_to_arr(filename, rssi_arr, arr_size);
    Kalman filter(Q, R, P, filtering_init_rssi);

    for(int i = 0; i < arr_size; i++){
        printf("RSSI:%d\n", (int)round(filter.getFilteredValue(rssi_arr[i])));
    }
}

void Kalman_mean_coeff_finder(void){
    opt_filter_params optparams_1m;
    opt_filter_params optparams_1_5m;
    opt_filter_params optparams_2m;
    printf("1 meter\n");
    Kalman_coeff_finder(FILENAME1, RSSI_IDEAL1, RSSI_FILTERING_INIT, &init_params, &optparams_1m);
    printf("+++++++++++++++++++++++++++++++++++++++++++\n");
    printf("1.5 meter\n");
    Kalman_coeff_finder(FILENAME1_5, RSSI_IDEAL1_5, RSSI_FILTERING_INIT, &init_params, &optparams_1_5m);
    printf("+++++++++++++++++++++++++++++++++++++++++++\n");
    printf("2 meter\n");
    Kalman_coeff_finder(FILENAME2, RSSI_IDEAL2, RSSI_FILTERING_INIT, &init_params, &optparams_2m);
    printf("+++++++++++++++++++++++++++++++++++++++++++\n");
    float Qmean = (optparams_1m.Q + optparams_1_5m.Q + optparams_2m.Q)/3;
    float Rmean = (optparams_1m.R + optparams_1_5m.R + optparams_2m.R)/3;
    float Pmean = (optparams_1m.P + optparams_1_5m.P + optparams_2m.P)/3;
    printf("Q_MEAN: %f\n", Qmean);
    printf("R_MEAN: %f\n", Rmean);
    printf("P_MEAN: %f\n", Pmean);
}