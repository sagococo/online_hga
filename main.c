#include <stdio.h>
#include <malloc.h>
#include "prepare_data.h"
#include "structure.h"

int datas = 20;
int flowchart_size = 15;
int swarm_size = 1000;
int stage_interval = 200;

int main(int argc, char * argv[]) {
    //    flowchart_size = strtol(argv[1], NULL, 10);
    //    swarm_size = strtol(argv[2], NULL, 10);

    flowchart_size = 15;
    swarm_size = 1000;

    struct machine_group * machineGroup = malloc(sizeof(struct machine_group));
    initialize_machine(machineGroup);

    struct flowchart_group * all_flowchartGroup = malloc(sizeof(struct flowchart_group));
    read_map(all_flowchartGroup);

    struct flowchart_group * flowchartGroup = malloc(sizeof(struct flowchart_group));
    pick_group(all_flowchartGroup, flowchartGroup, flowchart_size);

    for (int i = 0; i < datas; ++i) {
        initialize_flowcharts(flowchartGroup, i);

        printf("hga (%d, %d, %d)\n", flowchart_size, swarm_size, i);

    }

    return 0;
}