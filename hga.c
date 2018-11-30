//
// Created by Dell on 2018/8/17.
//

#include <malloc.h>
#include <stdio.h>
#include <io.h>
#include "hga.h"
#include "commen.h"
#include "tools.h"
#include "heuristic_func.h"

extern int stage_interval;
extern int swarm_size;
extern int flowchart_size;
extern int available;
extern int print_travers;
extern int print_simulation;

void hga_process(struct flowchart_group * flowchartGroup_ori, struct machine_group * machineGroup_ori, int time){
    struct flowchart_group * flowchartGroup = malloc(sizeof(struct flowchart_group));
    struct machine_group * machineGroup = malloc(sizeof(struct machine_group));

    *flowchartGroup = *flowchartGroup_ori;
    *machineGroup = *machineGroup_ori;

    struct activity_set * remain = malloc(sizeof(struct activity_set));
    remain->length = 0;

    struct individual * best = malloc(sizeof(struct individual));

    int max = -1;
    for (int i = 0; i < flowchartGroup->size; ++i) {
        int t = flowchartGroup->flowcharts[i].arrival_time;
        if(t > max){
            max = t;
        }
    }

    for (int i = 0; i < 100; ++i) {
        int start = i * stage_interval;
        int end = (i + 1) * stage_interval;

        struct flowchart_group * stage_arrivals = malloc(sizeof(struct flowchart_group));
        find_stage_arrivals(start, end, flowchartGroup, stage_arrivals);

        if (stage_arrivals->size == 0){
            continue;
        }

        hga_find_best(start, end, remain, stage_arrivals, flowchartGroup, machineGroup, best, time);

        if (max >= start && max < end){
            break;
        }

        hga_simulation(start, end, remain, stage_arrivals, flowchartGroup, machineGroup, best);

        find_remain(end, flowchartGroup, remain);

        if (stage_arrivals->size == 0 && remain->length == 0){
            break;
        }
    }

    free(flowchartGroup);
    free(machineGroup);
}


void hga_find_best(int start, int end, struct activity_set * remain, struct flowchart_group * arrivals, struct flowchart_group * flowchartGroup, struct machine_group * machineGroup, struct individual * best, int timed){
    struct swarm_group * swarmGroup = malloc(sizeof(struct swarm_group));
    hga_initialize_swarm_group(remain, arrivals, swarmGroup);

    for (int g = 1; g <= GEN; ++g) {
        struct flowchart_group * flowchart_copy = malloc(sizeof(struct flowchart_group));
        struct machine_group * machine_copy = malloc(sizeof(struct machine_group));
        struct flowchart_group * arrivals_copy = malloc(sizeof(struct flowchart_group));
        for (int i = 0; i < swarm_size; ++i) {
            *flowchart_copy = *flowchartGroup;
            *machine_copy = *machineGroup;
            *arrivals_copy = *arrivals;

            struct individual indiv = swarmGroup->individuals[i];

            hga_calculate_individual_efficency(start, end, remain, arrivals_copy, flowchart_copy, machine_copy, indiv);
            swarmGroup->individuals[i].total_makespan = calculate_total_makespan(flowchart_copy);
        }

        int min = INF;
        for (int i = 0; i < swarm_size; ++i) {
            if (swarmGroup->individuals[i].total_makespan < min){
                min = swarmGroup->individuals[i].total_makespan;
            }
        }

        printf("%d\n", min);

        FILE * file;

        char * filename = malloc(50* sizeof(int));
        sprintf(filename, "hga_f_%d_i_%d_g_%d_t_%d.txt", flowchart_size, STAGE_INTERVAL, swarm_size, timed);
        if (access(filename, 0) == 0){
            file = fopen(filename, "a");
        } else{
            file = fopen(filename, "w");
        }

        fprintf(file, "%d\n", min);
        fclose(file);

        if (g == GEN){
            best->total_makespan = INF;
            for (int i = 0; i < swarm_size; ++i) {
                if (swarmGroup->individuals[i].total_makespan < best->total_makespan){
                    *best = swarmGroup->individuals[i];
                }
            }
            break;
        }
        hga_swarm_genetic_operations(swarmGroup);
        free(flowchart_copy);
        free(machine_copy);
        free(arrivals_copy);
    }
    free(swarmGroup);
}

void hga_initialize_swarm_group(struct activity_set * remain, struct flowchart_group * arrivals, struct swarm_group * swarmGroup){
    int total = remain->length;
    for (int i = 0; i < arrivals->size; ++i) {
        for (int j = 1; j < arrivals->flowcharts[i].vertex - 1; ++j) {
            total++;
        }
    }

    swarmGroup->activity_size = total;
    swarmGroup->flowchart_size = arrivals->size;
    for (int i = 0; i < swarm_size; ++i) {

        swarmGroup->individuals[i].flowchart_size = arrivals->size;
        swarmGroup->individuals[i].activity_size = total;

        for (int j = 0; j < total; ++j) {
            swarmGroup->individuals[i].heuristic[j] = random_number(1, HEURISTIC_NUMBER);
        }

        for (int j = 0; j < arrivals->size; ++j) {
            swarmGroup->individuals[i].modes[j][0] = arrivals->flowcharts[j].vertex - 2;
            for (int k = 1; k < arrivals->flowcharts[j].vertex - 1; ++k) {
                int mode = arrivals->flowcharts[j].activities[k].mode_quantity;
                swarmGroup->individuals[i].modes[j][k] = random_number(0, mode - 1);
            }
        }
    }
}

