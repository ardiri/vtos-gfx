#
# @(#)Makefile
#
# Copyright 2000, Aaron Ardiri (mailto:aaron@ardiri.com)
# All rights reserved.
#
# This file  was generated as part  of the "gfx" graphics library that
# is specifically  developed for the Helio Computing Platform designed 
# by vtech:
#
#   http://www.vtechinfo.com/
#
# The contents of this file is confidential and  proprietary in nature
# ("Confidential Information"). Redistribution or modification without 
# prior consent of the original author is prohibited.
#

# compiler setup
PREFIX    = 
AS        = $(PREFIX)mips-vtos-gcc
CC        = $(PREFIX)mips-vtos-gcc
AR        = $(PREFIX)mips-vtos-ar

# compiler flags 
ASFLAGS   = -O2 
CCFLAGS   = -O2 
ARFLAGS   = -r

# compiler commands 

# source / outputs
OBJS      = gfx.o
LDOBJS    = sys0000.o sys0001.o sys0002.o sys0003.o sys0004.o sys0005.o sys0006.o

# compile requirements
gfx.a: $(OBJS) $(LDOBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS) $(LDOBJS)
	cp *.[aA] ..
	make clean

# compile rules
%.o: %.s
	cp $< tmp.S; $(AS) -c $(ASFLAGS) tmp.S -o $@
%.o: %.c
	$(CC) -c $(CCFLAGS) $<

# clean-up funtions
clean:
	rm -f *.[oO] *~ tmp.S

distclean:
	rm -f *.[oO] *.[aA] *~ tmp.S
