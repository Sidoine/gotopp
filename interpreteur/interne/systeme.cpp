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

Définition des fonctions intégrées permettant de faire des appels
au système (système de fichiers, date...).
					 
*/

#include <time.h>
#ifndef _WIN32
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#else
#include <shlobj.h>
#endif

#include <gotopp/base.h>
#include <gotopp/programme.h>
#include "defbib.h"

using namespace GotoPP;

//Objet pour parcourir un répertoire
namespace DefBib
{

	class Repertoire:public IObjet
	{
	public:
	#ifdef _WIN32
		HANDLE hFind;
		WIN32_FIND_DATA fd;
	#else
		char Nom[512];
		DIR * Dir;
		dirent * de;
		struct stat Stat;
		bool StatLue;
		void LireStat()
		{
			if (StatLue)
				return;
			static char Temp[512];
			if (strlen(Nom)+strlen(de->d_name)>510)
				throw TXT("nom de fichier trop long");

			sprintf(Temp,"%s/%s",Nom,de->d_name);

			if (stat(Temp,&Stat))
				throw TXT("le fichier qu'on voulait examiner a disparu");
			StatLue=true;
		}
	#endif
		Repertoire();
		Repertoire * Copier(){return this;}
		~Repertoire();
		void ChercherPremier(const chaineval * filtre);
		void ChercherSuivant();
		const chaineval* LireNom();
		int LireEstRepertoire();
		entier64 LireDateModif();
		entier64 LireTaille();
		int Fini();
	};

	Repertoire::~Repertoire()
	{
	#ifdef _WIN32
		if (hFind)
			FindClose(hFind);
	#else
		if (Dir)
			closedir(Dir);
	#endif
	}