void hga_calculate_individual_efficency(int start, int end, struct activity_set * remain, struct flowchart_group * arrivals, struct flowchart_group * flowchartGroup, struct machine_group * machineGroup, struct individual indiv){
    CMPSolve(arrivals, indiv);

    int time = start;
    int count = 0;
    int finished = 0;
    int total = indiv.activity_size;
    do{
        mark_new_arrive(arrivals, time);

        struct activity_set * set = malloc(sizeof(struct activity_set));
        while (1){
            int machine_index = find_available_machine(machineGroup, time);
            if (machine_index == -1) break;
            find_schedualble_aty(remain, arrivals, time, flowchartGroup, set);
            if (set->length == 0) break;

            struct activity chosen = hga_find_prior(set, indiv, count);
            count++;
            int flow_index = chosen.flow_index;
            int aty_index = chosen.index;

            //将活动部署到机器上去
            int execution_time = chosen.durations/machineGroup->machines[machine_index].speed;

            machineGroup->machines[machine_index].finish = time + execution_time;
            machineGroup->machines[machine_index].flow_index = flow_index;
            machineGroup->machines[machine_index].activity_index = aty_index;

            flowchartGroup->flowcharts[flow_index].activities[aty_index].state = 2;

            if (print_travers) printf("(%d, %d) on %d \n", flow_index, aty_index, machine_index);
        }
        free(set);

        //时间跳转
        int min = INF;
        for (int i = 0; i < MACHINE_NUMBER; ++i) {
            int finish_time = machineGroup->machines[i].finish;
            if (finish_time > time && finish_time < min){
                min = finish_time;
            }
        }

        if (min == INF){
            for (int i = 0; i < arrivals->size; ++i) {
                int arrive_time = arrivals->flowcharts[i].arrival_time;
                if (arrive_time > time && arrive_time < min){
                    min = arrive_time;
                }
            }
        }

        time = min;

        if (print_travers) printf("\ntime --- %d\n", time);

        //释放当前时间结束的机器
        for (int i = 0; i < MACHINE_NUMBER; ++i) {

            int flow = machineGroup->machines[i].flow_index;
            int aty = machineGroup->machines[i].activity_index;
            if (flow < 0 || flow > flowchart_size){
                continue;
            }

            int state = flowchartGroup->flowcharts[flow].activities[aty].state;
            if (state != 2){
                continue;
            }

            if (machineGroup->machines[i].finish <= time){

                flowchartGroup->flowcharts[flow].activities[aty].state = 1;
                finished++;

                if (print_travers) printf("(%d, %d) finished\n", flow, aty);

                int result = check_flowchart(flowchartGroup->flowcharts[flow]);
                if (result == 1){
                    flowchartGroup->flowcharts[flow].received = 1;
                    flowchartGroup->flowcharts[flow].finish_time = time;
                    if (print_travers == 1)
                        printf("workflow %d complete\n", flow);
                }
            }
        }

    }while (finished < total);
    return;
}


