#include <gotopp/base.h>

using namespace GotoPP;

#ifdef _UNICODE
chaineval * chaineval::Creer(const char * a)
{
	return Creer(a,strlen(a));
}

chaineval * chaineval::Creer(const char * a,size_t l)
{
	chaineval * v=Creer(l);
	cznprintf(v->c,l+1,L"%.*S",l,a);
	v->c[l]=0;
	return v;
}
#else
chaineval * chaineval::Creer(const wchar_t * a)
{
	size_t l=wcslen(a);
	chaineval * v=Creer(l);
	sprintf(v->c,"%S",a);
	v->c[l]=0;
	return v;
}
#endif

void chaineval::Construire(chaineval * & v, const carac * c, size_t l, size_t al)
{
	if (l==0)
		return;

	
	if (v)
	{
		if (l+v->l+1>al)
		{
			al=(al+l)*4/3*sizeof(carac)+sizeof(chaineval);
		    v=(chaineval*)GC_REALLOC(v,al);
		}
	}
	else
	{
		al=32*sizeof(carac);
		v=(chaineval*)GC_MALLOC_ATOMIC(al);
		v->l=0;
	}

	memcpy(&v->c[v->l],c,l*sizeof(carac));
	v->l+=l;
	v->c[v->l]=0;
}

void chaineval::Finaliser(chaineval * & v)
{
	v=(chaineval*)GC_REALLOC(v,v->l*sizeof(carac)+sizeof(chaineval));
}


chaineval * chaineval::CreerFormat(const carac *c, size_t l)
{
	chaineval * v=Creer(l);
	carac * dest=v->c;
	size_t i=l;
	while (i)
	{
		if (*c==';')
		{
			c++;
			i-=2;
			switch (*(c++))
			{
			case ';':
				*(dest++)=';';
				break;
			case 'n':
				*(dest++)='\n';
				break;
			case 'r':
				*(dest++)='\r';
				break;
			case '»':
				*(dest++)='»';
				break;
			case '0':
				*(dest++)=0;
				break;
			default:
				throw TXT("caractère d'échappement inconnu après ;");
			}
		}
		else
		{
			*(dest++)=*(c++);
			i--;
		}
	}
	*dest=0;
	v->l=dest-v->c;
	return v;
}

chaineval * chaineval::CreerEntier(int i)
{
	int ndec=0;
	bool Negatif;
	if (i<0)
	{
		ndec++;
		i=-i;
		Negatif=true;
	}
	else
		Negatif=false;
	int j=i;
	do
	{
		j/=10;
		ndec++;
	}
	while (j>0);
	chaineval * v=Creer(ndec);
	v->c[ndec--]=0;
	do
	{
		v->c[ndec]='0'+(i%10);
		i/=10;
		ndec--;
	}
	while (i);
	if (Negatif)
		v->c[0]='-';
	return v;
}

chaineval * chaineval::CreerCaractere(carac c)
{
	chaineval * v=Creer(1);
	v->c[0]=c;
	v->c[1]=0;
	return v;
}

chaineval * chaineval::CreerFlottant(float f)
{
	chaineval * v=Creer(15);
	cznprintf(v->c,15,_T("%f"),f);
	v->l=czlen(v->c);
	return v;
}
