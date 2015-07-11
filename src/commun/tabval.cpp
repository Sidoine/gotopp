/*

Interpréteur GOTO++
Copyright 2002 Sidoine de Wispelaere

Ce programme est un logiciel libre ; vous pouvez le redistribuer
et/ou le modifier sous les termes de la GNU General Public Licence
telle que publiée par la Free Software Fundation ; soit dans la
version 2 de la Licence, soit (à votre choix) dans une version
ultérieure.

Ce programme est distribué dans l'espoir qu'il sera utile mais
SANS AUCUNE GARANTIE ; sans même la garantie implicite qu'il soit
COMMERCIALISABLE ou ADEQUAT POUR UN USAGE PARTICULIER. Voir la
GNU General Public Licence pour plus de détails.

Vous devriez avoir reçu une copie de la GNU General Public Licence
avec ce programme ; dans le cas contraire, écrivez à :
Free Software Fundation, Inc.
59 Temple Place - Suite 330, Boston, MA  02111-1307, US

                          ****
Voir tabval.h
*/

#include <gotopp/base.h>

using namespace std;
using namespace GotoPP;

tableauval * tableauval::Copier()
{
	tableauval * ret=new tableauval();
	
	ret->l=l;
	ret->v=(valeur*)GC_MALLOC(l*sizeof(valeur));
	valeur * dest=ret->v;
	valeur * src=v;
	for (index_t a=0; a<l; a++)
		dest[a]=src[a];
	ret->Classe=Classe;
	ret->Dimensions=Dimensions;	
	return ret;
}

void tableauval::Dim(index_t t)
{
	if (t)
	{
		v=(valeur*)GC_REALLOC(v,t*sizeof(valeur));
		for (index_t i=l; i<t; i++)
			new (v+i) valeur();
	}
	else
	{
		v=0;
	}
	l=t;
}

//Uniquement pour obtenir une valeur
valeur & tableauval::Get(int i)
{
	if (i<0)
		throw TXT("Accès à un indice négatif du tableau");
	return Get((index_t)i);
}

valeur * tableauval::ObtPtr(int i)
{
	if (!v || i<0 || i>int(l))
		return 0;
	return v+i;
}

valeur * tableauval::ObtPtr(index_t i)
{
	if (i>l)
		return 0;
	return v+i;
}

valeur & tableauval::Get(index_t i)
{
	if (i>=int(l))
	{
		v=(valeur*)GC_REALLOC(v,(i+1)*sizeof(valeur));
		for (index_t j=l; j<i+1; j++)
			new (v+j) valeur();
		l=i+1;
	}
	return v[i];
}

void tableauval::ObtRef(reference & r, int i)
{
	if (i<0)
		throw TXT("Accès à un indice négatif du tableau");
	if (i>=int(l))
	{
		v=(valeur*)GC_REALLOC(v,(i+1)*sizeof(valeur));
		for (index_t j=l; j<(index_t)i+1; j++)
			new (v+j) valeur();
		l=i+1;
	}
	r.Type=RefType::Tableau;
	r.Base=this;
	r.Decalage=i;
}


