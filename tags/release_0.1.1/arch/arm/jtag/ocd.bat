@echo off

REM Copyright (c) 2008 Pieter Conradie <pieterconradie@users.berlios.de>, All rights reserved
REM Revision Info:  $Id: ocd.bat 1 2008-12-02 07:02:22Z Pieter.Conradie $

start "OpenOCD" /MIN openocd.exe -f interface/arm-usb-tiny.cfg -f target/TARGET.cfg