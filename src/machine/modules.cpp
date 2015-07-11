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

Les interfaces permettant aux modules d'accéder au compilateur et
à l'interpréteur. Je crois aussi que ça sert pour définir les fonctions
intégrées.
*/
#include <gotopp/base.h>
#include <gotopp/machine.h>
#include <gotopp/programme.h>
#include <gotopp/mutex.h>

using namespace GotoPP;

void CInterpreteur::Exception(carac *Texte)
{		
	programme->mutex.Prendre();
	if (Texte)
	{
		varGlob[0].Type=TYPE_CHAINE;
		varGlob[0].v.c=chaineval::Creer(Texte);
	}
	else
		varGlob[0].Effacer();
	Exception();
	programme->mutex.Liberer();
}

void CInterpreteur::DebutParametres()
{
	Pile.AjouterDebutParam();
}

void CInterpreteur::AppelerFonction(code* f)
{
	AppelerEtiquette(f);
	Executer();
}
	
const chaineval * CInterpreteur::LireChaine()
{
	valeur Chaine;
	Pile.Depiler(Chaine);
	Chaine.EnChaine();
	return Chaine.v.c;
}

IChaineA * CInterpreteur::LireChaineA()
{
	//TODO vérifier ça
	valeur Chaine;
	Pile.Depiler(Chaine);
	Chaine.EnChaine();
#ifdef _UNICODE
	IChaineA * ret=(IChaineA*)GC_MALLOC_ATOMIC(Chaine.v.c->l+sizeof(size_t));
	ret->l=Chaine.v.c->l;
	sprintf(ret->c,"%S",Chaine.v.c->c);
	return ret;
#else
	return (IChaineA*)Chaine.v.c;
#endif
}

int CInterpreteur::LireEntier()
{
	valeur Entier;
	Pile.Depiler(Entier);
	return Entier.Entier();
}

float CInterpreteur::LireFlottant()
{
	valeur Chances;
	Pile.Depiler(Chances);
	return Chances.Flottant();
}

void * CInterpreteur::LirePointeur()
{
	valeur Pointeur;
	Pile.Depiler(Pointeur);
	if (Pointeur.Type!=TYPE_REFERENCE)
		throw TXT("ce n'est pas une référence");
	return Deref(Pointeur.v.p);
}

IObjet * CInterpreteur::LireObjet()
{
	valeur objet;
	Pile.Depiler(objet);
	if (objet.Type!=TYPE_OBJETC)
		throw TXT("ce paramètre n'est pas un objet");
	return objet.v.o;
}

regexp * CInterpreteur::LireRegExp()
{
	valeur objet;
	Pile.Depiler(objet);
	if (objet.Type!=TYPE_OBJETC) //TODO: vérification du type d'objet RTTI
		throw TXT("ce paramètre n'est pas un objet");
	return (regexp*)objet.v.o;
}

IValeur * CInterpreteur::LireValeur()
{
	valeur objet;
	Pile.Depiler(objet);
	return objet.Copier();
}

entier64 CInterpreteur::LireEntier64()
{
	valeur objet;
	Pile.Depiler(objet);
	return objet.Entier64();
}

int CInterpreteur::GetNombreParametres()
{
	return Pile.nParams();
}
		
ITableauValeurs * CInterpreteur::LireTableauValeurs()
{
	valeur objet;
	Pile.Depiler(objet);
	if (objet.Type!=TYPE_TABLEAU)
		throw TXT("ce n'est pas un tableau");
	return objet.v.t;
}

void * CInterpreteur::LireVariable(int v)
{
	return &varGlob[v];
}

void CInterpreteur::FinParametres()
{
	Pile.FinParametres();
	programme->mutex.Liberer();
}

void CInterpreteur::RetournerChaine(carac * c)
{
	chaineval * chaine=chaineval::Creer(c);
	Pile.Ajouter(chaine);
	programme->mutex.Prendre();
}

void CInterpreteur::RetournerChaine(const IChaine * chaine)
{
	Pile.Ajouter((const chaineval*)chaine);
	programme->mutex.Prendre();
}

void CInterpreteur::RetournerChaineA(char * c)
{
	chaineval * chaine=chaineval::Creer(c);
	Pile.Ajouter(chaine);
	programme->mutex.Prendre();
}

void CInterpreteur::RetournerCarac(carac c)
{
	Pile.Ajouter(c);
	programme->mutex.Prendre();
}

void CInterpreteur::RetournerTableauValeurs(ITableauValeurs * tableau)
{
	Pile.Ajouter((tableauval*)tableau);
	programme->mutex.Prendre();
}

void CInterpreteur::RetournerEntier64(entier64 i)
{
	Pile.Ajouter(i);
	programme->mutex.Prendre();
}

void CInterpreteur::RetournerObjet(IObjet * o)
{
	Pile.Ajouter(o);
	programme->mutex.Prendre();
}

void CInterpreteur::RetournerEntier(int i)
{
	Pile.Ajouter(i);
	programme->mutex.Prendre();
}

void CInterpreteur::RetournerFlottant(float f)
{
	Pile.Ajouter(f);
	programme->mutex.Prendre();
}

void CInterpreteur::RetournerIndefini()
{
	Pile.AjouterIndef();
	programme->mutex.Prendre();
}

void CInterpreteur::Modifier(void *p, IObjet *o)
{
	programme->mutex.Prendre();
	valeur * v=(valeur*)p;
	v->Type=TYPE_OBJETC;
	v->v.o=o;
	programme->mutex.Liberer();
}

void CInterpreteur::Modifier(void *p, int i)
{
	programme->mutex.Prendre();
	*((valeur *)p)=i;
	programme->mutex.Liberer();
}

void CInterpreteur::Modifier(void * p, carac * c)
{
	programme->mutex.Prendre();
	valeur * v=(valeur*)p;
	v->Effacer();
	v->Type=TYPE_CHAINE;
	v->v.c=chaineval::Creer(c);
	programme->mutex.Liberer();
}

void CInterpreteur::Modifier(void * p, carac * c, int l)
{
	programme->mutex.Prendre();
	valeur * v=(valeur*)p;
	v->Effacer();
	v->Type=TYPE_CHAINE;
	v->v.c=chaineval::Creer(c,l);
	programme->mutex.Liberer();
}

#ifdef _UNICODE
void CInterpreteur::Modifier(void * p, char * c, int l)
{
	programme->mutex.Prendre();
	valeur * v=(valeur*)p;
	v->Effacer();
	v->Type=TYPE_CHAINE;
	v->v.c=chaineval::Creer(c,l);
	programme->mutex.Liberer();
}
#endif

void CInterpreteur::Ajouter(void *p, const chaineval *clef, const chaineval *val, size_t lval)
{
	programme->mutex.Prendre();
	valeur * v=(valeur*)p;
	v->EnTableHachage();
	areference r;
	v->v.h->ObtRef(r,clef);
	valeur *w=Deref(r);
	*w=val;
	programme->mutex.Liberer();
}
