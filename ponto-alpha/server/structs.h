//
// Created by root on 05/10/18.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef UNTITLED_STRUCTS_H
#define UNTITLED_STRUCTS_H

struct MemoryStruct {
    char *memory;
    size_t size;
};

struct user_list{
    int user_id;
    char *name;
    char *role;
    char *fingerprint;
};

#endif //UNTITLED_STRUCTS_H
