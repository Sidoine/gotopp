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
Voir hachval.h					 
*/

#include <gotopp/base.h>
#include <gotopp/hachage.h>

using namespace GotoPP;
		
hachageval * hachageval::Copier()
{
	hachageval * copie=new hachageval();

	copie->a=copie->l=l;
	copie->v=(pairehachage*)GC_MALLOC(sizeof(pairehachage)*l);
	memcpy(copie->v,v,sizeof(pairehachage)*l);
	copie->Index=(indexhachage*)GC_MALLOC(sizeof(indexhachage)*l);
	memcpy(copie->Index,Index,sizeof(indexhachage)*l);
	return copie;
}

int hachageval::ObtId(const chaineval * clef)
{
	if (l==0)
		return -1;
	
	int hache=CalculerHache((char*)clef->c,clef->l*sizeof(carac));
	
	int inf=-1;
	int sup=int(l);
	indexhachage * idx=Index;
	pairehachage * paire=v;
	
	while (inf<sup-1)
	{
		int mil=(inf+sup)/2;
		int h=idx[mil].hache;
		int ret=(hache<h?-1:(hache>h?1:0));
		if (ret==0)
			ret=czcmp(clef->c,paire[idx[mil].i].Clef->c);
		if (ret<0)
			sup=mil;
		else if (ret>0)
			inf=mil;
		else
			return idx[mil].i;
	}
	return -sup-1;//Où est-ce qu'il faut insérer
}

valeur * hachageval::Get(const chaineval * clef)
{
	int ret=ObtId(clef);
	if (ret<0)
		return NULL;
/*	cout << "Trouvé : "<< ret <<" "<< ((pairehachage*)v->v)[ret].Clef <<"=";
	cout << ((pairehachage*)v->v)[ret].Val.v.i << "\n";*/
	return &v[ret].Val;
}

void hachageval::ObtRef(reference & r, const chaineval * clef)
{
	int ret=ObtId(clef);
	if (ret<0)
	{
		int OuInserer=-ret-1;
		if (l==a)
		{
			a+=128;
			v=(pairehachage*)GC_REALLOC(v,a*sizeof(pairehachage));
			Index=(indexhachage*)GC_REALLOC(Index,a*sizeof(indexhachage));
		}
		if (OuInserer!=(int)l)
		{
			memmove(&Index[OuInserer+1],&Index[OuInserer],
				(l-OuInserer)*sizeof(indexhachage));
		}
		Index[OuInserer].i=uint(l);
		Index[OuInserer].hache=CalculerHache((char*)clef->c,clef->l*sizeof(carac));
		v[l].Clef=clef;
		v[l].Val.Type=TYPE_INCONNU;
		ret=int(l++);
	}
	r.Type=RefType::Hachage;
	r.BaseH=this;
	r.Decalage=ret;
}

void hachageval::Dim(index_t t)
{
	if (a<t)
	{
		a=t;
		v=(pairehachage*)GC_REALLOC(v,t*sizeof(pairehachage));
		Index=(indexhachage*)GC_REALLOC(Index,t*sizeof(indexhachage));
	}
}
