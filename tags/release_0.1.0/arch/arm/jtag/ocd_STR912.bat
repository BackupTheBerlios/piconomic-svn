@echo off

REM Copyright (c) 2008 Pieter Conradie <pieterconradie@users.berlios.de>, All rights reserved
REM Revision Info:  $Id $

start "OpenOCD" /MIN openocd.exe -f interface/arm-usb-tiny.cfg -f target/STR912.cfg