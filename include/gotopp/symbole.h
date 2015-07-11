#ifndef _GOTOPP_SYMBOLE_H
#define _GOTOPP_SYMBOLE_H

namespace GotoPP
{
	class Programme;

	class Symbole: public ISymbole
	{
	public:
		enum Type
		{
			Classe, 
			ClasseC,
			Bloc,
			VariableLocale, 
			VariableGlobale,
			VariableParam,
			VariableThis,
			//TODO: vérifier que les valeurs suivantes sont pertinentes
			//mettre dans typeDepart ?
			FonctionGPP,
			MethodeGPP,
			FonctionC,
			MethodeC,
			EtiquetteLocale
		}
		type; ///<Type de symbole

		Symbole(const GotoPP::carac * nom, size_t lNom, Symbole * parent=0, Type type=VariableGlobale);
		Symbole(const GotoPP::carac * nom,Symbole * parent=0, Type type=VariableGlobale)
		{
			new (this) Symbole(nom,czlen(nom),parent,type);
		}
		
		ISymbole * NouvelleMethodeC(voidf Fonc, const carac * nom);
		ISymbole * NouvelleFonctionC(voidf Fonc, const carac * nom);

		Symbole * Copier() { return this;}

		carac nom[TMAXIDENTIFIEUR+1];

		static const unsigned char constante=1;
		static const unsigned char filsthis=2;
		static const unsigned char filsglob=4;//les fils sont
			//tous des variables globales
		static const unsigned char autoref=8;//Les variables
		//déclarées comme étant de ce type sont toujours
		//en fait des références vers une variable de ce type
		
		unsigned char attributs;
		enum
		{
			Public,Protege,Prive,Cache,Local
		}
		acces;

		CType typeDepart; ///<Le type de départ
		CType TypeAc;///<Pour le mot-clef "zap" : change le type de donnée d'une variable
		index_t indice; ///<Numéro de la variable globale, etc.
		int etiquette; ///<Valeur de l'étiquette (GOTO++)
		valeur Valeur;///<La valeur, si c'est une constante
		Symbole * defConstr;
		Programme * programme;

		etype pod;

		Symbole *parent, *suivant, *fils; //espace de nommage
		size_t taille;//Taille prise par le symbole en mémoire

		Symbole():attributs(0),acces(Public),parent(0),
			suivant(0),fils(0),taille(0){}
		bool EstClasse()
		{
			return type==Classe || type==ClasseC;
		}
		bool PeutContenirFonction()
		{
			return type==FonctionGPP || type==MethodeGPP || type==Classe;
		}
		bool PeutContenirLocaux()
		{
			return type==FonctionGPP || type==MethodeGPP || type==Bloc;
		}

		bool IsPod()
		{
			return pod!=TYPE_NOTPOD;
		}
	};
}

#endif
