/*
 * @(#)syscall.s
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

/**
 ** vt-os 1.1
 **
 ** -- Aaron Ardiri, 2000
 **/

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
SYSCALL(pmalloc                                    ,48)
SYSCALL(pfree                                      ,50)
SYSCALL(strlen                                     ,119)
SYSCALL(strcpy                                     ,121)
SYSCALL(memset                                     ,573)
SYSCALL(strncmp                                    ,583)

/**
 ** vt-os 1.3
 **
 ** -- Aaron Ardiri, 2000
 **/

#undef SYSCALL
#define SYSCALL(name,number)                       \
        .globl   name;                             \
        .ent     name;                             \
name:;                                             \
        syscall  number;                           \
        .end     name;                             \

        .text

// vt-os 1.3.01

SYSCALL(LcdGetLcdMemAddress                        ,828)
