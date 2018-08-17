//
// Created by Dell on 2018/8/17.
//

#include "commen.h"

void find_stage_arrivals(int start, int end, struct flowchart_group * flowchartGroup, struct flowchart_group * stage_arrivals){
    stage_arrivals->size = 0;
    for (int i = 0; i < flowchartGroup->size; ++i) {
        int time = flowchartGroup->flowcharts[i].arrival_time;
        if(time >= start && time < end){
            stage_arrivals->flowcharts[stage_arrivals->size] = flowchartGroup->flowcharts[i];
            stage_arrivals->size++;
        }
    }
}

int calculate_total_makespan(struct flowchart_group * flowchart_group) {
    int total = 0;
    for (int i = 0; i < flowchart_group->size; ++i) {
        int start = flowchart_group->flowcharts[i].arrival_time;
        int end = flowchart_group->flowcharts[i].finish_time;
        if (end > start) {
            total += end - start;
        }
    }
    return total;
}

void CMPSolve(struct flowchart_group * flowchartGroup, struct individual indiv){
    for (int i = 0; i < indiv.flowchart_size; ++i) {
        for (int j = 1; j < flowchartGroup->flowcharts[i].vertex - 1; ++j) {
            int mode = indiv.modes[i][j];
            flowchartGroup->flowcharts[i].activities[j].durations = flowchartGroup->flowcharts[i].activities[j].modes[mode].duration;
            flowchartGroup->flowcharts[i].activities[j].renewable = flowchartGroup->flowcharts[i].activities[j].modes[mode].renewable;
            flowchartGroup->flowcharts[i].activities[j].nonrenewable= flowchartGroup->flowcharts[i].activities[j].modes[mode].nonrenewable;
        }
        find_lft_est(&flowchartGroup->flowcharts[i]);
    }
}

void find_lft_est(struct flowchart * flow){
    flow->length = find_hinder_longest_length(flow, 0);

    for (int i = 0; i < flow->vertex; ++i) {
        flow->activities[i].lft = flow->length - find_hinder_longest_length(flow, i) + flow->activities[i].durations;
        flow->activities[i].est = find_ahead_longest_length(flow, i) - flow->activities[i].durations;
    }
}

int find_hinder_longest_length(struct flowchart *f, int t) {

    int total = 0;
    int islast = 1;

    //遍历流程图f中的t节点所指向的任务
    for (int i = 1; i <= f->vertex; i++) {
        if (f->map[t][i] != -1) {
            islast = 0;
            int temp = find_hinder_longest_length(f, i);
            if (temp > total) {
                total = temp;
            }
        }
    }

    //判断是否是最后一个节点，如果是，则返回当前节点的duration，如果不是，则返回duration + total
    if (islast == 1) {
        return f->activities[t].durations;
    } else {
        return f->activities[t].durations + total;
    }
}

int find_ahead_longest_length(struct flowchart *f, int t) {

    int total = 0;
    int islast = 1;

    //遍历流程图f中的t节点所指向的任务
    for (int i = 1; i <= f->vertex; i++) {
        if (f->map[i][t] != -1) {
            islast = 0;
            int temp = find_ahead_longest_length(f, i);
            if (temp > total) {
                total = temp;
            }
        }
    }

    //判断是否是最后一个节点，如果是，则返回当前节点的duration，如果不是，则返回duration + total
    if (islast == 1) {
        return f->activities[t].durations;
    } else {
        return f->activities[t].durations + total;
    }
}

void mark_new_arrive(struct flowchart_group * flowchartGroup, int time){
    for (int i = 0; i < flowchartGroup->size; ++i) {
        int arrive = flowchartGroup->flowcharts[i].arrival_time;
        int state = flowchartGroup->flowcharts[i].received;
        if (arrive <= time && state == 0){
            flowchartGroup->flowcharts[i].received = 2;
        }
    }
}

int find_available_machine(struct machine_group * machineGroup, int time){
    int index = -1;
    int max_speed = -1;
    for (int i = 0; i < MACHINE_NUMBER; ++i) {
        if (machineGroup->machines[i].finish <= time && machineGroup->machines[i].speed > max_speed){
            index = i;
            max_speed = machineGroup->machines[i].speed;
        }
    }
    return index;
}
void find_schedualble_aty(struct activity_set * remain, struct flowchart_group * arrivals, int time, struct flowchart_group * flowchartGroup, struct activity_set * set){
    set->length = 0;
    for (int i = 0; i < remain->length; ++i) {
        if (isSchedualble(remain->queue[i], flowchartGroup) == 1){
            set->queue[set->length] = remain->queue[i];
            set->length++;
        }
    }

    for (int i = 0; i < arrivals->size; ++i) {
        if (arrivals->flowcharts[i].received == 2){
            for (int j = 1; j < arrivals->flowcharts[i].vertex - 1; ++j) {
                if (isSchedualble(arrivals->flowcharts[i].activities[j], flowchartGroup) == 1){
                    set->queue[set->length] = arrivals->flowcharts[i].activities[j];
                    set->length++;
                }
            }
        }
    }
}

int isSchedualble(struct activity activity, struct flowchart_group * flowchartGroup){
    int index_flow = activity.flow_index;
    int index_aty = activity.index;

    if (flowchartGroup->flowcharts[index_flow].activities[index_aty].state != 0){
        return 0;
    }

    int isOK = 1;
    for (int i = 0; i < flowchartGroup->flowcharts[index_flow].vertex - 1; ++i) {
        if (flowchartGroup->flowcharts[index_flow].map[i][index_aty] != -1 &&
            flowchartGroup->flowcharts[index_flow].activities[i].state != 1){
            isOK = 0;
            return isOK;
        }
    }
    return isOK;
}

int check_flowchart(struct flowchart flow){
    for (int i = 1; i < flow.vertex - 1; ++i) {
        if (flow.activities[i].state != 1){
            return 0;
        }
    }
    return 1;
}

void copy_to_flowchart_group(struct flowchart_group * arrivals, struct flowchart_group * flowchartGroup){
    for (int i = 0; i < arrivals->size; ++i) {
        int flow_index = arrivals->flowcharts[i].flowchart_index;
        flowchartGroup->flowcharts[flow_index] = arrivals->flowcharts[i];
    }
}

void find_remain(int end, struct flowchart_group * flowchartGroup, struct activity_set * remain){
    remain->length = 0;
    for (int i = 0; i < flowchartGroup->size; ++i) {
        if (flowchartGroup->flowcharts[i].received == 2){
            for (int j = 1; j < flowchartGroup->flowcharts[i].vertex - 1; ++j) {
                int state = flowchartGroup->flowcharts[i].activities[j].state;
                if (state == 0){
                    remain->queue[remain->length] = flowchartGroup->flowcharts[i].activities[j];
                    remain->length++;
                }
            }
        }
    }

}