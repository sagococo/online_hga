//
// Created by xiao on 2018/8/4.
//

#include <stdio.h>
#include <mem.h>
#include <stdlib.h>
#include "prepare_data.h"
#include "tools.h"

void initialize_machine(struct machine_group * machineGroup){
    for (int i = 0; i < MACHINE_NUMBER; i++) {
        machineGroup->machines[i].speed = random_number(MACHINE_SPEED_LOWER, MACHINE_SPEED_UPPER);
        machineGroup->machines[i].cost = machineGroup->machines[i].speed * 2;
        machineGroup->machines[i].finish = 0;
    }
}


void read_map(struct flowchart_group * flowchartgroup){
    //从文件中读取流程图
    FILE * file = fopen("test.txt", "r");
    int size = 0;
    fscanf(file, "%d", &size);

    flowchartgroup->size = size;

    //遍历每个flow
    for (int i = 0; i < size; ++i) {
        int vertex, edges;
        fscanf(file, "%d %d", &vertex, &edges);
        flowchartgroup->flowcharts[i].vertex = vertex;
        flowchartgroup->flowcharts[i].flowchart_index = i;

        flowchartgroup->flowcharts[i].length = -1;
        memset(flowchartgroup->flowcharts[i].map, -1, MAX_ACTIVITY * MAX_ACTIVITY * sizeof(int));

        int f[MAX_ACTIVITY];
        int t[MAX_ACTIVITY];
        memset(f, 0, MAX_ACTIVITY * sizeof(int));
        memset(t, 0, MAX_ACTIVITY * sizeof(int));

        //从txt文件中读取map
        for (int j = 0; j < edges; ++j) {
            int from, to;
            fscanf(file, "%d %d", &from, &to);
            flowchartgroup->flowcharts[i].map[from][to] = random_number(EDGE_LOWER, EDGE_UPPER);
            f[from] = 1;
            t[to] = 1;
        }

        //找出没有被指向的vertex，由0节点开始指向
        //找出没有指向的vertex，全部指向一个新的节点
        for (int j = 1; j <= flowchartgroup->flowcharts[i].vertex; ++j) {
            if (f[j] == 0) {
                flowchartgroup->flowcharts[i].map[j][flowchartgroup->flowcharts[i].vertex + 1] = 0;
            }
            if (t[j] == 0) {
                flowchartgroup->flowcharts[i].map[0][j] = 0;
            }
        }

        //添加开始和结束两个节点
        flowchartgroup->flowcharts[i].vertex += 2;

        //初始化第一个和最后一个activity
        int start = 0, end = flowchartgroup->flowcharts[i].vertex - 1;
        flowchartgroup->flowcharts[i].activities[start].index = 0;
        flowchartgroup->flowcharts[i].activities[start].mode_quantity = 0;
        flowchartgroup->flowcharts[i].activities[start].state = 1;
        flowchartgroup->flowcharts[i].activities[start].durations = 0;
        flowchartgroup->flowcharts[i].activities[start].renewable = 0;
        flowchartgroup->flowcharts[i].activities[start].nonrenewable = 0;
        flowchartgroup->flowcharts[i].activities[start].flow_index = i;

        flowchartgroup->flowcharts[i].activities[end].index = end;
        flowchartgroup->flowcharts[i].activities[end].mode_quantity = 0;
        flowchartgroup->flowcharts[i].activities[end].state = 1;
        flowchartgroup->flowcharts[i].activities[end].durations = 0;
        flowchartgroup->flowcharts[i].activities[end].renewable = 0;
        flowchartgroup->flowcharts[i].activities[end].nonrenewable = 0;
        flowchartgroup->flowcharts[i].activities[end].flow_index = i;

        //初始化其他的activity
        for (int k = 1; k < end; ++k) {
            flowchartgroup->flowcharts[i].activities[k].flow_index = i;
            flowchartgroup->flowcharts[i].activities[k].index = k;
            flowchartgroup->flowcharts[i].activities[k].state = 0;
            flowchartgroup->flowcharts[i].activities[k].mode_quantity = random_number(MODE_QUANTITY_LOWER, MODE_QUANTITY_UPPER);

            //初始化每个activity的不同mode
            for (int t = 0; t < flowchartgroup->flowcharts[i].activities[k].mode_quantity; ++t) {
                flowchartgroup->flowcharts[i].activities[k].modes[t].renewable = random_number(RENEWABLE_LOWER, RENEWABLE_UPPER);
                flowchartgroup->flowcharts[i].activities[k].modes[t].nonrenewable = random_number(NONRENEWABLE_LOWER, NONRENEWABLE_UPPER);
                flowchartgroup->flowcharts[i].activities[k].modes[t].duration = random_number(DURATION_LOWER, DURATION_UPPER);
            }
        }


        //初始化received
        flowchartgroup->flowcharts[i].received = 0;
    }

    //流程图到达时间初始化
    flowchartgroup->flowcharts[0].arrival_time = 0;
    flowchartgroup->flowcharts[1].arrival_time = 22;
    flowchartgroup->flowcharts[2].arrival_time = 56;
    flowchartgroup->flowcharts[3].arrival_time = 34;
    flowchartgroup->flowcharts[4].arrival_time = 84;
    flowchartgroup->flowcharts[5].arrival_time = 120;
    flowchartgroup->flowcharts[6].arrival_time = 150;
    flowchartgroup->flowcharts[7].arrival_time = 186;
    flowchartgroup->flowcharts[8].arrival_time = 222;
    flowchartgroup->flowcharts[9].arrival_time = 266;
    flowchartgroup->flowcharts[10].arrival_time = 286;
    flowchartgroup->flowcharts[11].arrival_time = 304;
    flowchartgroup->flowcharts[12].arrival_time = 314;
    flowchartgroup->flowcharts[13].arrival_time = 320;
    flowchartgroup->flowcharts[14].arrival_time = 370;
    flowchartgroup->flowcharts[15].arrival_time = 372;
    flowchartgroup->flowcharts[16].arrival_time = 392;
    flowchartgroup->flowcharts[17].arrival_time = 444;
    flowchartgroup->flowcharts[18].arrival_time = 448;
    flowchartgroup->flowcharts[19].arrival_time = 483;
    flowchartgroup->flowcharts[20].arrival_time = 496;
    flowchartgroup->flowcharts[21].arrival_time = 505;
    flowchartgroup->flowcharts[22].arrival_time = 532;
    flowchartgroup->flowcharts[23].arrival_time = 567;
    flowchartgroup->flowcharts[24].arrival_time = 589;

    fclose(file);
}

