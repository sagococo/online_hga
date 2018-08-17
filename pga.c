//
// Created by Dell on 2018/8/17.
//

#include <malloc.h>
#include <stdio.h>
#include <io.h>
#include <mem.h>
#include "pga.h"
#include "commen.h"
#include "tools.h"

extern int stage_interval;
extern int swarm_size;
extern int flowchart_size;
extern int available;
extern int print_travers;
extern int print_simulation;

void pga_process(struct flowchart_group * flowchartGroup_ori, struct machine_group * machineGroup_ori, int time){

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
        int start = i * STAGE_INTERVAL;
        int end = (i + 1) * STAGE_INTERVAL;

        struct flowchart_group * stage_arrivals = malloc(sizeof(struct flowchart_group));
        find_stage_arrivals(start, end, flowchartGroup, stage_arrivals);

        if (stage_arrivals->size == 0){
            continue;
        }

        pga_find_best(start, end, remain, stage_arrivals, flowchartGroup, machineGroup, best, time);

        if (max >= start && max < end){
            break;
        }

        pga_simulation(start, end, remain, stage_arrivals, flowchartGroup, machineGroup, best);

        find_remain(end, flowchartGroup, remain);

        if (stage_arrivals->size == 0 && remain->length == 0){
            break;
        }
    }
}