void hga_swarm_genetic_operations(struct swarm_group * swarmGroup){
    int count = 0;
    int new = swarm_size * (1 - OFFSPRING_KEEP_RATE);
    struct swarm_group * offsprings = malloc(sizeof(struct swarm_group));

    while (count < new){
        int a, b;
        do{
            a = random_number(0, swarm_size - 1);
            b = random_number(0, swarm_size - 1);
        }while (a == b);

        struct individual parent1 = swarmGroup->individuals[a];
        struct individual parent2 = swarmGroup->individuals[b];

        //modes
        for (int i = 0; i < swarmGroup->flowchart_size; ++i) {
            offsprings->individuals[count].modes[i][0] = parent1.modes[i][0];
            offsprings->individuals[count + 1].modes[i][0] = parent1.modes[i][0];
            int low, upper;
            do{
                low = random_number(1, parent1.modes[i][0]);
                upper = random_number(1, parent1.modes[i][0]);
            }while (low == upper);
            if (low > upper){
                int t = low;
                low = upper;
                upper = t;
            }
            for (int j = 1; j <= parent1.modes[i][0]; ++j) {
                if (j >= low && j <= upper){
                    offsprings->individuals[count].modes[i][j] = parent2.modes[i][j];
                    offsprings->individuals[count + 1].modes[i][j] = parent1.modes[i][j];
                } else{
                    offsprings->individuals[count].modes[i][j] = parent1.modes[i][j];
                    offsprings->individuals[count + 1].modes[i][j] = parent2.modes[i][j];
                }
            }
        }

        int low, upper;
        do{
            low = random_number(0, swarmGroup->activity_size - 1);
            upper = random_number(0, swarmGroup->activity_size - 1);
        }while (low == upper);
        if (low > upper){
            int temp = low;
            low = upper;
            upper = temp;
        }

        for (int i = 0; i < swarmGroup->activity_size; ++i) {
            if (i >= low && i <= upper){
                offsprings->individuals[count].heuristic[i] = parent2.heuristic[i];
                offsprings->individuals[count + 1].heuristic[i] = parent1.heuristic[i];
            } else{
                offsprings->individuals[count].heuristic[i] = parent1.heuristic[i];
                offsprings->individuals[count + 1].heuristic[i] = parent2.heuristic[i];
            }
        }
        count += 2;
    }

    for (int i = 0; i < new; ++i) {
        //找出当前swarm中total_makespan最大的
        int max = -1;
        int index = -1;
        for (int j = 0; j < swarm_size; ++j) {
            if (swarmGroup->individuals[j].total_makespan != -1 && swarmGroup->individuals[j].total_makespan > max){
                max = swarmGroup->individuals[j].total_makespan;
                index = j;
            }
        }

        //使用当前的offspring替换掉最大的那个
        offsprings->individuals[i].flowchart_size = swarmGroup->individuals[index].flowchart_size;
        offsprings->individuals[i].activity_size = swarmGroup->individuals[index].activity_size;

        swarmGroup->individuals[index] = offsprings->individuals[i];

        swarmGroup->individuals[index].makespan = -1;
        swarmGroup->individuals[index].total_makespan = -1;
        swarmGroup->individuals[index].total_delay = -1;

    }
    free(offsprings);
}

struct activity hga_find_prior(struct activity_set * set, struct individual indiv, int count){
    int heuristic = indiv.heuristic[count];
    switch (heuristic){
        case 1:
            return min_eft(set);
        case 2:
            return min_est(set);
        case 3:
            return min_lst(set);
        case 4:
            return min_lft(set);
        case 5:
            return min_spt(set);
        case 6:
            return min_sk(set);
    }
    return min_sk(set);
}

void hga_simulation(int start, int end, struct activity_set * remain, struct flowchart_group * arrivals, struct flowchart_group * flowchartGroup, struct machine_group * machineGroup, struct individual * best){
    CMPSolve(arrivals, *best);
    copy_to_flowchart_group(arrivals, flowchartGroup);

    int time = start;
    int count = 0;
    int finished = 0;
    int total = best->activity_size;
    remain->length = 0;

    do{
        mark_new_arrive(flowchartGroup, time);

        struct activity_set * set = malloc(sizeof(struct activity_set));
        while (1){
            int machine_index = find_available_machine(machineGroup, time);
            if (machine_index == -1) break;
            find_schedualble_aty(remain, flowchartGroup, time, flowchartGroup, set);
            if (set->length == 0) break;

            struct activity chosen = hga_find_prior(set, *best, count);
            count++;
            int flow_index = chosen.flow_index;
            int aty_index = chosen.index;

            //将活动部署到机器上去
            int execution_time = chosen.durations/machineGroup->machines[machine_index].speed;

            machineGroup->machines[machine_index].finish = time + execution_time;
            machineGroup->machines[machine_index].flow_index = flow_index;
            machineGroup->machines[machine_index].activity_index = aty_index;

            flowchartGroup->flowcharts[flow_index].activities[aty_index].state = 2;

        }
        free(set);

        //时间跳转
        int min = INF;
        for (int i = 0; i < MACHINE_NUMBER; ++i) {
            int finish_time = machineGroup->machines[i].finish;
            if (finish_time > time && finish_time < min){
                min = finish_time;
            }
        }

        if (min == INF){
            for (int i = 0; i < arrivals->size; ++i) {
                int arrive_time = arrivals->flowcharts[i].arrival_time;
                if (arrive_time > time && arrive_time < min){
                    min = arrive_time;
                }
            }
        }

        time = min;

        if (time > end){
            return;
        }

        //释放当前时间结束的机器
        for (int i = 0; i < MACHINE_NUMBER; ++i) {
            if (machineGroup->machines[i].finish == time){
                int flow = machineGroup->machines[i].flow_index;
                int aty = machineGroup->machines[i].activity_index;
                flowchartGroup->flowcharts[flow].activities[aty].state = 1;
                finished++;

                int result = check_flowchart(flowchartGroup->flowcharts[flow]);
                if (result == 1){
                    flowchartGroup->flowcharts[flow].received = 1;
                    flowchartGroup->flowcharts[flow].finish_time = time;
                }
            }
        }

    }while (finished < total);
    return;
}