
#ifndef _GPP_H
#define _GPP_H

#include "mutex.h"
#include "gppmodule.h"

namespace GotoPP
{
	class CInterpreteur;
	/// Description d'un code, pour le désassembleur
	struct DescCode
	{
		carac Nom[32];///<Le nom du code
		int nArgs;///Le nombre d'arguments que prend le code dans la pile
		int Arg[10];///Le type des arguments
	};

	/// Les instructions de la machine virtuelle
	enum Code
	{
		I_STOP,
		I_DEFINIR,
		I_AJOUTER,
		I_CONSTANTE,
		I_VARIABLE,
		I_VALEURVAR,
		I_DEBUTLIGNE,
		I_CHANCES,
		I_SOUSTRAIRE,
		I_CONVTYPE,
		I_FOIS,
		I_DIVISE,
		I_VALEUR,
		I_OPPOSE,
		I_RETOUR,
		I_CONCAT,
		I_TABLEAUREF,
		I_TABLEAUVAL,
		I_CTABLEAUREF,
		I_CTABLEAUVAL,
		I_DEBUTPARAM,
		I_DUPLIQUER,
		I_SUPPRIMER,
		I_VARIABLELOC,
		I_VALEURVARLOC,
		I_ETIQUETTE,
		I_GOTO,
		I_GOTONONZERO,
		I_HACHAGEREF,
		I_HACHAGEVAL,
		I_SUPERPLUS,
		I_INCREMENTER,
		I_DECREMENTER,
		I_DIVISEREGAL,
		I_MULTIPLIEREGAL,
		I_TAILLE,
		I_VALEURCLEF,
		I_THISREF,
		I_THISVAL,
		I_PROBASAUTERLIGNE,
		I_NOUVEAU,
		I_INSTANCEDE,
		I_GOTOTACHE,
		I_FINTACHE,
		I_EGAL,
		I_DIFFERENT,
		I_INFERIEUR,
		I_SUPERIEUR,
		I_INFEGAL,
		I_SUPEGAL,
		I_NON,
		I_GOTOZERO,
		I_DEFCONSTR,
		I_THIS,
		I_NONOPPOSE,
		I_ENERREUR,
		I_ERREUR,
		I_OU,
		I_FINOUET,
		I_ET,
		I_ENTIER,
		I_TYPEDE,
		I_SWITCH,
		I_PARAMREF,
		I_PARAMVAL,
		I_ESTREFVALIDE,
		I_EMPILER2,
		I_DEPILER2,
		I_VALEUR2,
		I_DECALG,
		I_DECALD,
		I_MODULO,
		I_ETBIN,
		I_OUBIN,
		I_EXISTE,
		I_NPARAMS,
		I_AJOUTERE,
		I_SOUSTRAIREE,
		I_FOISE,
		I_DIVISERE,
		I_CARAC,
		I_GOTOR,
		I_GOTOZEROR,
		I_GOTONONZEROR,
		I_GOTOTACHER,
		I_ETIQUETTELOC,
		I_ENERREURR,
		I_CONSTVAR,
		I_EXEC,
		I_EXECVAL,
		I_EXECC,
		I_EXECTHIS,
		I_EXECMEMETHISVAL,//garde le this courant (exec même this val)
		I_EXECTHISC,
		I_EXECTHISMODULE,
		I_FONCTIONC,
		I_FONCTIONMODULE,
		I_ETIQUETTE_THIS,
		I_DEPLACERHAUTPILE,
		I_ALLOCATION,
		I_FONCTIONC_THIS,
		I_EXPREG
	};

	class FichierSource: public gc
	{
	public:
		carac * Source;///<Le code source
		carac * AcPos;///<La position du compilateur dans le code
		size_t Taille;///<La taille du code source
		carac Nom[512];///<Le nom de fichier
		code* DebutCode,///<Permet de retrouver à quel code source correspond un code compilé
			*FinCode;
		bool Charger(const carac * NomFichier, const carac *CheminModules)
		{
			return Charger(NomFichier,czlen(NomFichier),CheminModules);
		}
		///Charge un fichier source en mémoire
		bool Charger(const carac * NomFichier, size_t lNom, const carac *CheminModules);
		uint Ligne;
		FichierSource * Parent;
		bool Compile;
		void Effacer()
		{
			delete[]Source;
		}
		FichierSource():Source(0){}
	};
			
