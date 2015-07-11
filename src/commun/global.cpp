#define DEFINITIONS
#include <gotopp/base.h>
#include <gotopp/compilateur.h>
#include <gotopp/programme.h>
#include <gotopp/global.h>
#include <time.h>
#ifdef _WIN32
#	include <windows.h>
#endif
#include "../compilateur/compriv.h"
#include "sauveur.h"
#include <gotopp/machine.h>
using namespace GotoPP;

namespace GotoPP
{
	Symbole * symboleStd;
	Symbole * symboleEntier;
	Symbole * symboleCarac;
	Symbole * symboleTableau;
	Symbole * symboleFlottant;
	Symbole * symboleTableHachage;
	Symbole * symboleChaine;
	Symbole * symboleExpReg;
	Symbole * symboleValeur;
	Symbole * symboleFonctionGPP;
	Symbole * symboleMethodeGPP;
	Symbole * symboleFonctionC;
	Symbole * symboleMethodeC;
	Symbole * symboleEtc; //pseudo-symbole, utilisé pour la définition de type
	Symbole * symboleDebutParams; //pseudo-symbole, utilisé pour la définition de type
	Symbole * symboleReference; //idem


	FnAfficherErreur * AfficherErreur;

	carac NomTypeBase[][32]=
	{
		TXT("INCONNU"),
		TXT("CHANCES"),
		TXT("CARAC"),
		TXT("ENTIER"),
		TXT("CHAINE"),
		TXT("EXPREG"),
		TXT("ENTIER64"),
	};

	carac NomTypeAutre[][32]=
	{
		//Types pouvant être pris par un pingouin
		//au cours de l'exécution mais n'étant
		//pas une classe en soi
		//=>template
		_T("HACHAGE"),
		_T("FONCTION_MODULE"),
		_T("OBJETC"),
		_T("REFERENCE"),
		_T("ETIQUETTE"),
		_T("DEBUTPARAM"),
		_T("FONCTION"),//Le numéro d'une fonction C
		_T("TABLEAU"),//Tableau
		
		//Utilisé par "sauveur"
		_T("BEBE"),
			
		//Types utilisés par le compilateur/désassembleur
		//(en fait des entiers)
		_T("VARIABLE"),
		_T("TYPE"),
		_T("CODE"),
		_T("VARIABLELOC"),
		_T("ETIQLOC"),
		_T("CLASSE"),
		_T("ETIQAUTO"),
		_T("ETIQAUTOBREAK"),
		_T("OPERATEURVV"),
		_T("OPERATEURRV"),
		_T("OPERATEURV"),
		_T("ETIQAUTOCONTINUE")
	};

