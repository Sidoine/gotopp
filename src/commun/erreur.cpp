#include <gotopp/base.h>
#include <gotopp/erreur.h>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
using namespace std;

GotoPP::CErreur::CErreur(carac * Format,...)
{
	va_list Args;
	va_start( Args, Format);
#ifdef _WIN32
	size_t l=czvcprintf(Format,Args);
    texte=new carac[l+2];
	czvprintf(Texte(),l+1,Format,Args);
#else
	texte=new carac[128];
	czvnprintf(Texte(),127,Format,Args);
#endif
	va_end(Args);
	Compteur()=1;
}



GotoPP::CErreur::~CErreur()
{
	if (texte)
	{
		ASSERT(Compteur()>0);
		Compteur()--;
		if (Compteur()==0)
			delete[] texte;
	}
}
