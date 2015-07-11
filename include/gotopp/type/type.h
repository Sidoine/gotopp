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
					 
Le type d'une variable, etc, en tenant compte des références,
des tableaux, etc.
*/

#ifndef _TYPE_H
#define _TYPE_H

namespace GotoPP
{
	typedef char type;
	typedef short classe;

	enum etype
	{
	//Lors de la compilation, 
		TYPE_PINGOUIN=7,

		//Types pouvant être pris par un pingouin
		//au cours de l'exécution
		TYPE_INCONNU=7,//Valeur indéfinie
		TYPE_CHANCES=1,//Nombre flottant
		TYPE_CARAC=2,//Caractère
		TYPE_ENTIER=3,//Entier 32 bits
		TYPE_CHAINE=4,//Chaînes de caractères
		TYPE_ENTIER64=6,
		TYPE_NOTPOD=0,
		
		//Types pouvant être pris par un pingouin
		//au cours de l'exécution mais n'étant
		//pas une classe en soi
		//=>template
		TYPE_HACHAGE=-1,
		TYPE_OBJETC=-3,
		TYPE_REFERENCE=-4,
		TYPE_DEBUTPARAM=-6,
		TYPE_TABLEAU=-8,//Tableau
		TYPE_ETIQUETTE=-5,
		TYPE_FONCTIONC=-7,//Le numéro d'une fonction C
		TYPE_FONCTIONMODULE=-2,
		TYPE_ETIQUETTE_THIS=-27,
		TYPE_FONCTIONC_THIS=-28,
		TYPE_FONCTIONMODULE_THIS=-29,
		TYPE_SYMBOLE=-30,
		
		//Utilisé par "sauveur"
		TYPE_BEBE=-9,
			
		//Types utilisés par le compilateur/désassembleur
		//(en fait des entiers)
		//Utilité pas très claire
		TYPE_VARIABLE=-10,
		TYPE_VARIABLELOC=-23,//Pas très claire
		TYPE_VARIABLEPARAM=-24,
		TYPE_VARIABLETHIS=-25,
		TYPE_TYPE=-11,
		TYPE_CODE=-12,
		TYPE_ETIQLOC=-14,
		TYPE_ETIQAUTO=-16,
		TYPE_ETIQAUTOBREAK=-17,
		TYPE_OPERATEURVV=-18,
		TYPE_OPERATEURRV=-19,
		TYPE_OPERATEURV=-20,
		TYPE_ETIQAUTOCONTINUE=-21,
		TYPE_ETC=-22,//Nombre de paramètres indéterminé
		
	};
	
	const int NTYPESBASE=8;

	const carac * NomType(type t);

/*
Les types fonctionnent comme une pile...
Par exemple :
entier a(entier chances) => a est de type 
[ entier chances entier fonction reference]
entier a<> => entier tableau
*/
	const int TMAXPILETYPE=16;

	class Symbole;

	class CType
	{
	public:
		Symbole * t[TMAXPILETYPE];
		uchar l;
		CType()
		{
			l=0;
		}

		CType(ISymbole * ty)
		{
			Def(reinterpret_cast<Symbole*>(ty));
		}

		CType(Symbole * ty)
		{
			Def(ty);
		}

		void Vider()
		{
			l=0;
		}
		void Empiler(Symbole * ty)
		{
			if (l==TMAXPILETYPE)
				throw TXT("type trop complexe");
			t[l++]=ty;
		}
		Symbole * Depiler()
		{
			ASSERT(l>0);
			return t[--l];
		}
		Symbole * & Dernier()
		{
			ASSERT(l>0);
			return t[l-1];
		}
		void Def(Symbole * ty)
		{
			l=1;
			t[0]=ty;
		}
		bool EstType(Symbole * ty)
		{
			return (l==1 && t[0]==ty);
		}
		bool EstFonction();
	};
}

#endif
