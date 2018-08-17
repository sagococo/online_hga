//
// Created by Dell on 2018/8/17.
//

#ifndef ONLINE_HGA_HGA_H
#define ONLINE_HGA_HGA_H

#include "structure.h"

void hga_process(struct flowchart_group *, struct machine_group *, int);
void hga_find_best(int, int, struct activity_set *, struct flowchart_group *, struct flowchart_group *, struct machine_group *, struct individual *, int);
void hga_calculate_individual_efficency(int, int, struct activity_set *, struct flowchart_group *, struct flowchart_group *, struct machine_group *, struct individual);


void hga_initialize_swarm_group(struct activity_set *, struct flowchart_group *, struct swarm_group *);
void hga_swarm_genetic_operations(struct swarm_group *);

struct activity hga_find_prior(struct activity_set *, struct individual, int);

void hga_simulation(int, int, struct activity_set *, struct flowchart_group *, struct flowchart_group *, struct machine_group *, struct individual *);


#endif //ONLINE_HGA_HGA_H
