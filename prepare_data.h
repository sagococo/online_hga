//
// Created by xiao on 2018/8/4.
//

#ifndef ONLINEHGA_PREPARE_DATA_H
#define ONLINEHGA_PREPARE_DATA_H

#include "structure.h"

void initialize_machine(struct machine_group *);
void read_map(struct flowchart_group *);
void pick_group(struct flowchart_group *, struct flowchart_group *, int);
void initialize_flowcharts(struct flowchart_group *, int);

#endif //ONLINEHGA_PREPARE_DATA_H
