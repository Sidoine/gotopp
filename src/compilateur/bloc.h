#ifndef GOTOPP_COMPILATEUR_BLOC_H
#define GOTOPP_COMPILATEUR_BLOC_H
#include <gotopp/symbole.h>
#include <gotopp/tableaux.h>
#include <gc_cpp.h>

namespace GotoPP
{
	struct LigneDeCode;

	enum class TypeBloc
	{
		Si,
		Sinon,
		TantQue,
		Switch,
		Fonction,
		Rien,
		For,
		ForEach,
		Classe
	};

	struct Bloc : public gc
	{
		TypeBloc type;
		bool blocLigne;
		int depart;
		Tableau<int> breaks;
		Tableau<int> continues;
		Symbole * symbole;

		// L'espace de nommage parent était spécifié dans la définition du bloc, on doit le supprimer aussi
		bool classeSpecifiee;

		// Le i++ là dedans : for (i=0;i<10;i++)
		LigneDeCode * pasDuFor;
		Symbole * indice, *tableau, *iterateur;
		Bloc() : blocLigne(false), classeSpecifiee(false), pasDuFor(nullptr)
		{}
	};
}

#endif