void pga_find_best(int start, int end, struct activity_set * remain, struct flowchart_group * arrivals, struct flowchart_group * flowchartGroup, struct machine_group * machineGroup, struct individual * best, int timed){
    struct swarm_group * swarmGroup = malloc(sizeof(struct swarm_group));
    pga_initialize_swarm_group(remain, arrivals, swarmGroup);

    for (int g = 1; g <= GEN; ++g) {
        struct flowchart_group * flowchart_copy = malloc(sizeof(struct flowchart_group));
        struct machine_group * machine_copy = malloc(sizeof(struct machine_group));
        struct flowchart_group * arrivals_copy = malloc(sizeof(struct flowchart_group));
        for (int i = 0; i < swarm_size; ++i) {
            *flowchart_copy = *flowchartGroup;
            *machine_copy = *machineGroup;
            *arrivals_copy = *arrivals;

            struct individual indiv = swarmGroup->individuals[i];

            pga_calculate_individual_efficency(start, end, remain, arrivals_copy, flowchart_copy, machine_copy, indiv);
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
        sprintf(filename, "pga_f_%d_i_%d_g_%d_t_%d.txt", flowchart_size, STAGE_INTERVAL, swarm_size, timed);
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
        pga_swarm_genetic_operations(swarmGroup);
        free(flowchart_copy);
        free(machine_copy);
        free(arrivals_copy);
    }
    free(swarmGroup);
}

void pga_initialize_swarm_group(struct activity_set * remain, struct flowchart_group * arrivals, struct swarm_group * swarmGroup){
    int total = remain->length;
    for (int i = 0; i < arrivals->size; ++i) {
        for (int j = 1; j < arrivals->flowcharts[i].vertex - 1; ++j) {
            total++;
        }
    }

    int sequence[MAX_ACTIVITY * MAX_FLOWCHART_SIZE];
    int count = 0;

    for (int i = 0; i < remain->length; ++i) {
        int flow_index = remain->queue[i].flow_index;
        int aty_index = remain->queue[i].index;
        int code = flow_index * 10000 + aty_index;

        sequence[count] = code;
        count++;
    }

    for (int i = 0; i < arrivals->size; ++i) {
        for (int j = 1; j < arrivals->flowcharts[i].vertex - 1; ++j) {
            int flow_index = arrivals->flowcharts[i].activities[j].flow_index;
            int aty_index = arrivals->flowcharts[i].activities[j].index;
            int code = flow_index * 10000 + aty_index;

            sequence[count] = code;
            count++;
        }
    }

    swarmGroup->activity_size = total;
    swarmGroup->flowchart_size = arrivals->size;

    for (int i = 0; i < swarm_size; ++i) {

        swarmGroup->individuals[i].flowchart_size = arrivals->size;
        swarmGroup->individuals[i].activity_size = total;

        int times = random_number(200, 500);

        for (int l = 0; l < times; ++l) {
            int a, b;
            do{
                a = random_number(0, total - 1);
                b = random_number(0, total - 1);
            }while (a == b);

            int temp = sequence[a];
            sequence[a] = sequence[b];
            sequence[b] = temp;
        }


        for (int j = 0; j < total; ++j) {
            swarmGroup->individuals[i].heuristic[j] = sequence[j];
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

void pga_calculate_individual_efficency(int start, int end, struct activity_set * remain, struct flowchart_group * arrivals, struct flowchart_group * flowchartGroup, struct machine_group * machineGroup, struct individual indiv){
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

            struct activity chosen = pga_find_prior(set, indiv);
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

struct activity pga_find_prior(struct activity_set * set, struct individual indiv){
    struct activity aty;
    int index = INF;
    for (int i = 0; i < set->length; ++i) {
        int flow_index = set->queue[i].flow_index;
        int aty_index = set->queue[i].index;
        int code = flow_index * 10000 + aty_index;

        for (int j = 0; j < indiv.activity_size; ++j) {
            if (code == indiv.heuristic[j]){
                if (j < index){
                    index = j;
                    aty = set->queue[i];
                }
                break;
            }
        }
    }
    return aty;
}

void pga_swarm_genetic_operations(struct swarm_group * swarmGroup){

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

        //heuristic
        int sequnce1[MAX_FLOWCHART_SIZE * MAX_ACTIVITY];
        int sequnce2[MAX_FLOWCHART_SIZE * MAX_ACTIVITY];
        memset(sequnce1, 0, sizeof(int) * MAX_FLOWCHART_SIZE * MAX_ACTIVITY);
        memset(sequnce2, 0, sizeof(int) * MAX_FLOWCHART_SIZE * MAX_ACTIVITY);
        struct individual copy_parent1 = parent1;
        struct individual copy_parent2 = parent2;
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

        for (int i = low; i <= upper; ++i) {
            int current = parent1.heuristic[i];
            for (int j = 0; j < swarmGroup->activity_size; ++j) {
                if (copy_parent2.heuristic[j] == current){
                    copy_parent2.heuristic[j] = -1;
                }
            }
            current = parent2.heuristic[i];
            for (int j = 0; j < swarmGroup->activity_size; ++j) {
                if (copy_parent1.heuristic[j] == current){
                    copy_parent1.heuristic[j] = -1;
                }
            }
        }

        for (int i = 0; i < swarmGroup->activity_size; ++i) {
            if(copy_parent1.heuristic[i] != -1){
                sequnce1[0]++;
                sequnce1[sequnce1[0]] = copy_parent1.heuristic[i];
            }

            if(copy_parent2.heuristic[i] != -1){
                sequnce2[0]++;
                sequnce2[sequnce2[0]] = copy_parent2.heuristic[i];
            }
        }

        int t = 1;
        for (int i = 0; i < swarmGroup->activity_size; ++i) {
            if (i >= low && i <= upper){
                offsprings->individuals[count].heuristic[i] = parent2.heuristic[i];
                offsprings->individuals[count + 1].heuristic[i] = parent1.heuristic[i];
            } else{
                offsprings->individuals[count].heuristic[i] = sequnce1[t];
                offsprings->individuals[count + 1].heuristic[i] = sequnce2[t];
                t++;
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
        //替换掉heuristic染色体
        for (int j = 0; j < swarmGroup->activity_size; ++j) {
            swarmGroup->individuals[index].heuristic[j] = offsprings->individuals[i].heuristic[j];
        }

        //替换掉mode染色体
        for (int j = 0; j < swarmGroup->flowchart_size; ++j) {
            for (int k = 1; k <= swarmGroup->individuals[index].modes[j][0]; ++k) {
                swarmGroup->individuals[index].modes[j][k] = swarmGroup->individuals[i].modes[j][k];
            }
        }

        swarmGroup->individuals[index].makespan = -1;
        swarmGroup->individuals[index].total_makespan = -1;
        swarmGroup->individuals[index].total_delay = -1;

    }
    free(offsprings);
}

void pga_simulation(int start, int end, struct activity_set * remain, struct flowchart_group * arrivals, struct flowchart_group * flowchartGroup, struct machine_group * machineGroup, struct individual * best){
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

            struct activity chosen = pga_find_prior(set, *best);
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

        if (time >= end){
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