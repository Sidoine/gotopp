#ifndef _BASE_H
#define _BASE_H

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifndef _WIN32
#include <wchar.h>
#include <config.h>
#ifdef _UNICODE
#include "wlib.h"
#endif
#else
#include "configwin.h"
#endif
#include <assert.h>
#include <gc_cpp.h>

#include "gppmodule.h"

namespace GotoPP
{
	typedef unsigned char byte;
	typedef unsigned long dword;
	typedef unsigned char uchar;
	typedef unsigned int uint;
	typedef int id;


	class IInterpreteur;
	
	const int TMAXIDENTIFIEUR=63;

	#ifdef DEFINITIONS
	#define GPP_GLOBAL
	#else
	#define GPP_GLOBAL extern
	#endif

	#ifdef _WIN32
	#	define ADRESSEFONC(x) x
	#else
	#	define ADRESSEFONC(x) &x
	#endif

	#ifdef _WIN32
	#define TACHE uintptr_t
	#else
	#define TACHE pthread_t
	#endif


#ifdef _UNICODE
	typedef wchar_t ucarac;
	#define TXT(x) L##x
	#define czcpy wcscpy
	#define czlen wcslen
	#define czncmp wcsncmp
	#define czcmp wcscmp
	#define czgetenv _wgetenv
	#define czvcprintf _vscwprintf
	
#ifdef _WIN32
#	define czncpy wcsncpy_s
#	define czvprintf vswprintf_s
#	define	czvnprintf vswnprintf_s
#else
#	define czvprintf vswprintf
#	define czvnprintf vswprintf
#	define czncpy wcsncpy
#endif
	#define czscspn wcscspn
	#define czschr wcschr
	#define putcarac putwchar
#ifdef _WIN32	
	#define cznprintf swprintf_s
#else
	#define cznprintf swprintf
#endif
	#define czcat wcscat
	#define czfopen _wfopen
	#define czfputws fputws
	#define czfputs(x,F) fprintf(F,"%S",x)
	#define oputcz(x) fputws(x,stdout)
	#define fputcz fputws
	#define czpopen _wpopen
	#define czoprintf wprintf
	#define czmain wmain
	#define czout wcout
	#define czin wcin
	#define czerr wcerr
	#define czstr wcsstr
	#define cztostr wcstombs
	#define cztowcs wcsncpy
	#define strtocz mbstowcs
#ifdef _WIN32
	#define cztoi _wtoi
	#define cztof _wtof
#else
	#define cztoi(x) wcstoul(x,0,10)
	#define cztof(x) wcstod(x,0)
#endif
#else
	typedef unsigned char ucarac;
	#define TXT(x) x
	#define czcpy strcpy
	#define czlen strlen
	#define czncpy strncpy
	#define czncmp strncmp
	#define czcmp strcmp
	#define czgetenv getenv
	#define czvcprintf _vscprintf
	#define czvprintf vsprintf
	#define	czvnprintf vsnprintf
	#define czscspn strcspn
	#define czschr strchr
	#define putcarac putchar
#ifdef _WIN32
	#define cznprintf _snprintf_s
#else
	#define cznprintf snprintf
#endif
	#define czcat strcat
	#define czfopen fopen
	#define czfputws(x,F) fprintf(F,"%S",x)
	#define czfputs(x,F) fputs(x,F)
	#define oputcz(x) puts(x,stdout)
	#define fputcz fputs
#ifdef _WIN32
	#define czpopen _popen
#else
	#define czpopen popen
#endif
	#define czoprintf printf
	#define czmain main
	#define czout cout
	#define czin cin
	#define czerr cerr
	#define czstr strstr
	#define cztostr strncpy
	#define cztowcs mbstowcs
	#define strtocz strncpy
	#define cztoi atoi
	#define cztof atof
#endif

	typedef void FnAfficherErreur(const GotoPP::carac * texte, const GotoPP::carac * fichier,
					int ligne, bool execution);
	extern FnAfficherErreur * AfficherErreur;
}

#ifdef _DEBUG
#define ASSERT(x) assert(x)
#else
#define ASSERT(x)
#endif

#include "tableaux.h"
#include "type/valeur.h"
#include "symbole.h"
#endif