void pick_group(struct flowchart_group * all_flowcharts, struct flowchart_group * flowchartGroup, int size){
    flowchartGroup->size = size;

    int chose[MAX_FLOWCHART_SIZE];
    memset(chose, 0, MAX_FLOWCHART_SIZE * sizeof(int));

    int count = 0;
    while (count < size){
        int chosen = random_number(0, all_flowcharts->size - 1);
        if (chose[chosen] != 0){
            continue;
        }
        flowchartGroup->flowcharts[count] = all_flowcharts->flowcharts[chosen];
        flowchartGroup->flowcharts[count].flowchart_index = count;
        for (int i = 0; i < flowchartGroup->flowcharts[count].vertex; ++i) {
            flowchartGroup->flowcharts[count].activities[i].flow_index = count;
        }
        count++;
        chose[chosen] = 1;
    }
}

void initialize_flowcharts(struct flowchart_group * flowchartGroup, int seed){
    srand(seed);

    for (int i = 0; i < flowchartGroup->size; ++i) {
        int end = flowchartGroup->flowcharts[i].vertex - 1;

        for (int k = 1; k < end; ++k) {
            flowchartGroup->flowcharts[i].activities[k].mode_quantity = random_number(MODE_QUANTITY_LOWER, MODE_QUANTITY_UPPER);

            //初始化每个activity的不同mode
            for (int t = 0; t < flowchartGroup->flowcharts[i].activities[k].mode_quantity; ++t) {
                flowchartGroup->flowcharts[i].activities[k].modes[t].renewable = random_number(RENEWABLE_LOWER, RENEWABLE_UPPER);
                flowchartGroup->flowcharts[i].activities[k].modes[t].nonrenewable = random_number(NONRENEWABLE_LOWER, NONRENEWABLE_UPPER);
                flowchartGroup->flowcharts[i].activities[k].modes[t].duration = random_number(DURATION_LOWER, DURATION_UPPER);
            }
        }
    }
}
