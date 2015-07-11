#ifndef _GOTOPP_COMPRIV_H
#define _GOTOPP_COMPRIV_H
#include <gotopp/programme.h>
#include <gotopp/gppmodule.h>

#include "../commun/instr.h"
#include "../commun/classe.h"
#include "instrconstr.h"

namespace GotoPP
{
	class Programme;
	class CErreur;

	const carac SEP_TAB_O='<';
	const carac SEP_TAB_F='>';
	const carac SEP_PROP='@';
	const carac SEP_HAC_O='[';
	const carac SEP_HAC_F=']';
	const int NMAXINSTRPARLIGNE=256;


	struct SEtiqA
	{
		int Etiq;//Position de l'étiquette
		int Ligne;//Position du début de ligne
		SEtiqA(int _etiq, int _ligne):Etiq(_etiq),Ligne(_ligne){}
	};

	struct SEtiqAA
	{
		int Etiq;//Position de l'étiquette
		int Ligne;//Position du début de ligne
		int CibleAuto;//Quelle est le numéro de la cible
		SEtiqAA(){}
	};


	enum ETypeVar
	{
		TV_GLOBALE,
		TV_LOCALE,
		TV_PARAMETRE
	};

	enum ETypeBloc
	{
		TB_SI,
		TB_SINON,
		TB_TANTQUE,
		TB_SWITCH,
		TB_FONCTION,
		TB_RIEN,
		TB_FOR,
		TB_FOREACH,
		TB_CLASSE
	};

	struct SVarLongue
	{
		CType Type;
		bool Val; //Est-ce qu'on veut la valeur ou la référence ?
		int nInstrsDepart;//Instruction de départ. Si nul la variable longue 
			//est encore vide
		SVarLongue()
		{
			Val=false;
			nInstrsDepart=0;
		}
	};

	enum EInstrPrec
	{
		IP_VARIABLE,
		IP_OPERATEUR,
		IP_PARO,
	};

	enum ELignePrec
	{
		LP_NORMALE,
		LP_FINSI,
		LP_DEBUTBLOC,
		LP_FOR_INIT,
		LP_FOR_TEST,
		LP_FOR_PAS,
		LP_FOREACH_INIT,
		LP_CAS
	};

	enum EInstrSpec
	{
		SPE_NORMAL=0,
		SPE_PAR_O=1,
		SPE_PAR_F=2,
		SPE_FIN=3,
		SPE_PAR_OM=4,
		SPE_DEJAMIS=5,
		SPE_NOOP=6
	};

	struct SInstr
	{
		uint p; //Priorité
		uint pVal,nVal; //Les valeurs faisant partie de l'instruction
		EInstrSpec Special;
		code Code;
	};

	struct LigneDeCode
	{
		SInstr ins[NMAXINSTRPARLIGNE];
		valeur val[NMAXINSTRPARLIGNE];
		uint nIns,nVals;
		LigneDeCode():nIns(0),nVals(0){}
		SInstr & Inserer(uint pos, uint nVals);
		SInstr & Ajouter(uint nVals);
		void Supprimer(int pos);
		void Init(){nIns=0;nVals=0;}
	};

	struct CBloc:public gc
	{
		ETypeBloc Type;
		bool BlocLigne;
		int Depart;
		Tableau<int> Break;
		Tableau<int> Continue;
		Symbole * symbole;
		bool classeSpecifiee;//L'espace de nommage parent était
			//spécifié dans la définition du bloc, on doit le supprimer aussi
		LigneDeCode * PasDuFor;//Le i++ là dedans : for (i=0;i<10;i++)
		Symbole * indice, * tableau, *iterateur;
		CBloc():PasDuFor(0),BlocLigne(false),classeSpecifiee(false)
			{}
	};

	struct SLabelL
	{
		int Numero;
		int Valeur;
		SLabelL(){}
	};

	class TypeEtInstr:public CType
	{
		//Cette classe rajoute un champ pour indiquer quelle instruction est
		//responsable de l'ajout de la variable ayant ce type, afin de pouvoir
		//insérer une instruction de conversion
	public:
		uint instruction;
		void operator=(const CType & t)
		{
			this->CType::operator=(t);
		}
	};

	class Compilateur:public gc, public ICompilateur
	{
	public:
		//Debug
		int AcLigne();
		carac * AcFichier();

