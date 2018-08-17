//
// Created by Dell on 2018/8/17.
//

#ifndef ONLINE_HGA_PGA_H
#define ONLINE_HGA_PGA_H

#include "structure.h"

void pga_process(struct flowchart_group *, struct machine_group *, int);
void pga_find_best(int, int, struct activity_set *, struct flowchart_group *, struct flowchart_group *, struct machine_group *, struct individual *, int);

void pga_initialize_swarm_group(struct activity_set *, struct flowchart_group *, struct swarm_group *);
void pga_calculate_individual_efficency(int, int, struct activity_set *, struct flowchart_group *, struct flowchart_group *, struct machine_group *, struct individual);

struct activity pga_find_prior(struct activity_set *, struct individual);
void pga_swarm_genetic_operations(struct swarm_group *);

void pga_simulation(int, int, struct activity_set *, struct flowchart_group *, struct flowchart_group *, struct machine_group *, struct individual *);


#endif //ONLINE_HGA_PGA_H
