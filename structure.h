//
// Created by Dell on 2018/8/13.
//

#ifndef ONLINEHGA_STRUCTURE_H
#define ONLINEHGA_STRUCTURE_H

//constants
#define MAX_ACTIVITY 30 //工作流中活动的最大数量
#define MAX_SWARM_SIZE 1300 //群体中染色体的最大数量
#define MAX_MACHINE_NUMBER 30 //机器的最大数量
#define MACHINE_NUMBER 10 //机器的数量
#define MAX_FLOWCHART_SIZE 30 //工作流的最大数量
#define GEN 100 //进化的代数
#define HEURISTIC_NUMBER 6 //使用到的启发方法的数量
#define STAGE_INTERVAL 200 //每个阶段的持续时间


#define INF 9999999 //定义无穷大的数值
#define MUTATION_RATE 0.008 //表示突变的概率
#define OFFSPRING_KEEP_RATE 0.2 //子代中保留的概率

#define RENEWABLE_NUMBER 500 //可重新使用资源的数量
#define NONRENEWABLE_NUMBER 1200 //不可重新使用资源的数量

//边的大小范围
#define EDGE_UPPER 5000 //
#define EDGE_LOWER 2000

//持续时间的大小范围
#define DURATION_UPPER 3000
#define DURATION_LOWER 1000

//每种模式中，所需可重新使用资源的数量范围
#define RENEWABLE_UPPER 3
#define RENEWABLE_LOWER 1

//每种模式中，所需不可重新使用资源的数量范围
#define NONRENEWABLE_UPPER 3
#define NONRENEWABLE_LOWER 1

//每个activity所拥有的模式个数范围
#define MODE_QUANTITY_UPPER 3
#define MODE_QUANTITY_LOWER 1

//机器的速度定义范围
#define MACHINE_SPEED_UPPER 200
#define MACHINE_SPEED_LOWER 50

struct mode {
    int renewable; //需要可重复利用资源的个数
    int nonrenewable; //需要不可重复利用资源的个数
    int duration; //当前模式下任务的持续时间
};

struct activity {
    int index; //当前活动的索引
    int flow_index; //当前活动所在的flow索引
    int mode_quantity; //当前活动的执行模式的个数
    struct mode modes[MAX_ACTIVITY]; //当前活动的执行模式
    int state; //当前活动是否已经完成,未完成为0，完成为1, 正在进行为2

    int est; //earliest start time
    int lft; //last finish time

    int durations;
    int renewable;
    int nonrenewable;
};

struct flowchart {
    int flowchart_index;
    int vertex; //活动的个数
    int length; //整个流程图的长度
    int arrival_time; //工作流到达的时间
    int finish_time; //工作流完成的时间
    int received; //工作流是否已接受 0:未接受 1：已完成 2：处理中
    int map[MAX_ACTIVITY][MAX_ACTIVITY]; //流程图的时序关系图
    struct activity activities[MAX_ACTIVITY]; //流程图的活动

};

struct flowchart_group{
    int size;
    struct flowchart flowcharts[MAX_FLOWCHART_SIZE];
};

struct machine {

    int finish;

    int speed; //机器的速度
    int cost; //机器的消耗

    int flow_index; //最后一个处理的活动的flow索引
    int activity_index; //最后一个处理的活动的索引
};

struct machine_group {
    struct machine machines[MAX_MACHINE_NUMBER]; //表示一个机器组
};

struct individual {
    int modes[MAX_FLOWCHART_SIZE][MAX_ACTIVITY]; //模式染色体
    int heuristic[MAX_ACTIVITY * MAX_FLOWCHART_SIZE]; //启发算法染色体
    int makespan;
    int total_makespan;
    int total_delay;
    int cost;

    int flowchart_size;
    int activity_size;
};

struct swarm_group {
    int activity_size;
    int flowchart_size;
    struct individual individuals[MAX_SWARM_SIZE];
};

struct activity_set {
    int length;
    struct activity queue[MAX_ACTIVITY * MAX_FLOWCHART_SIZE];
};

#endif //ONLINEHGA_STRUCTURE_H
