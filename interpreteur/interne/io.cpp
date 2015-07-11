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

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include <gotopp/base.h>
#include <gotopp/compilateur.h>
#include <gotopp/programme.h>
#include <gotopp/mutex.h>

#include "defbib.h"

using namespace GotoPP;

namespace DefBib
{

	class Repertoire:public GotoPP::ObjetNonClonable
	{
	public:
		void Constructeur();
		void ChercherPremier();
		void ChercherSuivant();
		void LireNom();
		void LireEstRepertoire();
		void LireDateModif();
		void Fini();
		void LireTaille();
	};

	class Fichier:public GotoPP::ObjetNonClonable
	{
		FILE * F;
		int acces;
		bool pipe;
		
	public:
		enum Mode {Texte,Binaire,UCS2} mode;
		Fichier():F(0){}
		Fichier(FILE * _F, Mode _mode, int _acces, bool _pipe):F(_F),mode(_mode),acces(_acces),pipe(_pipe)
		{
		}
		void Fermer();
		void Ouvrir(int nParams, const GotoPP::chaineval * nom, int _acces, int _mode);
		const GotoPP::chaineval * LireTexte(int nParams, int nCars);
		int LireEntier(int nParams, int nChiffres);
		void EcrireValeur(GotoPP::valeur * val);
		int Taille();
		int LireMode();
		void ChgMode(int mode);
		int Fin();
		~Fichier();

		static void EcrireTexte(GotoPP::IInterpreteur * itp);
	};

	Fichier::~Fichier()
	{
		Fermer();
	}

	void Fichier::Fermer()
	{
		if ((F)&&(F!=stdin)&&(F!=stdout)&&(F!=stderr))
		{
			if (pipe)
		#ifdef _WIN32
				_pclose(F);
		#else
				pclose(F);
		#endif
			else
				fclose(F);
			F=0;
		}
	}

/*AIDE fichier%OUVRIR
Ouvrir un fichier en lecture ou en écriture
\param nom chaine Nom du fichier à ouvrir.
\param mode entier Mode d'ouverture : 0 en lecture (par défaut), 1 en écriture.
\param binaire entier Par défaut, ouvre en tant que fichier texte. Mettre à 1 pour ouvrir un fichier binaire.
\desc
Quand le fichier est ouvert en écriture, si un fichier du même nom existait déjà,
il est supprimé.

Pour tester si le fichier a bien été ouvert, utiliser la fonction enentier :
F enentier vaut 0 si le fichier n'a pas été ouvert, 1 sinon.

En mode binaire, le fonctionnement des fonctions d'entrée/sortie est modifié :
les valeurs sont enregistrées ou lues dans le fichier dans la même représentation
qu'en mémoire et non en les convertissant en du texte.
\exemple
fichier F
F@OUVRIR(«machin.txt»)
GOTONONNULPOURLESNULS %erreur }F@Fin(){
Ligne=F@LIREDUTEXTE()
GOTOPRINT()
F@OUVRIR(«nouveau.txt» *(1)) GOTO note : le fichier précédent est automatiquement refermé
F@ECRIREDUTEXTE(&Ligne)
GOTOFIN
§erreur 
GOTOPRINTDUTEXTE() ; «Erreur en ouvrant le fichier»
*/
	void Fichier::Ouvrir(int nParams, const chaineval * nom, int _acces, int _mode)
	{
		Fermer();
		acces=nParams>1?_acces:0;
		mode=(Mode)(nParams>2?_mode:0);
		if (nParams==0)
			throw TXT("passez le nom du fichier en paramètre");

		carac Md[3];
		if (acces==0)
			Md[0]='r';
		else
			Md[0]='w';
		if (mode==Binaire||mode==UCS2)
		{
			Md[1]='b';
			Md[2]=0;
		}
		else
			Md[1]=0;

		pipe=(nom->c[0]=='|');
		if (pipe)
			F=czpopen(&nom->c[1],Md);
		else
			F=czfopen(nom->c,Md);
	}

	const chaineval * Fichier::LireTexte(int nParams, int nCars)
	{
		if (nParams==0)
			nCars=-1;

		if (F==NULL)
			return chaineval::Creer(_T(""));

		chaineval * c;
		if (mode==Binaire)
		{
			if (nCars==-1)
				throw TXT("chaînes AZT en binaire non supportées");
			c=chaineval::Creer(nCars);
			fread((carac*)c->c,nCars,1,F);
		}
		else if (mode==Texte)
		{	
			char Temp[128];
			if (!fgets(Temp,127,F))
				*Temp=0;
			size_t l=strlen(Temp);
			if (Temp[l-1]=='\n')
				Temp[--l]=0;
			if (Temp[l-1]=='\r')
				Temp[--l]=0;
			c=chaineval::Creer(Temp,l);
		}
	#ifdef _UNICODE
		else
		{
			wchar_t Temp[128];
			if (!fgetws(Temp,127,F))
				*Temp=0;
			size_t l=wcslen(Temp);
			if (Temp[l-1]=='\n')
				Temp[--l]=0;
			if (Temp[l-1]=='\r')
				Temp[--l]=0;
			c=chaineval::Creer(Temp,l);
		}
	#endif
		return c;
	}

