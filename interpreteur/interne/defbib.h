#ifndef _DEFBIB_H
#define _DEFBIB_H

#include <gotopp/machine.h>

namespace DefBib
{
	//Divers
	//Librairie de fonctions
	//void biSauver();
	//void biCharger();
	void biGotoErreur(GotoPP::IInterpreteur * interpreteur);
	void biNombreChepa(GotoPP::IInterpreteur * interpreteur);
	void biBoum();
	void biGotoDansTexte(GotoPP::IInterpreteur * interpreteur);
	//Processus
	void biDodo(int temps);
	void biBigCrunch(GotoPP::CTache* tache);

	//Fonction de hasard
	void biGotoGoto(GotoPP::IInterpreteur * itp);
	void biGotoMulti(GotoPP::CInterpreteur * itp, bool Sub);
	void biGotoChepa(GotoPP::IInterpreteur * itp);
	void biInitialisation(GotoPP::IInterpreteur * interpreteur);
	int biTableauPoids(GotoPP::ITableauValeurs * poids);
	
	//Mathématiques
	//void biRacine();
	//void biPuissance();
	void tOUTeNpLUS(GotoPP::IInterpreteur * i);
	
	//Tableaux
	void biPremierPingouin(GotoPP::IInterpreteur * interpreteur);
	void biBoiteAPingouins(GotoPP::IInterpreteur * interpreteur);
	void biPousse(GotoPP::IInterpreteur * interpreteur);
	void biPlop(GotoPP::IInterpreteur * interpreteur);
	void biMetsDessous(GotoPP::IInterpreteur * interpreteur);
	void biEnleveDessous(GotoPP::IInterpreteur * interpreteur);
	void biEnleveMilieu(GotoPP::IInterpreteur * interpreteur);
	void biTab(GotoPP::IInterpreteur * interpreteur);
	
	//Chaînes
	int biRechercherDansChaine(const GotoPP::chaineval * texte, GotoPP::IValeur * _atrouver);
	GotoPP::carac biCaractereDansChaine(const GotoPP::chaineval * chaine, int position);
	const GotoPP::chaineval * biExtraireChaine(const GotoPP::chaineval * chaine, int depart, int fin);
	int biLongueurChaine(const GotoPP::chaineval * chaine);
	int biCompChaines(const GotoPP::chaineval * b, const GotoPP::chaineval *a);
	GotoPP::ITableauValeurs * biBananaSplit(const GotoPP::chaineval * chaine, GotoPP::IValeur * _motif);
	void biMarkov(GotoPP::IInterpreteur * _interpreteur);
	const GotoPP::chaineval * biRemplacer(const GotoPP::chaineval * dansquoi, GotoPP::regexp * motif, const GotoPP::chaineval * parquoi);
	const GotoPP::chaineval * biJoindre(GotoPP::ITableauValeurs * _tableau, const GotoPP::chaineval * separateur);
	const GotoPP::chaineval * biChomp(const GotoPP::chaineval * chaine);
	const GotoPP::chaineval * biBegue(const GotoPP::chaineval * chaine, int repetitions);
	
	
	//Console
	void biGotoPrint(GotoPP::IInterpreteur * interpreteur);
	void biGotoPrintDuTexte(GotoPP::IInterpreteur * interpreteur);
	const GotoPP::chaineval * biEntreTonTexte();
	const GotoPP::chaineval * biEntreTonTexteFurtif();
	int biEntreUnNombre();
	void biGotoXY(int x, int y);
	void biCouleurTexte(GotoPP::IInterpreteur * interpreteur);
	void InitConsole();
	void biInfosConsole(GotoPP::IInterpreteur * interpreteur);
	void biNettoyerEcran();
	int biTapeUneTouche();
	void biMontrerCurseur(int montrer);
	void RestaurerConsole();

	//Système
	//Infos sur les répertoires
	const GotoPP::chaineval * biRepCourant();
	const GotoPP::chaineval *  biRepParent();
	const GotoPP::chaineval * biETTelephone(const GotoPP::chaineval * nom);
	//Date et heure
	const GotoPP::chaineval * biTexteDate(GotoPP::entier64 date);
	GotoPP::entier64 biHeure();
	GotoPP::entier64 biDateFichierEnDate(GotoPP::entier64 dateFichier);
	void biDateIncomprehensible(GotoPP::IInterpreteur * interpreteur);
	//Autre système
	const GotoPP::chaineval * biLireEnv(const GotoPP::chaineval * env);
	const GotoPP::chaineval * biConcatRep(const GotoPP::chaineval * parent, const GotoPP::chaineval * fils);
	int biGotoDOS(const GotoPP::chaineval * commande);
	
	//Test
	void TestConstructeur();
	void TestLireEntier();
	void TestEcrireEntier();
	void TestMethode();

	
	void CreerIO(GotoPP::Programme * programme);
	void CreerSysteme(GotoPP::Programme * programme);
}


#endif
