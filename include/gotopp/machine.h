#ifndef _GPP_MV_H
#define _GPP_MV_H
#include "pile.h"
#include "instruction.h"
#include "gppmodule.h"

namespace GotoPP
{
	class CVariable;
	class CErreur;
	class Programme;
	class CSwitch;
	struct SLabel;
	class CTache;

	class CAppel
	{
	public:
		code * Instruction;
		index_t lPile;//Nombre de valeurs qu'il y avait dans la pile avant l'appel
				//y compris le paramètre de retour
		index_t PosPileF;//Combien il y avait de variables locales avant l'appel
		index_t nParams;//Nombre de paramètres passés à la fonction
		tableauval * This;
		code * EnErreur;
	};

	const int NMAXAPPELS=128;

	class PileFonction
	{
		static const index_t TailleMax=4096;
		valeur val[TailleMax];
	public:
		index_t l;
		valeur & operator[](index_t i)
		{
			ASSERT(i<l);
			return val[i];
		}
		void RedimMin(index_t taille)
		{
			ASSERT(taille<TailleMax);
			if (l<taille)
				l=taille;
		}

		void Redim(index_t taille)
		{
			ASSERT(taille<TailleMax);
			l=taille;
		}
		valeur * Fin()
		{
			return val+l;
		}
		void EffacerDernier()
		{
			ASSERT(l>0);
			val[--l].Effacer();
		}
		PileFonction():l(0){}
	};

	class CInterpreteur:public gc, public IInterpreteur
	{
	public:
		~CInterpreteur() override 
		{
		}

	private:
		CPile Pile2;
		index_t BasenParams;
		tableauval * This;
		uint Ligne;
		carac* AcFichierSource();
		code *DebutLigne;
		CAppel Appel[NMAXAPPELS];
		CInstruction Ins;
		PileFonction PileF;
		index_t posPileF;
		index_t DebutParam;
		index_t DebutVarLoc;
		CSwitch ** Switch;
		
	public:
		valeur * varGlob;
		CPile Pile;
		IObjet * ThisC;
		regexp * DerExpReg;
		valeur DerVar;
		int nAppels;

	public:
		SLabel ** Label;
		Programme * programme;
		void UtiliserProgramme(Programme * programme);
		CTache * NouvelleTache(code * Depart);
		CInterpreteur(Programme * programme);
		//Divers
		valeur * Deref(reference & r);
		void Executer(code * i);
		void Executer();
		void ExecuterCatch(code * i);
		void ExecuterCatch();
		// void ContenuPile();
		void Sauver(FILE *F);
		void Charger(FILE *F,int Taille);
		void Reinit();
		void *Compilateur;
		void Retour();
		void AppelerEtiquette(code * v, bool appelThis=false);
		void AppelerFonctionC(voidf f);
		void AppelerMethodeC(voidf f);
		void Goto(code *v);
		void Exception();
		void LireThis();
		void AfficherErreur(CErreur & e);
		void AfficherErreur(const carac *c);
		void Depiler2(valeur & val);

	public: //Implémentation de IInterpreteur
		IObjet * GetThis() override;
		virtual const IChaine * LireChaine() override;
		virtual int LireEntier() override;
		virtual float LireFlottant() override;
		virtual void * LirePointeur() override;
		virtual void * LireVariable(int v) override;
		virtual IChaineA * LireChaineA() override;
		virtual void FinParametres() override;
		virtual IObjet * LireObjet() override;
		virtual ITableauValeurs * LireTableauValeurs() override;
		virtual regexp * LireRegExp() override;
		virtual IValeur *LireValeur() override;
		virtual entier64 LireEntier64() override;
		virtual int GetNombreParametres() override;
		
		virtual void RetournerChaine(carac * c) override;
		virtual void RetournerChaineA(char * c) override;
		virtual void RetournerEntier(int i) override;
		virtual void RetournerFlottant(float f) override;
		virtual void RetournerIndefini() override;
		virtual void RetournerChaine(const IChaine * chaine) override;
		virtual void RetournerCarac(carac c) override;
		virtual void RetournerTableauValeurs(ITableauValeurs * tableau) override;
		virtual void RetournerEntier64(entier64 i) override;
		virtual void RetournerObjet(IObjet * o) override;

		virtual void Modifier(void * p, IObjet * o) override;
		virtual void Modifier(void * p, int i) override;
		virtual void Modifier(void * p, carac * c) override;
		virtual void Modifier(void * p, carac * c, int l) override;
#ifdef _UNICODE
		virtual void Modifier(void * p, char * c, int l) override;
#endif

		virtual void Ajouter(void *p, const chaineval * clef, const chaineval * val, size_t lval);
		
		virtual void Exception(carac *Texte) override;
		virtual void DebutParametres() override;
		virtual void AppelerFonction(code* f) override;
	};

	typedef void (CInterpreteur::*FonctionGPP)();
}


#endif