		void AfficherErreur(CErreur & e);
		void AfficherErreur(const carac *c);
		BoutCode* Compiler();
		Programme * programme;
		FichierSource * AcFichierSource;
		InstrConstr Ins;
		carac * Source;
		Compilateur(Programme * programme);
		GotoPP::BoutCode* Charger(const carac*fichier);
		GotoPP::BoutCode* Compiler(const carac *CodeSource);

	private:
		Tableau<int> RefEtiq;
		Tableau<int> RefEtiqL; //Références à des étiquettes locales
		Tableau<SEtiqA> RefEtiqA; //Références à des étiquettes automatiques
		Tableau<CBloc*> Bloc;
		index_t CibleAuto;//Numéro de la cible automatique que peuvent partager
			//plusieurs GOTO automatiques
		Tableau<int> EtOu;
		uint Ligne;
		Tableau<int> AcSwitch;
		valeur Valeur[NMAXINSTRPARLIGNE];
		TypeEtInstr PileType[NMAXINSTRPARLIGNE];
		uint PosPileType;
		int DebutLigne;
		uint nValeurs;
		SInstr Instr[NMAXINSTRPARLIGNE];
		uint nInstr;
		LigneDeCode insTri;
		SInstr Temp[NMAXINSTRPARLIGNE];
		uint nTemp;
		size_t lClef;
		index_t DernierSi;
		carac * MotClef;
		float ProbaSauterLigne;
		ELignePrec LignePrec,LigneAc;
		EInstrPrec InstrPrec,InstrAc;
		static const int NMAXVARLONGUES=16;
		SVarLongue VarLongue[NMAXVARLONGUES];
		index_t iVarLongue;//Variable en cours de traitement
		int NiveauGourou;
		bool DeclarationStricte;//Interdit de déclarer implicitement des variables
		Tableau<Symbole*> useEspace;
		Tableau<Symbole*>::Iterateur etaitPremierSymbole;

		void PreparerInt(CInterpreteur *Int);
		
		void LireType(CType &Type);

		bool CompilerClasse(Symbole * s);
		bool CompilerConstante();
		bool CompilerMotClef();
		bool CompilerMotClef0();	
		bool CompilerMotClef1();		
		bool CompilerMotClef2();	
		bool CompilerMotClef3();	
		bool CompilerOperateur();
		bool CompilerSeparateur();
		bool CompilerTableau();
		bool CompilerVariable(Symbole * s, bool Valeur);
		void CompilerFonction();
		void AjouterInstruction(uint j);
		void InsererChangementType(int posPile, Symbole * nouveauType);
		void CreerCodeFinal(LigneDeCode & insTri);
		void DebutBloc(ETypeBloc Type,Symbole * espace=0);
		void FinBloc();
		void SauterEspaces();
		void ChercherMotClef(bool avecChiffres=false);
		void FinOu();
		bool SymboleDisponible();//Vérifie que le symbole est disponible
		Symbole * ChercherSymbole(bool creer=false);//Dans tout l'espace de nommage accessible
		Symbole * ChercherSymboleDans(Symbole * espace);//Uniquement dans espace
		Symbole * ChercherSymboleSimple(CBloc ** minEN=0);//Sans séparateur du style ::
		Symbole * LireEtiquette(bool &local);
		void InsererConstante(Symbole * fonction);
	
		void SauverBinaire(const carac * Fichier,bool Etat);
		void ChargerBinaire(const carac * Fichier);
		void AfficherType(CType & t);
	//	int NouvelleTache(CInterpreteur * Int);
		void SupprimerVarLongue();

		void Desass(bool UneLigne);

		void PremierePasse();
		void ListeVars();

		void InitAccesModule();
		
		bool Executer();

		

		inline void AjOperateurVV(int y);
		inline void AjParO(uint priorite); 
		inline void AjParF();
		inline void AjDebutParams();
		inline void AjCode(code Code, uint Priorite);
		inline void AjEntier(int e);
		inline void AjEntierType(int e, type Type);
		inline void AjExpReg(carac *er, carac* modif);
		inline void AjCarac(carac c);
		inline void AjChaine(carac *c, size_t l);
		inline void AjNoOp(uint priorite);
		inline void AjChances(float f);
		inline void AjType(type Type);
		inline void AjPtrType(Symbole * ptr, type Type);

		inline void AjouterArgs(int argc,carac **argv, int DebutArg);
		inline bool EstNouveauSymbole(Symbole * s);
		Symbole & DernierSymbole()
		{
			return *programme->symbole.Dernier();
		}
	};
}

#endif
