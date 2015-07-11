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
					 
Des fonctions intégrées pour gérer les tableaux.
*/
#include <gotopp/base.h>
#include "defbib.h"

using namespace GotoPP;

namespace DefBib
{
/*AIDE BOITEAPINGOUINS
Cette fonction prend tous ses paramètres
et crée un tableau qu'elle retourne.
\param element pingouin Un élément du nouveau tableau
\etc
\retour tableau Un tableau formé de l'ensemble des éléments
\exemple
J==BOITEAPINGOUINS(*(1) *(2) *(3))
GOTO J est un tableau contenant 1,2 et 3
*/
void biTab(IInterpreteur * interpreteur)
{
	CInterpreteur * itp=static_cast<CInterpreteur*>(interpreteur);
	//Crée un tableau
	valeur v;
	tableauval * t=new tableauval();
	//On redimensionne le tableau
	int l=0;
	int p=itp->Pile.Pos-1;
	while (itp->Pile.Pile[p].Type!=TYPE_DEBUTPARAM && p)
	{
		p--;
		l++;
	}
	t->Dim(l);
	p=l-1;
	do
	{
		itp->Pile.Depiler(v);
		if (v.Type==TYPE_DEBUTPARAM)
			break;
		t->Get(p--)=v;
	}
	while (1);
	itp->Pile.Ajouter(t);	
}

/*AIDE PremierPingouin
Renvoie une référence vers le premier élément d'un tableau ou d'une 
table de hachage.
\param table reference Une référence sur un tableau ou une table de hachage.
\retour reference Une référence sur le premier élément.
\desc
Cette fonction est très utile pour parcourir une table de hachage.
\exemple
J[«Nostradamus»]=«l'Archétype Humain»
J[«DieuNIAC»]=«un crétin»
J[«Superprolétaire»]=«satanique»
P=PREMIER(J)
Longueur=tailleenlongueur J
§1 GOTOPRINTDUTEXTE() ; &P# « est » &P"
P=&P++1
Longueur=-*(1)
GOTONONNULPOURLESNULS %1 &Longueur
*/
void biPremierPingouin(IInterpreteur * interpreteur)
{
	CInterpreteur * itp=static_cast<CInterpreteur*>(interpreteur);
	valeur v;
	itp->Pile.Depiler(v);
	itp->Pile.FinParametres();	

	reference r;//Parce qu'on augmente pas le compteur
	if (v.Type==TYPE_REFERENCE)
	{
		valeur *w=itp->Deref(v.v.p);
		if (w->Type==TYPE_TABLEAU)
		{
			if (w->v.t->l)
			{
				r.Type=RefType::Tableau;
				r.Base=w->v.t;
				r.Decalage=0;
			}
		}
		else if (w->Type==TYPE_HACHAGE)
		{
			if (w->v.h->l)
			{
				r.Type=RefType::Hachage;
				r.BaseH=w->v.h;
				r.Decalage=0;
			}
		}
		else
			r.Type=0;

		if (r.Type)
			itp->Pile.Ajouter(r);
		else
			itp->Pile.AjouterIndef();
	}
	else
		itp->Pile.AjouterIndef();
}


/*AIDE Pousse
Rajoute un élément en bout de troupeau.
\param tableau reference Référence sur un tableau
\param element pingouin L'élement à rajouter
\desc
Un troupeau de pingouins, ce n'est pas très accueillant. Quand on veut rajouter
un nouveau pingouin on est obligé de pousser. D'où cette fonction, très utilisée
dans le milieu fermé de l'élevage de pingouins.
\exemple
MonTroupeau=tab(«Alphonse» «Bertrand»)
Pousse(MonTroupeau «Gérard»)
GOTOPRINTDUTEXTE(Marijuana(&MonTroupeau «;n»)
*/
void biPousse(IInterpreteur * interpreteur)
{
	CInterpreteur * itp=static_cast<CInterpreteur*>(interpreteur);
	valeur truc;
	itp->Pile.Depiler(truc);
	valeur tableau;
	itp->Pile.Depiler(tableau);
	itp->Pile.FinParametres();
	if (tableau.Type!=TYPE_REFERENCE)
		throw TXT("passez une référence");
	valeur * v=itp->Deref(tableau.v.p);
	v->EnTableau();
	v->v.t->Get(v->v.t->l)=truc;
	itp->Pile.AjouterIndef();
}

/*AIDE Plop
Enlève un élément en bout de troupeau.
\param tableau reference Référence sur un tableau
\retour pingouin L'élément qu'on a retiré
\desc
Parfois, un pingouin s'intègre mal à son nouveau troupeau. La peine
dans le c&oelig;ur, on est donc obligé de l'envoyer à la boucherie.
Cette fonction sert à ça, Plop étant une honomatopée rendant assez bien
le bruit du pingouin extrait de son troupeau.
*/

void biPlop(IInterpreteur * interpreteur)
{
	CInterpreteur * itp=static_cast<CInterpreteur*>(interpreteur);
	valeur tableau;
	itp->Pile.Depiler(tableau);
	itp->Pile.FinParametres();
	if (tableau.Type!=TYPE_REFERENCE)
		throw TXT("passez une référence");
	valeur * v=itp->Deref(tableau.v.p);
	v->EnTableau();

	if (v->v.t->l)
		itp->Pile.Ajouter(v->v.t->Get(v->v.t->l-1));
	else
		itp->Pile.AjouterIndef();
	
	if (v->v.t->l)
		v->v.t->Dim(v->v.t->l-1);
}

/*AIDE MetsDessous
Ajoute un élément en début de tableau.
\param tableau reference Référence sur un tableau
\param element pingouin L'élément à ajouter
*/
void biMetsDessous(IInterpreteur * interpreteur)
{
	CInterpreteur * itp=static_cast<CInterpreteur*>(interpreteur);
	valeur truc;
	itp->Pile.Depiler(truc);
	valeur tableau;
	itp->Pile.Depiler(tableau);
	itp->Pile.FinParametres();
	if (tableau.Type!=TYPE_REFERENCE)
		throw TXT("passez une référence");
	valeur * v=itp->Deref(tableau.v.p);
	v->EnTableau();
	
	index_t al=v->v.t->l;
	v->v.t->Dim(al+1);
	if (al)
		memmove(&v->v.t->v[1],v->v.t->v,
			al*sizeof(valeur));
	v->v.t->Get(0)=truc;
	itp->Pile.AjouterIndef();
}

/*AIDE EnleveDessous
Enlève le premier élément d'un tableau. Y'a pas à dire, c'est un nom plus explicite que les
shift ou autre qu'on trouve dans les langages moins bien.
\param tableau reference Référence sur un tableau
\retour element pingouin L'élément qui a été retiré
*/
void biEnleveDessous(IInterpreteur * interpreteur)
{
	CInterpreteur * itp=static_cast<CInterpreteur*>(interpreteur);
	valeur tableau;
	itp->Pile.Depiler(tableau);
	itp->Pile.FinParametres();
	if (tableau.Type!=TYPE_REFERENCE)
		throw TXT("passez une référence");
	valeur * v=itp->Deref(tableau.v.p);
	v->EnTableau();

	if (v->v.t->l)
		itp->Pile.Ajouter(v->v.t->Get(0));
	else
		itp->Pile.AjouterIndef();
	
	if (v->v.t->l>1)
		memmove(v->v.t->v,&v->v.t->v[1],
			(v->v.t->l-1)*sizeof(valeur));
	if (v->v.t->l)
		v->v.t->Dim(v->v.t->l-1);
}

/*AIDE EnleveMilieu
Enlève un élément au milieu du tableau.
\param tableau reference Référence sur un tableau
\param indice entier Numéro de l'élément du tableau à retirer (à partir de 0)
\retour element pingouin L'élement qui a été supprimé
*/
void biEnleveMilieu(IInterpreteur * interpreteur)
{
	CInterpreteur * itp=static_cast<CInterpreteur*>(interpreteur);
	valeur indice;
	itp->Pile.Depiler(indice);
	int i=indice.Entier();
	if (i<0)
		throw TXT("indice négatif");
	valeur tableau;
	itp->Pile.Depiler(tableau);
	itp->Pile.FinParametres();
	if (tableau.Type!=TYPE_REFERENCE)
		throw TXT("passez une référence");
	valeur * v=itp->Deref(tableau.v.p);
	v->EnTableau();

	if (i<(int)v->v.t->l)
	{
		itp->Pile.Ajouter(v->v.t->Get(i));
		if (i<(int)v->v.t->l-1)
			memmove(&v->v.t->v[i],&v->v.t->v[i+1],
				(v->v.t->l-i-1)*sizeof(valeur));
		v->v.t->Dim(v->v.t->l-1);
	}
	else
		itp->Pile.AjouterIndef();
}

}
