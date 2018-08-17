//
// Created by Dell on 2018/8/17.
//

#ifndef ONLINE_HGA_COMMEN_H
#define ONLINE_HGA_COMMEN_H

#include "structure.h"

void find_stage_arrivals(int, int, struct flowchart_group *, struct flowchart_group *);
int calculate_total_makespan(struct flowchart_group *);

void CMPSolve(struct flowchart_group *, struct individual);
void find_lft_est(struct flowchart *);
int find_hinder_longest_length(struct flowchart *, int);
int find_ahead_longest_length(struct flowchart *, int);

void mark_new_arrive(struct flowchart_group *, int);
int find_available_machine(struct machine_group *, int);
void find_schedualble_aty(struct activity_set *, struct flowchart_group *, int, struct flowchart_group *, struct activity_set *);
int isSchedualble(struct activity, struct flowchart_group *);

int check_flowchart(struct flowchart);
void copy_to_flowchart_group(struct flowchart_group *, struct flowchart_group *);

void find_remain(int, struct flowchart_group *, struct activity_set *);

#endif //ONLINE_HGA_COMMEN_H
