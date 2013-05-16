/*
   console io

   Copyright (C) 2008,2009:
         Daniel Roggen, droggen@gmail.com

   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <wincon.h>
//#include <windows.h>


#ifdef WIN32

#include <wtypes.h>
int ConsoleInit(void)
{
	//create debug console
	AllocConsole();
 
	//initial screen buffer so console commands will work	
 	HANDLE hStdout=CreateConsoleScreenBuffer(GENERIC_WRITE|GENERIC_READ,FILE_SHARE_WRITE, NULL,CONSOLE_TEXTMODE_BUFFER, NULL);
 	
	//error check
	if(hStdout==0) 
		return 1;
 		
	SetConsoleActiveScreenBuffer(hStdout);
 	
	//lets text formating (ie newlines, etc)  work with writeconsole()
	SetConsoleMode(hStdout, ENABLE_PROCESSED_OUTPUT);
 
	//redirects std output to console buffer, so that printf works
   //freopen( "CON", "w", stdout );  //redirects stdout to console, but
	//doesn"t work after SetConsoleActiveScreenBuffer issued
   freopen("CONOUT$", "wt", stdout);
	//freopen("CONOUT$", "wta+", stdout);
 	
	//Also let us read from console with c library, since I can"t get 
	//ReadConsole to work
	freopen("CONIN$","r",stdin);
 	
	//set console title
	char *title="Console";
#ifdef UNICODE
	 // Convert to a wchar_t*
    size_t origsize = strlen(title) + 1;
    const size_t newsize = 128;
    size_t convertedChars = 0;
    wchar_t wcstring[newsize];
    mbstowcs(wcstring, title, newsize);
//    wcscat_s(wcstring, L" (wchar_t *)");

	SetConsoleTitle(wcstring);
#endif
#ifndef UNICODE
	SetConsoleTitle(title);
#endif
 	
	//set buffer size
	COORD crd;
	crd.X=80;
	crd.Y=3000;
	SetConsoleScreenBufferSize(hStdout, crd);	
}
#endif


#ifdef LINUX

int ConsoleInit(void)
{
   return 0;
}

#endif





