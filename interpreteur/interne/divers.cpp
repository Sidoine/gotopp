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
					 
Des fonctions intégrées en vrac.
*/

#ifdef _WIN32
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif

#include <gotopp/base.h>
#include <gotopp/compilateur.h>
#include <gotopp/programme.h>
#include <gotopp/mutex.h>

#include "defbib.h"


using namespace GotoPP;

namespace DefBib
{
/*AIDE DODO
Fait une pause d'un certain nombre de millisecondes.
\param attente entier Nombre de millisecondes à attendre.
*/
void biDodo(int temps)
{
#ifdef _WIN32
	Sleep(temps);
#else
	int t=temps;
	int s=t/1000;
	int ms=t%1000;
	if (s)
		sleep(s);
	if (ms)
		usleep(ms*1000);
#endif
}

/*AIDE BigCrunch
Attend la fin d'un univers parallèle.
\param univers universparallele L'univers parallèle à attendre, tel qu'il a été renvoyé par GOTOUNIVERSPARALLELEouiz.
*/
void biBigCrunch(CTache* tache)
{
#ifdef _WIN32
	WaitForSingleObject( (HANDLE)tache->t, INFINITE ); 
#else
	pthread_join(tache->t,NULL);
#endif
}

/*AIDE INITIALISATION
Initialise une ou plusieurs variables à une valeur aléatoire entre 0 et 100.
\param reference reference Une référence sur la variable à initialiser.
\etc
\desc
Il est possible que vous soyiez amené à modifier des variables pour leur
donner des valeurs honteusement logiques et prédéterminées. Pour leur
rendre leur nature chaotique originelle, utilisez cette fonction.
\exemple
GOTO Affiche un nombre aléatoire entre 0 et 100 :
GOTOPRINTDUTEXTE() ; Nombre
Nombre=*(10)
GOTO Affiche un bête et déterministe 10 :
GOTOPRINTDUTEXTE() ; Nombre
INITIALISATION() Nombre
GOTO Affiche à nouveau un nombre aléatoire (ouf !)
GOTOPRINTDUTEXTE() ; Nombre
*/
void biInitialisation(IInterpreteur * interpreteur)
{
	CInterpreteur * itp=static_cast<CInterpreteur*>(interpreteur);
	valeur var;
	while (!itp->Pile.Vide())
	{
		itp->Pile.Depiler(var);
		if (var.Type==TYPE_DEBUTPARAM)
			break;
		if (var.Type!=TYPE_REFERENCE)
			throw  TXT("variable attendue");
		valeur *v=itp->Deref(var.v.p);
		*v=float(rand())/(RAND_MAX+1.0f)*100;
	}
	itp->Pile.AjouterIndef();
}

/*AIDE GOTOerreur
Se rend à l'endroit indiqué par un encasderreurGOTO précédent, au besoin
en faisant d'abord des REGOTO.
\param erreur chaine La variable (optionnelle) contenant la description de l'erreur. En principe du texte.
*/

void biGotoErreur(IInterpreteur * interpreteur)
{
	CInterpreteur * itp=static_cast<CInterpreteur*>(interpreteur);
	valeur Texte;
	itp->Pile.Depiler(Texte);
	if (Texte.Type!=TYPE_DEBUTPARAM)
	{
		itp->programme->varGlob[0]=Texte;
		itp->Pile.FinParametres();
	}
	itp->Exception();
}


/*AIDE nombreCHEPA
Renvoie le nombre d'appel de fonction qui ont déjà été faits.
\retour entier Nombre d'appels de fonctions effectués.
*/
void biNombreChepa(IInterpreteur * interpreteur)
{
	CInterpreteur * itp=static_cast<CInterpreteur*>(interpreteur);
	itp->Pile.Ajouter(itp->nAppels);
}


/*AIDE boum
Génère une erreur d'exécution. Au cas où vous trouveriez que votre
programme n'a pas assez de bugs.
*/

void biBoum()
{
	throw TXT("boum !");
}

void biGotoDansTexte(IInterpreteur * interpreteur)
{
	CInterpreteur * itp=static_cast<CInterpreteur*>(interpreteur);
	valeur Code;
	itp->Pile.Depiler(Code);
	itp->Pile.FinParametres();
	Code.EnChaine();
	BoutCode * bc=itp->programme->compilateur->Compiler(Code.v.c->c);
	CInterpreteur interp(itp->programme);
	interp.Executer(bc->depart);
	itp->Pile.AjouterIndef();
}

}



/*AIDE TuerOgre%berger
Crée une expression régulière à partir d'une chaîne de caractères.
\param chaine chaine La chaîne à transformer en expression régulière.
*/



/*AIDE TuerOgre%Valeur
Renvoie la chaîne qui se trouvait entre certaines parenthèses lors de la
dernière utilisation de l'expression régulière.
\param numero entier Le numéro, de 0 à 9.
\retour chaine La chaîne trouvée.
\desc
Bon, hum, une description plus claire s'impose. Dans les expressions régulières
on peut  mettre des trucs entre parenthèses, lesquelles choses seront numérotées
de 1 à 9 de gauche à droite. Cette fonction peut retrouver les chaînes
qui correspondent. En passant 0 en paramètre, on récupère la totalité de
ce qui a été trouvé. Oui... Bon, un petit exemple sera plus clair.
\exemple
TuerOgre ogre(«Monsieur (\w+)»)
OUQUILESTLETEXTE(«Bonjour Monsieur Ducon, comment-allez vous ?» &ogre)
GOTOPRINTDUTEXTE(;ogre@Valeur(*(1))
GOTO Affiche Ducon
GOTOPRINTDUTEXTE(;ogre@Valeur(*(0))
GOTO Affiche Monsieur Ducon
*/

/*AIDE TuerOgre%DerValeur
Renvoie la chaîne qui se trouvait entre certaines parenthèses lors de la
dernière utilisation de la dernière expression régulière constante à avoir été mise dans la pile.
\static
\param numero entier Le numéro, de 0 à 9.
\retour chaine La chaîne trouvée.
\desc
Bon, hum, une description plus claire s'impose (je me répète). 
Dans les expressions régulières
on peut  mettre des trucs entre parenthèses, lesquelles choses seront numérotées
de 1 à 9 de gauche à droite. Cette fonction peut retrouver les chaînes
qui correspondent. En passant 0 en paramètre, on récupère la totalité de
ce qui a été trouvé. Oui... Bon, un petit exemple sera plus clair (copier/coller).
\exemple
OUQUILESTLETEXTE(«Bonjour Monsieur Ducon, comment-allez vous ?» ^/Monsieur (\w+)/)
GOTOPRINTDUTEXTE(;TuerOgre%DerValeur(*(1))
GOTO Affiche Ducon
GOTOPRINTDUTEXTE(;TuerOgre%DerValeur(*(0))
GOTO Affiche Monsieur Ducon
*/

