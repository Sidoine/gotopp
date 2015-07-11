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
					 
Une classe de base disposant d'un compteur de référence
avec destructeur virtuel
*/

#ifndef _OBJETC_H
#define _OBJETC_H

namespace GotoPP
{
	class ObjetNonClonable:public IObjet, public gc_cleanup
	{
	public:
		virtual ObjetNonClonable * Copier()
		{
			return this;
		}
		virtual ~ObjetNonClonable(){}
	};

}

#include "bebe.h"
#endif
