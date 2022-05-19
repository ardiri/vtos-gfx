/*
 * @(#)sys0003.s
 *
 * Copyright 2000, Aaron Ardiri (mailto:aaron@ardiri.com)
 * All rights reserved.
 *
 * This file  was generated as part  of the "gfx" graphics library that
 * is specifically  developed for the Helio Computing Platform designed 
 * by vtech:
 *
 *   http://www.vtechinfo.com/
 *
 * The contents of this file is confidential and  proprietary in nature
 * ("Confidential Information"). Redistribution or modification without 
 * prior consent of the original author is prohibited.
 */

#include <pr3910.h>

#define SYSCALL(name,number)                       \
        .globl   name;                             \
        .ent     name;                             \
name:;                                             \
        .set     noat;                             \
        li       v0, number;                       \
        syscall;                                   \
        j        ra;                               \
        .set     at;                               \
        .end     name;                             \

        .text
SYSCALL(strcpy                                     ,121)
