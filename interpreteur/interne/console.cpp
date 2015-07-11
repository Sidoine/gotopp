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
					 
Fonctions intégrées pour l'accès à la console (ou au terminal texte,
si vous préférez cette dénomination).
A FAIRE : rajouter la couleur pour curses
*/

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <conio.h>
#include <ctype.h>
#include <fcntl.h> //pour _O_BINARY

HANDLE sortie;
#else
#include <curses.h>
#include <signal.h>
bool CursesInit=false;
int NumeroPaire[256];
#endif
#include <iostream>
#include <gotopp/base.h>
#include "defbib.h"
#include <gotopp/mutex.h>
#include <gotopp/programme.h>


using namespace GotoPP;
using namespace std;

namespace DefBib
{

#ifdef _WIN32
unsigned int ancCP;
unsigned int ancOutputCP;
#endif

void InitConsole()
{
#ifdef _WIN32
	ancCP=GetConsoleCP();
	ancOutputCP=GetConsoleOutputCP();
	SetConsoleCP(1252);
	SetConsoleOutputCP(1252); 
	sortie=GetStdHandle(STD_OUTPUT_HANDLE);
#endif
}

void RestaurerConsole()
{
#ifndef _WIN32
	if (CursesInit)
	{
		CursesInit=false;
		endwin();
	}
#else
	CONSOLE_CURSOR_INFO cci;
	cci.bVisible=TRUE;
	cci.dwSize=1;
	SetConsoleCursorInfo(sortie,&cci);
	SetConsoleCP(ancCP);
	SetConsoleOutputCP(ancCP);
#endif
}

#ifndef _WIN32

void resizeHandler(int sig)
{
}

void InitCurses()
{
	initscr();
	keypad(stdscr, TRUE);
	nonl();
	intrflush(stdscr,FALSE);
	cbreak();
	noecho();
	scrollok(stdscr,TRUE);
	notimeout(stdscr,FALSE);
	CursesInit=true;
	signal(SIGWINCH, resizeHandler);
	//start_color();

/*	if ((COLOR_PAIRS>=256)&&(COLORS>=16))
	{
		can_change_color()
	}
	else
	{
		printf("Pas assez de couleurs paires=%d couleurs=%d\n",
				COLOR_PAIRS,COLORS);
	}*/
/*	int IndexCouleur[16];
	//Création des couleurs
	if (can_change_color() && COLORS>=16)
	{
		for (int i=0; i<16; i++)
		{
			IndexCouleur[i]=i;
			int r,v,b;
			int val=(i & 8)?1000:500;
				
			r=(i & 1)?val:0;
			v=(i & 2)?val:0;
			b=(i & 4)?val:0;
			init_color(i,r,v,b);
		}
	}
	else
	{
		for (int i=0; i<16; i++)
		{
			int r,v,b;
			int val=(i & 8)?1000:500;
				
			r=(i & 1)?val:0;
			v=(i & 2)?val:0;
			b=(i & 4)?val:0;
			int dif=6000;int difv=0;
			for (int j=0; j<COLORS; j++)
			{
				int ir,iv,ib;
				color_content(j,&ir,&iv,&ib);
				di=abs(ir-r)+abs(iv-v)+abs(ib-b);
				if (di<dif)
				{
					dif=di;
					difv=j;
				}
			}
			IndexCouleur[i]=difv;
		}
	}
	//Création des paires
	for (int f=0; f<COLORS; f++)
	{
		for (int b=0; b<COLORS; b++)
		{
			if (f || b)
			{
				init_pair(f*COLORS+b,COLORS-f-1,b);
			}
		}
	}*/
}
#endif

const chaineval * biEntreTonTexteFurtif()
{
	
	char Temp[128];
#ifdef _WIN32
	if (!gets(Temp))
		*Temp=0;
#else
	if (!CursesInit)
		InitCurses();
	getnstr(Temp,127);
#endif
	size_t l=strlen(Temp);
	if (Temp[l-1]=='\n')
		Temp[l-1]=0;
	if (Temp[l-2]=='\r')
		Temp[l-2]=0;
	return chaineval::Creer(Temp);
}

/*AIDE ENTRETONTEXTE
Lit une ligne de texte dans l'entrée standard (le clavier, normalement)
\retour chaine La ligne lue.
\desc Cette fonction permet de sonder les pensées de l'utilisateur en lui faisant
entrer du texte. Cela permet également de lui demander son nom, ce qui est
bien utile dans les exemples.
\exemple
GOTOPRINTDUTEXTE() «Entre ton nom, sympathique gourou : »
NomDuSympathiqueGourou=ENTRETONTEXTE()
GOTOPRINTDUTEXTE() ; &NomDuSympathiqueGourou «Bonjour »
*/
const GotoPP::chaineval * biEntreTonTexte()
{
	char Temp[128];
#ifdef _WIN32
	if (!fgets(Temp,127,stdin))
		*Temp=0;
#else
	if (CursesInit)
	{
		echo();
		getnstr(Temp,127);
		noecho();
	}
	else
		fgets(Temp,127,stdin);
#endif
	size_t l=strlen(Temp);
	if (Temp[l-1]=='\n')
	Temp[l-1]=0;
	if (Temp[l-2]=='\r')
	Temp[l-2]=0;
	
	return chaineval::Creer(Temp);
}

/*AIDE ENTREUNNOMBRE
Lit un nombre sur l'entrée standard (d'habitude, c'est un clavier).
\retour entier Le nombre lu.
$LONGDESC
Permet par exemple de demander combien de pingouins ont été vus.
$EXEMPLE
GOTO On lit le nombre de pingouins dans un fichier
GOTOPRINTDUTEXTE() ; «Combien de pingouins ont été vus par l'expédition scientifique ?»
NbPingouins=ENTREUNNOMBRE()
GOTOPRINTDUTEXTE() ; « pingouin(s) ont été vus.» &NbPingouins «Le rapport indique que »
*/
int biEntreUnNombre()
{
	char Temp[11];
#ifdef _WIN32
	fgets(Temp,10,stdin);
#else
	if (CursesInit)
	{
		echo();
		getnstr(Temp,10);
		noecho();
	}
	else
		fgets(Temp,10,stdin);
#endif
	return atoi(Temp);
}

/*AIDE TapeUneTouche
Attend qu'une touche soit frappée et renvoie sa valeur.
\retour touche entier La touche qui a été enfoncée.
\desc Les touches fléchées renvoient des valeurs spéciales : &TOUCHEBAS pour le bas,
&TOUCHEHAUT pour le haut, &TOUCHEGAUCHE pour la gauche et &TOUCHEDROITE pour la droite.
Ces valeurs sont des constantes qui varient (bon, ok, des quasi-constantes) d'un système à l'autre.
*/
int biTapeUneTouche()
{
	int c;
#ifdef _WIN32
	c=_getch();
	if (c==0 || c==0xE0)
		c=-_getch();
#else
	if (!CursesInit)
		InitCurses();
	c=getch();
#endif
	return c;
}

/*AIDE GOTOPRINT
Affiche la valeur de la dernière variable modifiée par un =.
\desc Cette fonction permet d'afficher la valeur de la dernière variable qui a été modifiée
par une affectation à l'aide de l'opérateur =. Un retour à la ligne
est ajouté à la fin.
\exemple
J=*(3)
GOTOPRINT() GOTO Affiche 3
*/
void biGotoPrint(IInterpreteur * interpreteur)
{
	CInterpreteur * itp=static_cast<CInterpreteur*>(interpreteur);
	if (itp->DerVar.Type==TYPE_REFERENCE)
	{
		itp->Deref(itp->DerVar.v.p)->Ecrire(stdout);
		czout << '\n';
	}
	else
		czout << "0\n";
	itp->Pile.AjouterIndef();
}

/*AIDE GOTOPRINTDUTEXTE
Affiche tous ses paramètres à l'écran.
\param texte chaine Texte à afficher
\etc
\desc Tous les paramètres sont affichés les uns à la suite des autres. Contrairement
à GOTOPRINT, aucun retour à la ligne n'est ajouté à la fin.

Remarquez que les paramètres sont affichés en commençant par le dernier.
\exemple
GOTOPRINTDUTEXTE() ; *(15) «3+9=»
GOTO Affiche 3+9=15 suivis d'un retour à la ligne.
*/
void biGotoPrintDuTexte(IInterpreteur * interpreteur)
{
	CInterpreteur * itp=static_cast<CInterpreteur*>(interpreteur);
	while (!itp->Pile.Vide())
	{
		valeur v;
		itp->Pile.Depiler(v);
		if (v.Type==TYPE_DEBUTPARAM)
			break;
#ifdef _WIN32
		v.Ecrire(stdout);
#else
		if (CursesInit)
		{
			switch(v.Type)
			{
			case TYPE_ENTIER:
				printw("%d",v.v.i);
				break;
			case TYPE_CHANCES:
				printw("%f",v.v.f);
				break;
			case TYPE_CARAC:
				printw("%c",v.v.i);
				break;
			case TYPE_CHAINE:
				{
#ifdef _UNICODE
					char * tmp=allocmbs(v.v.c->c,v.v.c->l);
					addstr(tmp);
					free(tmp);
#else
					addstr(v.v.c->c);
#endif
				}
				break;
			default:
				printw("<?:%d>",v.Type);
			}
			refresh();
		}
		else
			v.Ecrire(stdout);
#endif
	}
	itp->Pile.AjouterIndef();
}


/*AIDE MontrerCurseur
Affiche ou cache le curseur texte.
\param afficher entier 0 pour cacher le curseur, 1 pour le montrer
*/
void biMontrerCurseur(int montrer)
{
#ifdef _WIN32
	CONSOLE_CURSOR_INFO cci;
	cci.bVisible=montrer?TRUE:FALSE;
	cci.dwSize=1;
	SetConsoleCursorInfo(sortie,&cci);
#else
	if (!CursesInit)
		InitCurses();
	
	curs_set(montrer?1:0);
#endif		
}

/*AIDE gotoXY
Déplace le curseur à un point de l'écran.
\param x entier Coordonnée horizontale (0 correspond à la gauche de l'écran).
\param y entier Coordonnée verticale (0 correspond au haut de l'écran)
*/

void biGotoXY(int x, int y)
{
#ifdef _WIN32
	COORD c;
	c.X=x;
	c.Y=y;
	CONSOLE_SCREEN_BUFFER_INFO csb;
	GetConsoleScreenBufferInfo(sortie,&csb);
	c.X+=csb.srWindow.Left;
	c.Y+=csb.srWindow.Top;
	SetConsoleCursorPosition(sortie,c);
#else
	if (!CursesInit)
		InitCurses();
	move(y,x);
#endif
}


/*AIDE CouleurTexte
Change la couleur du texte et du fond.
\param texte entier Couleur du texte, de 0 à 15.
\param fond entier Couleur du fond, de 0 à 15.
\desc
Ne fonctionne que sous Windows.
*/
void biCouleurTexte(IInterpreteur * interpreteur)
{
	CInterpreteur * itp=static_cast<CInterpreteur*>(interpreteur);
	valeur couleur;
	valeur fond;
	itp->Pile.Depiler(couleur);
	if (itp->Pile.Dernier().Type!=TYPE_DEBUTPARAM)
		itp->Pile.Depiler(fond);
	itp->Pile.FinParametres();
	int Fond,Texte;
	if (fond.Type)
	{
		Texte=fond.v.i;
		Fond=couleur.v.i;
	}
	else
	{
		Fond=0;
		Texte=couleur.v.i;
	}
#define ROUGE 1
#define VERT 2	
#define BLEU 4
#define INTENSITE 8

#ifdef _WIN32
	WORD Couleur=0;
	if (Texte & ROUGE)
		Couleur|=FOREGROUND_RED;
	if (Texte & VERT)
		Couleur|=FOREGROUND_GREEN;
	if (Texte & BLEU)
		Couleur|=FOREGROUND_BLUE;
	if (Texte & INTENSITE)
		Couleur|=FOREGROUND_INTENSITY;
	if (Fond & ROUGE)
		Couleur|=BACKGROUND_RED;
	if (Fond & VERT)
		Couleur|=BACKGROUND_GREEN;
	if (Fond & BLEU)
		Couleur|=BACKGROUND_BLUE;
	if (Fond & INTENSITE)
		Couleur|=BACKGROUND_INTENSITY;
	SetConsoleTextAttribute(sortie,Couleur);
#else
	
#endif
	itp->Pile.AjouterIndef();
}



/*AIDE
InfosConsole
\param informations reference Une référence vers un tableau qui sera modifié par la fonction.
Les éléments seront dans l'ordre la largeur et la hauteur de l'écran.
\desc
Renvoie un tableau avec des informations sur la console, à commencer (et à finir) par la taille
de l'écran.
*/
void biInfosConsole(IInterpreteur * interpreteur)
{
	CInterpreteur * itp=static_cast<CInterpreteur*>(interpreteur);
	valeur reftab;
	itp->Pile.Depiler(reftab);
	if (reftab.Type!=TYPE_REFERENCE)
		throw TXT("passez une référence vers un tableau en paramètre de InfosConsole");
	itp->Pile.FinParametres();
	valeur *t=itp->Deref(reftab.v.p);
	t->EnTableau();
#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csb;
	GetConsoleScreenBufferInfo(sortie,&csb);
	t->v.t->Get(0)=csb.srWindow.Right-csb.srWindow.Left+1;
	t->v.t->Get(1)=csb.srWindow.Bottom-csb.srWindow.Top+1;
#else
	int mx,my;
	getmaxyx(stdscr,my,mx);
	t->v.t->Get(0)=mx;
	t->v.t->Get(1)=my;
#endif
	itp->Pile.AjouterIndef();
}



/*AIDE NettoyerEcran
Efface l'écran.
*/
void biNettoyerEcran()
{
#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csb;
	GetConsoleScreenBufferInfo(sortie,&csb);
	int tx=csb.srWindow.Right-csb.srWindow.Left+1;
	int ty=csb.srWindow.Bottom-csb.srWindow.Top+1;
	CHAR_INFO * Ecran=new CHAR_INFO[tx*ty];
	memset(Ecran,0,ty*tx*sizeof(CHAR_INFO));
	COORD taille,depart;
	taille.X=tx;
	taille.Y=ty;
	depart.X=0;
	depart.Y=0;
	WriteConsoleOutput(sortie,Ecran,taille,depart,&csb.srWindow);
	delete Ecran;
#else
	if (!CursesInit)
		InitCurses();
	erase();
#endif
}

}