	class CModule: public gc
	{
	public:
		void * Handle;
		carac Nom[TMAXIDENTIFIEUR+1];
		CModule():Handle(0){}
		~CModule();
	};

	struct SLabel:public gc
	{
		int Numero;
		code* Valeur;
		SLabel(){}
	};

	class CTache: public ObjetNonClonable
	{
	public:
		TACHE t;
		CInterpreteur * Int;
		CTache():t(0),Int(0){}
	};

	class CCasSwitch:public gc
	{
	public:
		int Valeur;
		code* Pos;
		CCasSwitch():Pos(0),Valeur(0){}
		CCasSwitch(code* _pos, int _valeur):Pos(_pos),Valeur(_valeur){}
	};

	class CSwitch:public gc
	{
	public:
		Tableau<CCasSwitch> Cas;
		code* PosDefaut;
		CSwitch():PosDefaut(0){}
	};

	

	//Des variables qui contiennent le programme
	
	class ICompilateur;

	class Programme:public gc, public IProgramme
	{
	public://Implémentation de IProgramme
		virtual float GetVersion();
		/*virtual Symbole * NouvellePropriete(carac *Nom, int Prive, 
			voidf FonctionLire, voidf FonctionEcrire);*/
		//virtual int NouvelleVariable(const carac *Nom);

	public:
		Programme();
		
		Mutex mutex;
		//GLOBAL CInstruction Ins;//Le code du programme
		Tableau<CSwitch*> Switch;
		Tableau<SLabel*> Label;
		Tableau<valeur> varGlob;
		Tableau<CModule*> Module;
		Tableau<CTache*> Tache;
		Tableau<FichierSource*> fichierSource;
		Tableau<Symbole*> symbole;
		ICompilateur * compilateur;
		Symbole * symboleGlobal;

		Symbole * NouvelleVariable(const carac *Nom, size_t lNom,
								   CType type, Symbole * parent=0);
		Symbole * NouvelleVariable(const carac *Nom, CType type,
								   Symbole * parent=0);

		Symbole * NouvelleVariable(const carac *Nom, Symbole * cl, 
								   Symbole * parent=0);

		ISymbole * NouvelleFonctionC(voidf f,const carac *Nom, ISymbole * parent=0);

		ISymbole * NouvelleFonctionC(voidf f,const carac *Nom,carac*Type,ISymbole * parent=0);

		Symbole * NouveauSymbole(const carac *Nom, size_t lNom, CType type, Symbole * parent=0);
		Symbole * ChercherClasse(const carac *Nom, Symbole * parent=0);
		ISymbole * NouvelleClasseC(const carac *Nom, size_t lNom,voidf constructeur, ISymbole * parent=0);
		ISymbole * NouvelleMethodeC(voidf Fonc, const carac *Nom,  ISymbole * parent=0);
		Symbole * NouvelleConstante(const carac *Nom, size_t lNom,
										int val, Symbole * parent=0);
		Symbole * NouvelleConstante(const carac * Nom, size_t lNom, Symbole * parent=0);
		
		carac * NomVarGlob(index_t i);
		carac * NomEtiquette(code *c);
		void DefClasses();
		void InitVarGlob();
		code * ChercherEtiquette(int i);

		ISymbole * NouvelleClasseC(const carac * Nom, voidf constructeur, ISymbole * parent=0)
		{
			return NouvelleClasseC(Nom,czlen(Nom),constructeur,parent);
		}
		Symbole * NouvelleConstante(const carac *Nom, int val, Symbole * parent=0)
		{
			return NouvelleConstante(Nom,czlen(Nom),val,parent);
		}
		void SauverBinaire(const carac * Fichier, bool Etat);
		void ChargerBinaire(const carac * Fichier);
		bool ChargerModule(carac *MotClef,uint lClef);
		
	};
	
		
	//Paramètres
	GPP_GLOBAL carac CheminModules[512];

	int CompCasSwitch(const void * a, const void *b);
	int CmpLabels(const void* _a, const void* _b);

	void Init();//Initialise l'interpréteur !
}
#endif
