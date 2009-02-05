@echo off

REM Copyright (c) 2008 Pieter Conradie <pieterconradie@users.berlios.de>, All rights reserved
REM Revision Info:  $Id: insight_dbg.bat 1 2008-12-02 07:02:22Z Pieter.Conradie $

start "ARM-ELF-INSIGHT" /MIN arm-elf-insight.exe -x debug.gdb ../../../../DEBUG/PROJECT.elf