	int Fichier::LireEntier(int nParams, int nChiffres)
	{
		if (nParams==0)
			nChiffres=sizeof(int);

		if (F==NULL)
			return 0;

		int i;

		if (mode==Binaire)
		{
			//TODO: Ne marche qu'en little endian
			if (nChiffres>sizeof(int))
				nChiffres=sizeof(int);
			fread(&i,nChiffres,1,F);
		}
		else if (mode==Texte)
		{
			char Temp[11];
			fgets(Temp,10,F);
			i=atoi(Temp);
		}
	#ifdef _UNICODE
		else
		{
			wchar_t temp[11];
			fgetws(temp,10,F);
			i=_wtoi(temp);
		}
	#endif
		return i;
	}

	void Fichier::EcrireValeur(valeur * val)
	{
		if (mode==Binaire)
			val->EcrireBin(F);
		else if (mode==Texte)
			val->Ecrire(F);
		else
			val->EcrireU(F);
	}

	void Fichier::EcrireTexte(IInterpreteur * _itp)
	{
		CInterpreteur * itp=(CInterpreteur*)_itp;
		Fichier * fichier=(Fichier*)itp->GetThis();
		
		while (!itp->Pile.Vide())
		{
			valeur v;
			itp->Pile.Depiler(v);
			if (v.Type==TYPE_DEBUTPARAM)
				break;

			fichier->EcrireValeur(&v);
 		}
		itp->Pile.AjouterIndef();
	}

/*AIDE fichier%TailleFichier
Renvoie la taille du fichier.
\retour entier La taille du fichiers, en octets.
\desc
Remarquez que cette valeur n'a de sens qu'en mode binaire. En mode texte,
la taille renvoyée ne correspond pas nécessairement au nombre de caractères
(et en fait, si vous ne voulez pas être la risée des gourous du GOTO++, ne
comptez pas dessus).
*/
	int Fichier::Taille()
	{
		if (F==NULL)
			return 0;
		else
		{
	#ifdef _WIN32
			return ((int)_filelength(_fileno(F)));
	#else
			size_t ac=ftell(F);
			fseek(F,0,SEEK_END);
			int taille=((int)ftell(F));
			fseek(F,ac,SEEK_SET);
			return taille;
	#endif
		}
	}

	int Fichier::LireMode()
	{
		return acces;
	}

	
/*AIDE fichier%Fin
Indique si la fin du fichier a été atteinte.
\retour entier 1 si la fin du fichier a été atteinte, 0 sinon.
*/
	int Fichier::Fin()
	{
		return F?(int)feof(F):1;
	}

	

/*AIDE fichier%ChangeMode
Modifie le mode (binaire ou pas) de lecture ou d'écriture d'un fichier.
Bien pratique pour manipuler l'entrée ou la sortie standard.
\param mode entier 1 pour binaire, 0 pour texte.
*/
	void Fichier::ChgMode(int _mode)
	{
		if (!mode)
			return;
		if (_mode)
		{
			acces=_mode;	
	#ifdef _WIN32
			_setmode(_fileno(F),_O_BINARY);
	#endif
		}
		else
		{
			acces=0;
	#ifdef _WIN32
			_setmode(_fileno(F),_O_TEXT);
	#endif
		}
	}

	void CreerIO(Programme * programme)
	{
		ISymbole * classeFichier=CreateurClasseC<DefBib::Fichier>::Creer(programme,0,_T("fichier"));
		CreateurAppelMethodeC_3V<Fichier,const chaineval *,int,int>::AjouterFonction<&Fichier::Ouvrir>(classeFichier,_T("OUVRIR"));
		CreateurAppelMethodeC_R1V<Fichier,const chaineval *, int>::AjouterFonction<&Fichier::LireTexte>(classeFichier,_T("LIREDUTEXTE"));
		CreateurAppelMethodeC_R1V<Fichier,int, int>::AjouterFonction<&Fichier::LireEntier>(classeFichier,_T("LIREUNNOMBRE"));
		programme->NouvelleMethodeC(Fichier::EcrireTexte,_T("ECRIREDUTEXTE"),classeFichier);
		CreateurAppelMethodeC_R0<Fichier,int>::AjouterFonction<&Fichier::Fin>(classeFichier,_T("Fin"));
		CreateurAppelMethodeC_0<Fichier>::AjouterFonction<&Fichier::Fermer>(classeFichier,_T("fermer"));
		CreateurAppelMethodeC_1<Fichier,int>::AjouterFonction<&Fichier::ChgMode>(classeFichier,_T("ChangeMode"));

		programme->NouvelleVariable(_T("entreestd"),CType(classeFichier));
		programme->varGlob.Dernier().Type=TYPE_OBJETC;
		programme->varGlob.Dernier().v.o=new Fichier(stdin,Fichier::Texte,0,false);
		
		programme->NouvelleVariable(_T("sortiestd"),CType(classeFichier));
		programme->varGlob.Dernier().Type=TYPE_OBJETC;
		programme->varGlob.Dernier().v.o=new Fichier(stdout,Fichier::Texte,1,false);
		
		programme->NouvelleVariable(_T("erreurstd"),CType(classeFichier));
		programme->varGlob.Dernier().Type=TYPE_OBJETC;
		programme->varGlob.Dernier().v.o=new Fichier(stderr,Fichier::Texte,1,false);
	}

}
