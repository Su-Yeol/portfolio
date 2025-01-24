
                    UMSD Debug Tool Release version 7.0.0
                   =======================================
MCLI Release version 7.0.0 GPL 2.0 Compliant.

This program is free software; you can redistribute it and/or modify it under the terms 
of the GNU General Public License as published by the Free Software Foundation; 
either version 2 of the License, or (at your option) any later version.
 
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License along with this program;
if not, write to the Free Software Foundation, Inc., 
675 Mass Ave, Cambridge, MA 02139, USA.			


Table of Content:
-----------------
1) Release History 
2) Source Code Organization
3) General Introduction
4) How to - use this debug tool

1) Release History 
------------------
UMSD_Debug_Tool_v7.0.0.tar.gz - Apr.10.2023
UMSD_Debug_Tool_v6.0.1.tar.gz - Oct.31.2022
UMSD_Debug_Tool_v3.0.4.tar.gz - Mar.29.2019
UMSD_Debug_Tool_v3.0.1.tar.gz - Sep.30.2018
UMSD_Debug_Tool_v3.0.0_Alpha.tar.gz - Feb.09.2018
UMSD_Debug_Tool_v3.0.0.tar.gz - Dec.27.2017
         First individual release for MCLI

2) Source Code Organization
--------------------------
	a) include, header files for MCLI commands methods and structures defines
	b) src, MCLI implementation source code
	c) lib, third party libraries, cJSON and libcli
	e) linux, Linux project
	f) visual studio, Windows project(visual studio) 

3) General Introduction
-----------------------
	MCLI program emulates one telnet command line interface for UMSD API verification
	There will be one command for each UMSD API, and it validates both chip and API functions.
	
4) How to - use this debug tool
---------------------------------	
	This MCLI utilities is used to verify the UMSD Driver, you should move this host folder to
	UMSD root folder. and then refer the host/linux/README_CLI.txt to compile and run this application.