	DescCode descCode[255]=
	{
		{_T("STOP"),0},
		{_T("DEFINIR"),0},
		{_T("AJOUTER"),0},
		{_T("CONSTANTE"),2,{TYPE_TYPE,TYPE_INCONNU}},
		{_T("VARGLOBREF"),1,{TYPE_VARIABLE}},
		{_T("VARGLOBVAL"),1,{TYPE_VARIABLE}},
		{_T("DEBUTLIGNE"),1,{TYPE_ENTIER}},
		{_T("CHANCES"),1,{TYPE_CHANCES}},
		{_T("SOUSTRAIRE"),0},
		{_T("CONVTYPE"),2,{TYPE_TYPE,TYPE_TYPE}},
		{_T("FOIS"),0},
		{_T("SUR"),0},
		{_T("VALEUR"),0},
		{_T("ABSOLUE"),0},
		{_T("OPPOSE"),0},
		{_T("REGOTO"),0},
		{_T("CONCAT"),0},
		{_T("TABLEAUREF"),0},
		{_T("TABLEAUVAL"),0},
		{_T("CTABLEAUREF"),1,{TYPE_ENTIER}},
		{_T("CTABLEAUVAL"),1,{TYPE_ENTIER}},
		{_T("DEBUTPARAM")},
		{_T("DUPLIQUER")},
		{_T("SUPPRIMER")},
		{_T("VARLOCREF"),1,{TYPE_VARIABLELOC}},
		{_T("VARLOCVAL"),1,{TYPE_VARIABLELOC}},
		{_T("ETIQUETTE"),1,{TYPE_ETIQUETTE}},
	//	{_T("ETIQLOC"),1,{TYPE_ENTIER}},
		{_T("GOTO"),1,{TYPE_ETIQUETTE}},
		{_T("GOTONONZERO"),1,{TYPE_ETIQUETTE}},
		{_T("HACHAGEREF")},
		{_T("HACHAGEVAL")},
		{_T("SUPERPLUS")},
		{_T("INCREMENTER")},
		{_T("DECREMENTER")},
		{_T("DIVISEREGAL")},
		{_T("MULTIPLIEREGAL")},
		{_T("TAILLE")},
		{_T("VALEURCLEF")},
		{_T("THISREF"),1,{TYPE_ENTIER}},
		{_T("THISVAL"),1,{TYPE_ENTIER}},
		{_T("PROBASAUTERLIGNE"),2,{TYPE_CHANCES,TYPE_ENTIER}},
		{_T("NOUVEAU")},
		{_T("INSTANCEDE")},
		{_T("GOTOTACHE"),1,{TYPE_ENTIER}},
		{_T("FINTACHE")},
		{_T("EGAL")},
		{_T("DIFFERENT")},
		{_T("INFERIEUR")},
		{_T("SUPERIEUR")},
		{_T("INFEGAL")},
		{_T("SUPEGAL")},
		{_T("NON")},
		{_T("GOTOZERO"),1,{TYPE_ENTIER}},
		{_T("DEFCONSTR"),2,{TYPE_ENTIER,TYPE_ENTIER}},
		{_T("THIS")},
		{_T("NONOPPOSE")},
		{_T("ENERREUR"),1,{TYPE_ETIQUETTE}},
		{_T("ERREUR")},
		{_T("OU"),1,{TYPE_ETIQLOC}},
		{_T("FINOUET")},
		{_T("ET"),1,{TYPE_ETIQLOC}},
		{_T("ENTIER"),1,{TYPE_ENTIER}},
		{_T("TYPEDE")},
		{_T("SWITCH"),1,{TYPE_ENTIER}},
		{_T("PARAMREF"),1,{TYPE_ENTIER}},
		{_T("PARAMVAL"),1,{TYPE_ENTIER}},
		{_T("ESTREFVALIDE")},
		{_T("EMPILER2")},
		{_T("DEPILER2")},
		{_T("VALEUR2")},
		{_T("DECALG")},
		{_T("DECALD")},
		{_T("MODULO")},
		{_T("ETBIN")},
		{_T("OUBIN")},
		{_T("EXISTE")},
		{_T("NPARAMS")},
		{_T("AJOUTERE")},
		{_T("SOUSTRAIREE")},
		{_T("FOISE")},
		{_T("DIVISERE")},
		{_T("CARAC"),1,{TYPE_CARAC}},
		{_T("GOTOR"),1,{TYPE_ETIQLOC}},
		{_T("GOTOZEROR"),1,{TYPE_ETIQLOC}},
		{_T("GOTONONZEROR"),1,{TYPE_ETIQLOC}},
		{_T("GOTOTACHER"),1,{TYPE_ETIQLOC}},
		{_T("ETIQUETTELOC"),1,{TYPE_ETIQLOC}},
		{_T("ENERREURR"),1,{TYPE_ETIQLOC}},
		{_T("CONSTVAR"),1,{TYPE_ENTIER}},
		{_T("EXEC"),1,{TYPE_ETIQUETTE}},
		{_T("EXECVAL"),0},
		{_T("EXECC"),1,{TYPE_ENTIER}},
		{_T("EXECTHIS")},
		{_T("EXECMEMETHISVAL")},
		{_T("EXECTHISC"),1,{TYPE_ENTIER}},
		{_T("EXECTHISMODULE"),1,{TYPE_ENTIER}},
		{_T("FONCTIONC"),1,{TYPE_FONCTIONC}},
		{_T("FONCTIONMODULE"),1,{TYPE_FONCTIONMODULE}},
		{_T("ETIQUETTE_THIS"),1,{TYPE_ETIQUETTE}},
		{_T("DEPLACERHAUTPILE"),1,{TYPE_ENTIER}},
		{_T("ALLOCATEUR"),1,{TYPE_ENTIER}},
		{_T("FONCTIONC_THIS"),1,{TYPE_FONCTIONC}},
		{_T("EXPREG"),1,{TYPE_OBJETC}}
	};
}

const carac * GotoPP::NomType(type t)
{
	if (t>=0)
		return NomTypeBase[t];
	else
		return NomTypeAutre[-t-1];
}

void ChercherChemin(carac *CheminModules)
{
	//On supprime le nom de l'exécutable
	carac *c=CheminModules;
	while (*c)
		c++;
	while (*c!='/' && *c!='\\' && c>CheminModules)
		c--;
	*c=0;
	if (c>CheminModules+4
			&& c[-1]=='n'
		 && c[-2]=='i' && c[-3]=='b'
			&& (c[-4]=='/'||c[-4]=='\\'))
		c[-4]=0;
}		

namespace GotoPP
{
	void RegExp_GetValeur(IInterpreteur * _inter)
	{
		CInterpreteur * inter=(CInterpreteur*)_inter;
		int j=inter->LireEntier();
		inter->FinParametres();
		inter->RetournerChaine(inter->DerExpReg->GetValeur(j));
	}

