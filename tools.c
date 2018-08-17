//
// Created by Dell on 2018/7/12.
//

#include <stdlib.h>
#include <mem.h>
#include "tools.h"

#include "stdio.h"

extern int datas;
extern int flowchart_size;
extern int swarm_size;
extern int stage_interval;

int random_number(int start, int end){
    return start + rand()%(end - start + 1);
}

void handle_result(){
    int hga[1000], pga[1000];
    memset(hga, 0, sizeof(int) * 1000);
    memset(pga, 0, sizeof(int) * 1000);

    for (int i = 0; i < datas; ++i) {
        char * hga_file_name = malloc(50);
        char * pga_file_name = malloc(50);

        sprintf(hga_file_name, "hga_f_%d_i_%d_g_%d_t_%d.txt", flowchart_size, stage_interval, swarm_size, i);
        sprintf(pga_file_name, "pga_f_%d_i_%d_g_%d_t_%d.txt", flowchart_size, stage_interval, swarm_size, i);

        FILE * hga_file = fopen(hga_file_name, "r");
        FILE * pga_file = fopen(pga_file_name, "r");

        for (int j = 0; j < 150; ++j) {
            int hga_value, pga_value;

            fscanf(hga_file, "%d\n", &hga_value);
            fscanf(pga_file, "%d\n", &pga_value);

            hga[j] += hga_value;
            pga[j] += pga_value;
        }

        fclose(hga_file);
        fclose(pga_file);

        remove(hga_file_name);
        remove(pga_file_name);
    }

    char * hga_filename = malloc(50);
    char * pga_filename = malloc(50);
    sprintf(hga_filename, "hga_f_%d_i_%d_g_%d.txt", flowchart_size, stage_interval, swarm_size);
    sprintf(pga_filename, "pga_f_%d_i_%d_g_%d.txt", flowchart_size, stage_interval, swarm_size);

    FILE * hga_file = fopen(hga_filename, "w");
    FILE * pga_file = fopen(pga_filename, "w");

    for (int i = 0; i < 150; ++i) {
        fprintf(hga_file, "%d\n", hga[i]);
        fprintf(pga_file, "%d\n", pga[i]);
    }

    fclose(hga_file);
    fclose(pga_file);
}