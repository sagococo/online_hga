//
// Created by Dell on 2018/7/12.
//

#include <stdlib.h>
#include "tools.h"

#include "stdio.h"

int random_number(int start, int end){
    return start + rand()%(end - start + 1);
}