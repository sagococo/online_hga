//
// Created by Dell on 2018/8/17.
//

#include "heuristic_func.h"
#include "structure.h"

struct activity min_sk(struct activity_set * set) {
    int min = INF;
    int index = 0;
    for (int i = 0; i < set->length; ++i) {
        int temp = set->queue[i].lft - set->queue[i].est - set->queue[i].durations;
        if (temp < min) {
            min = temp;
            index = i;
        }
    }
    return set->queue[index];
}

struct activity min_lft(struct activity_set *set) {
    int min = INF;
    int index = 0;
    for (int i = 0; i < set->length; ++i) {
        int temp = set->queue[i].lft;
        if (temp < min) {
            min = temp;
            index = i;
        }
    }
    return set->queue[index];
}

struct activity min_spt(struct activity_set *set) {
    int min = INF;
    int index = 0;
    for (int i = 0; i < set->length; ++i) {
        int temp = set->queue[i].durations;
        if (temp < min) {
            min = temp;
            index = i;
        }
    }
    return set->queue[index];
}

struct activity min_lst(struct activity_set *set) {
    int min = INF;
    int index = 0;
    for (int i = 0; i < set->length; ++i) {
        int temp = set->queue[i].lft - set->queue[i].durations;
        if (temp < min) {
            min = temp;
            index = i;
        }
    }
    return set->queue[index];
}

struct activity min_est(struct activity_set *set) {
    int min = INF;
    int index = 0;
    for (int i = 0; i < set->length; ++i) {
        int temp = set->queue[i].est;
        if (temp < min) {
            min = temp;
            index = i;
        }
    }
    return set->queue[index];
}

struct activity min_eft( struct activity_set *set) {
    int min = INF;
    int index = 0;
    for (int i = 0; i < set->length; ++i) {
        int temp = set->queue[i].est + set->queue[i].durations;
        if (temp < min) {
            min = temp;
            index = i;
        }
    }
    return set->queue[index];
}