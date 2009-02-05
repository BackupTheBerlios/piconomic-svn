####################################################################
# Copyright (c) 2008 Pieter Conradie <pieterconradie@users.berlios.de>
# All rights reserved.
#
# GDB Script to initialise board to a known state for debugging
#
# If this script file is named ".gdbinit" it will be executed 
# automatically when GDB is started.
#
# Useful GDB commands
#
# source script.gdb : read and execute GDB commands from file "script.gdb"
# c                 : continue running the program
# s                 : step
# n                 : next
# break main        : set a breakpoint function "main"
# thbreak main      : set a temporary hardware assisted breakpoint at function "main"
# info registers    : display CPU register values
# info mem          : display memory regions as defined by OpenOCD
# display/i $pc     : display current assembly instruction
#
# Revision Info     :  $Id: program_and_debug.gdb 1 2008-12-02 07:02:22Z Pieter.Conradie $
#
####################################################################

# Set up the GDB debugging environment
set output-radix 16
set input-radix 16

# Change timeout to 10 seconds
set remotetimeout 10

# Specify the TCP/IP port OpenOCD GDB Server is using to listen for gdb commands
target remote localhost:3333

# Reset the target
monitor reset

# Program flash and verify
load
compare-sections

# Force HW breakpoints when GDB request SW breakpoints
monitor arm7_9 force_hw_bkpts enable

# Set a temporary breakpoint at "main()"
thbreak main

# Run to breakpoint
continue
