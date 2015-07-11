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
					 
Un tableau de "valeur"s avec décompte des références et de taille variable.

*/

#ifndef _TABLEAU_H
#define _TABLEAU_H

namespace GotoPP
{
	struct valeur;
	struct reference;
	class Symbole;

	class tableauval:public ITableauValeurs, public gc
	{
	public:
		index_t l;
		valeur * v;
		Symbole * Classe;
		byte Dimensions;
		tableauval():v(0),l(0)
		{
		}
		tableauval * Copier();
		valeur & Get(int i);
		valeur & Get(index_t i);
		valeur * ObtPtr(int i);
		valeur * ObtPtr(index_t i);
		void ObtRef(reference & r, int i);
		void Dim(index_t t);
		index_t Taille(){return l;}
		const IValeur * GetValeur(index_t i)
		{
			return reinterpret_cast<IValeur*>(ObtPtr(i));
		}
	};
}

#endif
