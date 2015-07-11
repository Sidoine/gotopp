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
					 
Trucs de hasard.
*/
#include <gotopp/base.h>
#include <gotopp/programme.h>

#include "defbib.h"

using namespace GotoPP;

namespace DefBib
{

int biTableauPoids(ITableauValeurs * _poids)
{
	tableauval * poids=static_cast<tableauval*>(_poids);
	//Lit un tableau contenant des poids
	//et choisi un entier au hasard
	//en fonction du poids correspondant
	valeur * v=poids->v;
	index_t n=poids->l;
	float total=0;
	for (index_t i=0; i<n; i++)
	{
		total+=v->Flottant();
		v++;
	}
	float h=rand()*total/(RAND_MAX+1.0f);
	float t2=0;
	v=poids->v;
	index_t j=0;
	for (j=0; j<n; j++)
	{
		t2+=v->Flottant();
		if (t2>=h)
			break;
		v++;
	}
	return j;
}

/*AIDE GOTOCHEPA
Se rend à une étiquette choisie aléatoirement dans une liste, avec
la possibilité de revenir avec un REGOTO.
\param cible pingouin Cible où se rendre ou chances en % que la cible précédente soit choisie
\etc
\retour pingouin La valeur renvoyée par le REGOTO.
\desc
Cette commande est très semblable à GOTOGOTO, la seule différence
est qu'il est possible de revenir à l'instruction qui la suit
à l'aide d'un REGOTO.

Il est possible de récupérer la valeur renvoyée par REGOTO.
\exemple
GOTOCHEPA() %1
GOTOPRINTDUTEXTE() «Ce texte est toujours affiché.»
GOTOFIN
§1
GOTOPRINTDUTEXTE() «Ce texte a 90% de chances d'être affiché.»
REGOTO
*/
void biGotoChepa(IInterpreteur * interpreteur)
{
	biGotoMulti(static_cast<CInterpreteur*>(interpreteur),true);
}

/*AIDE GOTOGOTO
Se rend à une étiquette choisie aléatoirement dans une liste.
\param cible pingouin Cible où se rendre ou probabilité
\etc
\desc
Le programme se branchera sur une des cibles. Un nombre aléatoire
entre 0 et 100 est tiré et, en fonction de celui-ci, une des cibles est choisie.
En effet, on peut assigner un nombre à virgule flottante (ou « chance ») à chacune
des cibles qui donnera la probabilité en pourcents d'aller à cette cible.

Si la somme de tous les pourcentages est supérieure à 100,
les pourcentages sont diminués d'autant que nécessaire pour ramener la somme à 100,
en commençant par la première cible de la liste des paramètres.

Si la somme des pourcentages est inférieure à 100, la probabilité restante
correspond à celle de continuer l'exécution du programme à la ligne suivante.

Le paramètre £cible£ peut être un entier, auquel cas la cible
doit être une étiquette globale, ou une étiquette, qui peut
être globale ou locale. Ce paramètre peut aussi être un nombre à virgule flottante,
ce qui permet de définir le pourcentage de chance d'aller à la cible qui précède. Si
la cible n'est pas suivie d'une probabilité, la valeur de 90 est prise
par défaut.

\exemple
GOTO Dans l'exemple suivant il y a 90% de chances d'aller en 2
GOTO et 10% de chances de continuer
GOTOGOTO() %2
GOTOPRINTDUTEXTE() ; «10% de chances.»
GOTOFIN
§2 GOTOPRINTDUTEXTE() ; «90% de chances.»

GOTO Dans cet exemple il y a 50% de chances d'aller en 2,
GOTO 30% de chances d'aller en 3 et 20% de chances d'aller
GOTO en 4
GOTOGOTO() %2 %3 ?30 *(4) ?20
*/
void biGotoGoto(IInterpreteur * interpreteur)
{
	biGotoMulti(static_cast<CInterpreteur*>(interpreteur),false);
}

void biGotoMulti(CInterpreteur * itp, bool Sub)//, int nParams)
{
	float r=float(rand())/(RAND_MAX+1.0f);
	while (!itp->Pile.Vide())
	{
		float pourc=0.90f;
		valeur var;itp->Pile.Depiler(var);
		if (var.Type==TYPE_DEBUTPARAM)
			break;
		if (var.Type==TYPE_CHANCES)
		{
			pourc=var.v.f/100;
			if (pourc<0)
				pourc=0;
			itp->Pile.Depiler(var);
		}
		if ((var.Type!=TYPE_ENTIER)&&(var.Type!=TYPE_ETIQUETTE))
			throw "entier ou étiquette attendu pour cible d'un GOTO";

		code * cible;
		if (r<pourc)
		{
			while (itp->Pile.Dernier().Type!=TYPE_DEBUTPARAM)
				itp->Pile.EffacerDernier();

			if (var.Type==TYPE_ENTIER)
			{
				cible=itp->programme->ChercherEtiquette(var.v.i);
			}
			else
				cible=var.v.et;

			if (Sub)
				itp->AppelerEtiquette(cible);
			else
				itp->Goto(cible);
			break;
		}
		else
			r-=pourc;
	}
	itp->Pile.AjouterIndef();
}

}
