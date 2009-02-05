REM Copyright (c) 2008 Pieter Conradie <pieterconradie@users.berlios.de>, All rights reserved
REM Revision Info:  $Id: telnet.bat 1 2008-12-02 07:02:22Z Pieter.Conradie $

@echo off
start "PuTTY" "C:\Program Files\PuTTY\putty.exe" -telnet -P 4444 localhost