	Repertoire::Repertoire()
	{
	#ifdef _WIN32
		hFind=NULL;
		memset(&fd,0,sizeof(WIN32_FIND_DATA));
	#else
		Dir=NULL;
	#endif
	}

	

/*AIDE
Repertoire%ChercherPremier
Cherche le nom du premier fichier d'un répertoire.
\param repertoire chaine Le répertoire à parcourir.
\desc
Remarquez que « . » et « .. » sont exclus de la liste, contrairement
aux fonctions du même type qu'on trouve dans les autres langages.
\exemple
GOTO Affiche récursivement le contenu du répertoire
niveaugourou 0

%dir(RepCourant())
GOTOFIN

§dir:ASV(base)
	Repertoire rep
	rep@ChercherPremier(&base)
	TantQue } non rep@Fini() {
	
		AUTOGOTOZ } OUQUILESTLETEXTE(rep@LireNom() $ ^/\.[gc]pp$/) { /^
			GOTOPRINTDUTEXTE(;ConcatRep(&base rep@LireNom()))
		
		AUTOGOTOZ } rep@LireEstRepertoire() { /^
			%dir(ConcatRep(&base rep@LireNom()))
		rep@ChercherSuivant()
	FinTantQue
APLUS
*/
	void Repertoire::ChercherPremier(const chaineval * filtre)
	{
	#ifdef _WIN32
		carac Temp[MAX_PATH];
		cznprintf(Temp,MAX_PATH-1,_T("%s\\*.*"),filtre->c);
		hFind=FindFirstFile(Temp,&fd);
		if (hFind)
		{
			while (czcmp(fd.cFileName,_T("."))==0 ||
				czcmp(fd.cFileName,_T(".."))==0)
			{
				if (!FindNextFile(hFind,&fd))
				{
					FindClose(hFind);
					hFind=NULL;
					break;
				}
			}
		}
	#else
		cztostr(Nom,filtre->c,511);
		Dir=opendir(Nom);
		if (Dir)
		{
			de=readdir(Dir);
			while (strcmp(de->d_name,".")==0 ||
				strcmp(de->d_name,"..")==0)
			{
				de=readdir(Dir);
				if (!de)
				{
					closedir(Dir);
					Dir=NULL;
					return;
				}
			}
		}
		StatLue=false;
	#endif
	}

	
/*AIDE
Repertoire%ChercherSuivant
Cherche le répertoire suivant.
*/
	void Repertoire::ChercherSuivant()
	{
	#ifdef _WIN32
		do
		{
			if (!FindNextFile(hFind,&fd))
			{
				FindClose(hFind);
				hFind=NULL;
				return;
			}
		}
		while (czcmp(fd.cFileName,_T("."))==0 ||
			czcmp(fd.cFileName,_T(".."))==0);
	#else
		do
		{
			if (!(de=readdir(Dir)))
			{
				closedir(Dir);
				Dir=NULL;
				return;
			}
		}
		while (strcmp(de->d_name,".")==0 ||
			strcmp(de->d_name,"..")==0);
		StatLue=false;
	#endif
	}



/*AIDE  Repertoire%LireNom
Cette fonction indique le nom d'un fichier.
\retour chaine Le nom du fichier courant
*/
	const chaineval * Repertoire::LireNom()
	{
		chaineval * nom;
	#ifdef _WIN32
		nom=chaineval::Creer(fd.cFileName);
	#else
		nom=chaineval::Creer(de->d_name);
	#endif
		return nom;
	}

	

/*AIDE Repertoire%LireEstRepertoire
Cette fonction renvoie 1 si le fichier actuel est un répertoire, 0 sinon.
\retour entier 1 si le fichier actuel est un répertoire, 0 sinon

*/
	int Repertoire::LireEstRepertoire()
	{
	#ifdef _WIN32
		return (int(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
	#else
		LireStat();
		return (S_ISDIR(Stat.st_mode));
	#endif
	}

	

/*AIDE Repertoire%LireDateModif
Cette propriété indique la date de dernière modification d'un fichier.
\desc
Cette date est renvoyée dans l'unité ayant la précision maximale du système de fichier.
Pour la transformer en une date en secondes depuis le 1er janvier 1970, utilisez
la fonction $DateFichierEnDate$.
\retour entier64 la date de dernière modification du fichier courant
*/
	entier64 Repertoire::LireDateModif()
	{
	#ifdef _WIN32
		return (entier64) (*(entier64*)&fd.ftLastWriteTime);
	#else
		LireStat();
		return (entier64) Stat.st_mtime;
	#endif
	}

	entier64 Repertoire::LireTaille()
	{
	#ifdef _WIN32
		return ((entier64) (*(entier64*)&fd.ftLastWriteTime));
	#else
		LireStat();
		return (entier64) Stat.st_size;
	#endif
	}

	

/*AIDE
Repertoire%Fini
Indique si tout le répertoire a été parcouru.
\retour entier 1 si tout le répertoire a été parcouru, 0 sinon.
*/
	int Repertoire::Fini()
	{
		
	#ifdef _WIN32
		return int(hFind==NULL);
	#else
		return int(Dir==NULL);
	#endif
	}

/*AIDE RepCourant
Renvoie le nom du répertoire courant.
\retour chaine Un truc du genre « . ».
\desc 
Il ne s'agit pas de son nom complet, mais tout simplement de . ou autre chose,
suivant le système d'exploitation.
*/
	const chaineval * biRepCourant()
	{
		return chaineval::Creer(_T("."));
	}

	
/*AIDE RepParent
Renvoie le nom du répertoire parent.
\retour chaine Un truc du genre « .. ».
\desc
Il ne s'agit pas de son nom complet, mais tout simplement de .. ou autre chose,
suivant le système d'exploitation.
*/
	const chaineval *  biRepParent()
	{
		return chaineval::Creer(_T(".."));
	}

	
/*AIDE ConcatRep
Assemble deux trucs (répertoire ou fichier) pour en faire un seul.
\param rep chaine Le répertoire
\param rep chaine Un sous-répertoire de §rep§ ou un fichier de §rep§.
\retour chaine La concaténation de §rep§ et §truc§.
\desc En gros ça met / entre les deux sous UNIX, et \ sous Windows. Et _è"çà sous
GOTIX. Mais vous ne travaillez pas sous GOTIX, n'est-ce pas ? Sales traîtres.
*/
	const chaineval * biConcatRep(const chaineval * parent, const chaineval * fils)
	{
		size_t lTot=fils->l+parent->l+1;
		chaineval * Total=chaineval::Creer(lTot);
		memcpy(Total->c,parent->c,parent->l*sizeof(carac));
	#ifdef _WIN32
		Total->c[parent->l]='\\';
	#else
		Total->c[parent->l]='/';
	#endif
		memcpy(&Total->c[parent->l+1],fils->c,fils->l*sizeof(carac));
		Total->c[Total->l]=0;
		return Total;
	}


	
/*AIDE TexteDate
Transforme une date UNIX en un texte lisible par un être humain.
\param a entier64 Un entier contenant un nombre de secondes depuis le 1er janvier 1970.
\retour chaine Une chaîne du genre « 30 février 2029 à 25:61 ».
*/
	const chaineval * biTexteDate(entier64 date)
	{
		carac Texte[64];
	#ifdef _WIN32
		tm * z=_gmtime64(&date);
	#else
		time_t t=date;
		tm * z=gmtime(&t);
	#endif
		if (!z)
			throw TXT("date invalide");
	#ifdef _UNICODE
		wcsftime(Texte,64,_T("%c"),z);
	#else
		strftime(Texte,64,"%c",z);
	#endif
		return chaineval::Creer(Texte);
	}

	

/*AIDE Quelleheureestil
Renvoie une date au format UNIX. C'est-à-dire... Oh et puis renseignez-vous.
\retour entier64 Un entier représentant la date.
\desc
Notez que c'est un entier long (enfin, au moins sous Windows). Étant d'une nature paresseuse, un certain
nombre d'opérations mathématiques ne fonctionnent pas avec les entiers longs.
*/
	entier64 biHeure()
	{
	#ifdef _WIN32
		return ((entier64)_time64(NULL));
	#else
		return ((entier64) time(NULL));
	#endif
	}


/*AIDE DateFichierEnDate
Transforme une date de fichier en une date de pas fichier (mais tout autant incompréhensible)
\param datefichier entier64 La date renvoyée par les fonctions de fichier
\retour entier64 Une date au format incompréhensible
\desc
Avec certains systèmes d'exploitation cette fonction
ne fait rien. Mais c'est *mal* d'en déduire que vous
pouvez vous en passer si justement vous l'utilisez.
Le GOTO++ n'est pas fait pour tourner sous UNIX, il 
est fait pour tourner sous GOTIX, où les dates de fichier
ne respectent évidemment pas les conventions habituelles.
*/
	entier64 biDateFichierEnDate(entier64 dateFichier)
	{
	#ifdef _WIN32
	#define EPOCH_BIAS 116444736000000000i64
		return ((entier64)((dateFichier-EPOCH_BIAS)/10000000i64));
	#else
		return (dateFichier);
	#endif
	}

/*AIDE DateIncomprehensible
Transforme une date compréhensible (jour, mois, année, heure...) en une
date UNIX.
\param jour entier Jour du mois. De 1 à 31. Sauf parfois, où le maximum est 30, 29 ou 28, mais je ne vous apprends rien.
\param mois entier Le mois, de 0 (janvier) à 11 (décembre).
\param annee entier L'année. Complète, pas sur deux chiffres. Sauf si vous vivez dans le passé. Mais le GOTO++ n'existait pas à l'époque. Ou alors c'est que j'ai enfin terminé mon projet de machine à voyager dans le temps.
\param heures entier L'heure. De 0 à 23.
\param minutes entier Les minutes. De 0 à 59.
\param secondes entier Les secondes. De 23 à 134. Ahah. Non je blague. De 0 à 59.
\retour entier64 La date UNIX correspondante.
*/
	void biDateIncomprehensible(IInterpreteur * interpreteur)
	{
		CInterpreteur * itp=static_cast<CInterpreteur*>(interpreteur);
		int Val[6];
		memset(Val,0,sizeof(Val));
		int c=0;
		valeur v;
		while(c<6)
		{
			itp->Pile.Depiler(v);
			if (v.Type==TYPE_DEBUTPARAM)
				break;
			Val[c++]=v.Entier();
		}
		if (v.Type!=TYPE_DEBUTPARAM)
			itp->Pile.FinParametres();
		
		struct tm Temps;
		Temps.tm_sec=Val[5];
		Temps.tm_min=Val[4];
		Temps.tm_hour=Val[3];
		Temps.tm_mday=Val[2];
		Temps.tm_mon=Val[1];
		Temps.tm_year=Val[0]-1900;
		itp->Pile.Ajouter((entier64)mktime(&Temps));
	}
	
/*AIDE GOTODOS
Demande à l'interpréteur de commande d'exécuter une commande, attend sa fin et renvoie
le résultat.
\param commande chaine La commande à exécuter.
\retour entier La valeur de retour du programme.
*/
	int biGotoDOS(const chaineval * commande)
	{
	#if defined(_UNICODE)
		#if defined(_WIN32) 
			return _wsystem(commande->c);
		#else
			char * cmd=new char[commande->l+1];
			wcstombs(cmd,commande->c,commande->l+1);
			int ret=system(cmd);
			delete[]cmd;
			return ret;
		#endif
	#else
		return system(commande->c);
	#endif
	}


/*AIDE ProtegeonsLaNature
Renvoie la valeur d'une variable d'environnement.
\param variable chaine Nom de la variable d'environnement.
\retour chaine La valeur de cette variable.
*/
	const chaineval * biLireEnv(const chaineval * env)
	{
		const chaineval * Env;
	#ifdef _UNICODE
		char * cmd=new char[env->l+1];
		wcstombs(cmd,env->c,env->l+1);
		Env=chaineval::Creer(getenv(cmd));
		delete[]cmd;
	#else
		Env=chaineval::Creer(getenv(env->c));
	#endif
		return Env;
	}

	const chaineval * biETTelephone(const chaineval * nom)
	{
	#ifdef _WIN32
		carac Base[MAX_PATH]=_T("");
		SHGetSpecialFolderPath(NULL,Base,CSIDL_APPDATA,TRUE);
		czcat(Base,_T("GOTO++/"));
		CreateDirectory(Base,NULL);
		czcat(Base,nom->c);
		CreateDirectory(Base,NULL);
		chaineval * ret=chaineval::Creer(Base,czlen(Base));
		return ret;
	#else
		size_t lenv=strlen(getenv("HOME"));
		size_t l=lenv+nom->l+6;	//le /.gpp/
		char *rep=new char[l+1];
		strcpy(rep,getenv("HOME"));
		strcpy(&rep[lenv],"/.gpp");
		mkdir(rep,0700);
		rep[lenv+5]='/';
		//memcpy(&ret.v->c[lenv+6],nom->c,nom->l);
		cztostr(&rep[lenv+6],nom->c,nom->l);
		rep[l]=0;
		mkdir(rep,0700);
		chaineval * ret=chaineval::Creer(rep);
		return ret;
	#endif
	}

	void CreerSysteme(GotoPP::Programme * programme)
	{

		ISymbole * c=CreateurClasseC<DefBib::Repertoire>::Creer(programme,0,_T("Repertoire"));
		CreateurAppelMethodeC_1<Repertoire, const chaineval*>::AjouterFonction<&Repertoire::ChercherPremier>(c,_T("ChercherPremier"));
		CreateurAppelMethodeC_0<Repertoire>::AjouterFonction<&Repertoire::ChercherSuivant>(c,_T("ChercherSuivant"));
		CreateurAppelMethodeC_R0<Repertoire,int>::AjouterFonction<&Repertoire::Fini>(c,_T("Fini"));
		//TODO: Remplacer par des propriétés
		CreateurAppelMethodeC_R0<Repertoire,const chaineval*>::AjouterFonction<&Repertoire::LireNom>(c,_T("LireNom"));
		CreateurAppelMethodeC_R0<Repertoire,int>::AjouterFonction<&Repertoire::LireEstRepertoire>(c,_T("LireEstRepertoire"));
		CreateurAppelMethodeC_R0<Repertoire,entier64>::AjouterFonction<&Repertoire::LireDateModif>(c,_T("LireDateModif"));
		CreateurAppelMethodeC_R0<Repertoire,entier64>::AjouterFonction<&Repertoire::LireTaille>(c,_T("LireTaille"));
		
	}
}