	void InitSymbolesStd()
	{
		symboleStd=new Symbole(_T("std"),0,Symbole::Classe);
		symboleEntier=new Symbole(_T("entier"),symboleStd,Symbole::ClasseC);
		symboleEntier->pod=TYPE_ENTIER;
		symboleCarac=new Symbole(_T("carac"),symboleStd,Symbole::ClasseC);
		symboleCarac->pod=TYPE_CARAC;
		symboleTableau=new Symbole(_T("_tableau"),symboleStd,Symbole::ClasseC);
		symboleTableau->pod=TYPE_TABLEAU;
		symboleFlottant=new Symbole(_T("chances"),symboleStd,Symbole::ClasseC);
		symboleFlottant->pod=TYPE_CHANCES;
		symboleTableHachage=new Symbole(_T("_tablehachage"),symboleStd,Symbole::ClasseC);
		symboleTableHachage->pod=TYPE_HACHAGE;
		symboleChaine=new Symbole(_T("chaine"),symboleStd,Symbole::ClasseC);
		symboleChaine->pod=TYPE_CHAINE;
		symboleValeur=new Symbole(_T("pingouin"),symboleStd,Symbole::ClasseC);
		symboleValeur->pod=TYPE_PINGOUIN;
		symboleFonctionGPP=new Symbole(_T("etiquette"),symboleStd,Symbole::ClasseC);
		symboleFonctionGPP->pod=TYPE_ETIQUETTE;
		symboleMethodeGPP=new Symbole(_T("_methodegpp"),symboleStd,Symbole::ClasseC);
		symboleFonctionC=new Symbole(_T("_fonctionc"),symboleStd,Symbole::ClasseC);
		symboleMethodeC=new Symbole(_T("_methodec"),symboleStd,Symbole::ClasseC);
		symboleEtc=new Symbole(_T("_etc"),symboleStd,Symbole::ClasseC);
		symboleDebutParams=new Symbole(_T("_debutparams"),symboleStd,Symbole::ClasseC);
		symboleReference=new Symbole(_T("_reference"),symboleStd,Symbole::ClasseC);
		symboleReference->pod=TYPE_REFERENCE;
		symboleExpReg=new Symbole(_T("TuerOgre"),symboleStd,Symbole::ClasseC);
		symboleExpReg->defConstr=(Symbole*)CreateurAppelFonctionC_R2<regexp*,const carac*,const carac*>::
			AjouterFonction<&regexp::Creer>(symboleExpReg,_T("berger"));
		CreateurAppelMethodeC_R1<regexp,const chaineval *, int>::AjouterFonction<&regexp::GetValeur>(symboleExpReg,_T("Valeur"));
		symboleExpReg->NouvelleFonctionC(&RegExp_GetValeur,_T("DerValeur"));
		
	}
	class BaseGotoPP:public IGotoPP
	{
	public:
		void * Allouer(size_t t)
		{
			return GC_MALLOC(t);
		}
		void * Reallouer(void * m, size_t t)
		{
			return GC_REALLOC(m,t);
		}
		IChaine * AllouerChaine(const carac* texte, size_t longueur)
		{
			return chaineval::Creer(texte,longueur);
		}
		IChaine * AllouerChaine(const carac* texte)
		{
			return chaineval::Creer(texte,czlen(texte));
		}
#ifdef _UNICODE
		IChaine * AllouerChaine(const char* texte, size_t longueur)
		{
			return chaineval::Creer(texte,longueur);
		}
		IChaine * AllouerChaine(const char* texte)
		{
			return chaineval::Creer(texte,strlen(texte));
		}
#endif
		bool TesterVersion(float version, bool unicode)
		{
			return version>=VersionMin && unicode==Unicode;
		}
	}
	baseGotoPP;

	IGotoPP * iGotoPP=&baseGotoPP;

}


void GotoPP::Init()
{
	time_t z;
	srand((unsigned int)(time(&z)));
//	Source=NULL;
//	InitAccesModule();
	GC_init();
	GC_disable();

	InitSymbolesStd();

#ifdef _WIN32
	GetModuleFileName(NULL,GotoPP::CheminModules,512);
	ChercherChemin(GotoPP::CheminModules);
#else
	char * c=getenv("GOTOPP");
	if (c)
		strtocz(GotoPP::CheminModules,c,512);
	else
	{
		char tmp[512];
		if (readlink("/proc/self/exe",tmp,512)!=-1)
		{
			strtocz(GotoPP::CheminModules,tmp,512);
			ChercherChemin(GotoPP::CheminModules);
		}
	}
#endif
}
