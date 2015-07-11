#ifndef _GOTOPP_GLOBAL_H
#define _GOTOPP_GLOBAL_H

namespace GotoPP
{
	void Init();
	extern Symbole * symboleStd;
	extern Symbole * symboleEntier;
	extern Symbole * symboleCarac;
	extern Symbole * symboleTableau;
	extern Symbole * symboleFlottant;
	extern Symbole * symboleTableHachage;
	extern Symbole * symboleChaine;
	extern Symbole * symboleExpReg;
	extern Symbole * symboleValeur;
	extern Symbole * symboleFonctionGPP;
	extern Symbole * symboleMethodeGPP;
	extern Symbole * symboleFonctionC;
	extern Symbole * symboleMethodeC;
	extern Symbole * symboleEtc; //pseudo-symbole, utilisé pour la définition de type
	extern Symbole * symboleDebutParams; //pseudo-symbole, utilisé pour la définition de type
	extern Symbole * symboleReference; //idem
}

#endif
