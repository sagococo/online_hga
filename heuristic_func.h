//
// Created by Dell on 2018/8/17.
//

#ifndef ONLINE_HGA_HEURISTIC_FUNC_H
#define ONLINE_HGA_HEURISTIC_FUNC_H

#include "structure.h"

struct activity min_sk(struct activity_set *);
struct activity min_lft(struct activity_set *);
struct activity min_spt(struct activity_set *);
struct activity min_lst(struct activity_set *);
struct activity min_est(struct activity_set *);
struct activity min_eft(struct activity_set *);

#endif //ONLINE_HGA_HEURISTIC_FUNC_H
