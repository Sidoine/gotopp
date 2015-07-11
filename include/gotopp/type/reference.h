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
					 
Référence vers une valeur, qu'elle soit dans une table de hachage,
un tableau ou que ce soit une variable globale ou locale...
*/

#ifndef _REFERENCE_H
#define _REFERENCE_H

#include "objetc.h"

namespace GotoPP
{
	namespace RefType
	{
		enum e
		{
			Inconnu,
			Global,
			Local,
			Tableau,
			Hachage,
			Bebe
		};
	}		

	struct reference
	{
		char Type;
		union
		{
			tableauval * Base;
			hachageval * BaseH;
			IObjet * BaseB;
		};
		index_t Decalage;
		void Init()
		{
			Type=RefType::Inconnu;
		}

		void Copier(const reference & r)
		{
			Type=r.Type;
			Decalage=r.Decalage;
			if (Type==RefType::Hachage)
			{
				BaseH=r.BaseH;
			}
			else if (Type==RefType::Tableau)
			{
				Base=r.Base;
			}
			else if (Type==RefType::Bebe)
			{
				BaseB=r.BaseB;
			}
		}
	};

	struct areference:public reference
	{
		areference()
		{
			Init();
		}
	};	
}
#endif
