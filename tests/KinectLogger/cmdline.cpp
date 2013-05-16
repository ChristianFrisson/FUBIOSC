/*
 cmdline - Command Line (c) 1996-2007 Daniel Roggen

   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
31.10.2007: DR. Corrected bug affecting vararg parameter passing; relying on stdarg now, as should be.

1.12.2006: DR. There are a number of issues:
- only space are allowed on the command line, isspace should be used instead of 32
- the minimum size of a string argument is sizeof(int), due to the zeroeing of the parameters done before the command line processing.
- changed to case sensitive. 
*/

#include <stdio.h>
//#include <unistd.h>
//#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

int __numword(char *X)
{
	int i,n;

	i=n=0;

	while(X[i]!=0)
	{
		if(X[i]==32)
			n++;
		i++;
	}
	n++;
	return n;
}
void __getword(char *X,int n,char *s)
{
	int i,p;

	i=p=0;

	while(i!=n)
	{
		if(X[p]==32)
			i++;
		p++;
	}
	i=0;
	while(X[p]!=32 && X[p]!=0)
	{
		s[i]=X[p];
		p++;
		i++;
	}
	s[i]=0;

}
#define __CMDLINE__MAX_CMDLINE_ARG 256
int ScanCommandLine(char *X,int c,char **v,...)
{
	int a,nw,w,d,f,nj,j,s;
	char x[256];
	//char *firstarg,*arg;
	char *arg;
	// Assumes a maximum of N parameters; let's just go for it, memory is free nowadays, and the function is called only once...
	char *allargs[__CMDLINE__MAX_CMDLINE_ARG];
	char *pchar;
	int *pint;

	//printf("Scancommandline\n");

	//firstarg=(char *)(&v)+4;
	nj=0;
	nw=__numword(X);

	if(nw>__CMDLINE__MAX_CMDLINE_ARG)
		// too many parameters to scan....
		return 1;

	/*printf("ptr to X: %p\n",&X);
	printf("ptr to c: %p\n",&c);
	printf("ptr to v: %p\n",&v);*/
	//printf("firstarg: %p\n",firstarg);
	
	
	
	va_list ap;
	va_start(ap,v);

	// Possibly: we may need to do a parsing of the argument string in order to to know the type of the arguments... here we assume 
	// parameter passing of pointers to char or to int is done in the same way in the compiler calling convention.
	for(w=0;w<nw;w++)
	{
		allargs[w]=va_arg(ap,char *);
		//printf("va_arg w %d - %p *p: %p\n",w,allargs[w],*allargs[w]);
		pint=(int*)allargs[w];
		*pint=0;
		
		
		/*printf("va_arg: %p *p: %p\n",pint,*pint);
		
		arg=firstarg+4*w;
		pint = *(int **)arg;
		printf("w: %d. arg: %p p(data): %p\n",w,arg,pint);
		*pint=0;*/
	}
	
	va_end(ap);
	
	// This strupr is historical: case insensitive processing, however standard C (and GCC in particular) don't alow for string literal manipulation.
	//strupr(X);
	/*
	// This seems buggy
	for(a=1;a<c;a++)
	{
		strupr(v[a]);
	}
	*/

	// On itŠre les arguments de la ligne de commande (argv[1]->argv[n])
	for(a=1;a<c;a++)		// Bypasses argv[0] (name of executable)
	{
		// Recherche de l'‚l‚ment de X qui correspond (p.ex -f et -f@)
		d=-1;
		f=0;
		s=0;
		for(w=0;w<nw;w++)
		{
			__getword(X,w,x);
			if(strcmp(x,"@")==0)
				continue;
			if( (strcmp(x,v[a])==0) && (v[a][strlen(v[a])-1]!='@') )
			{
				d=w;
				break;
			}
			if( (strlen(v[a])+1==strlen(x)) && (x[strlen(x)-1]=='@') && (strncmp(x,v[a],strlen(v[a]))==0) )
			{
				d=w;
				f=1;
				break;
			}
		}
		// Un ‚l‚ment n'a pas ‚t‚ trouv‚. On cherche alors un @.
		if(d==-1)
		{
			j=0;
			d=-1;
			for(w=0;w<nw;w++)
			{
				__getword(X,w,x);
				if(strcmp(x,"@")==0)
				{
					j++;
					if(j>nj)
					{
						d=w;
						nj++;
						s=1;
						break;
					}
				}
			}
			if(d==-1)
			{
				// Il s'agit r‚ellement d'une erreur: un argv ne correspond pas
				// du tout avec les arguments de X.
//				printf("really error\n");
				return 1;
			}
		}
		if(f==0)
		{
			// Ok, argument de type -f. On met un int … 1.
			//arg=firstarg+4*d;
			arg = allargs[d];
			if(s)		// C'est un @
			{
				//printf("pchar arg: %p\n",arg);
				//	pchar = *(char **)arg;		// Old
				pchar = (char*)arg;
				strcpy(pchar,v[a]);
			}
			else		// C'est un -f
			{
				//printf("pint arg: %p\n",arg);
				//pint = *(int **)arg;			// Old
				pint=(int*)arg;
				*pint=1;
			}
		}
		else		// Cas du type -f@
		{
			if(a+1<c)
			{
				//arg=firstarg+4*d;
				arg=allargs[d];
				//pchar = *(char **)arg;		// Old
				pchar =(char *)arg;
				strcpy(pchar,v[a+1]);
				a++;
			}
			else
			{
				// Il manque un argument sur la ligne de commande
//				printf("Missing arg\n");
				return 1;
			}
		}
	}
	return 0;
}

