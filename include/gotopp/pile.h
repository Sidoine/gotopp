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

Pile de valeurs					 
*/

#ifndef _PILE_H
#define _PILE_H

namespace GotoPP
{
	const carac erreurPilePleine[]=TXT("pile pleine");
	const carac erreurPileVide[]=TXT("pile vide");

	class CPile
	{
		static const int TAILLEPILE=4096;
	public:
		valeur Pile[TAILLEPILE];
		uint Pos;
		CPile()
		{
			Pos=0;
		}
		valeur & Dernier()
		{
			if (Pos==0)
				throw erreurPileVide;
			return Pile[Pos-1];
		}
		void AjouterEtiquette(code * e)
		{
			if (Pos==TAILLEPILE)
				throw erreurPilePleine;
			Pile[Pos].v.et=e;
			Pile[Pos++].Type=TYPE_ETIQUETTE;
		}

		void AjouterIndef()
		{
			if (Pos==TAILLEPILE)
				throw erreurPilePleine;
			Pile[Pos++].Type=TYPE_INCONNU;
		}
		void AjouterDebutParam()
		{
			if (Pos==TAILLEPILE)
				throw erreurPilePleine;
			Pile[Pos++].Type=TYPE_DEBUTPARAM;
		}
		
		void Ajouter(IObjet * o)
		{
			if (Pos==TAILLEPILE)
				throw erreurPilePleine;
			Pile[Pos].Type=TYPE_OBJETC;
			Pile[Pos++].v.o=o;
		}

		void Ajouter(tableauval * t)
		{
			if (Pos==TAILLEPILE)
				throw erreurPilePleine;
			Pile[Pos].Type=TYPE_TABLEAU;
			Pile[Pos++].v.t=t;
		}
		
		void Ajouter(const reference &v)
		{
			if (Pos==TAILLEPILE)
				throw erreurPilePleine;
			Pile[Pos].Type=TYPE_REFERENCE;
			Pile[Pos].v.p.Init();
			Pile[Pos++].v.p.Copier(v);
		}
		void Ajouter(const valeur & v)
		{
			if (Pos==TAILLEPILE)
				throw erreurPilePleine;
			Pile[Pos++]=v;
		}
		void Ajouter(int i)
		{
			if (Pos==TAILLEPILE)
				throw erreurPilePleine;
			Pile[Pos].Type=TYPE_ENTIER;
			Pile[Pos++].v.i=i;
		}
		void Ajouter(entier64 i)
		{
			if (Pos==TAILLEPILE)
				throw erreurPilePleine;
			Pile[Pos].Type=TYPE_ENTIER64;
			Pile[Pos++].v.i64=i;
		}
		void Ajouter(carac i)
		{
			if (Pos==TAILLEPILE)
				throw erreurPilePleine;
			Pile[Pos].Type=TYPE_CARAC;
			Pile[Pos++].v.i=i;
		}
		void Ajouter(float f)
		{
			if (Pos==TAILLEPILE)
				throw erreurPilePleine;
			Pile[Pos].Type=TYPE_CHANCES;
			Pile[Pos++].v.f=f;
		}
		void AjouterType(type Type, int i)
		{
			if (Pos==TAILLEPILE)
				throw erreurPilePleine;
			Pile[Pos].Type=Type;
			Pile[Pos++].v.i=i;
		}
		void AjouterType(type Type, voidf v)
		{
			if (Pos==TAILLEPILE)
				throw erreurPilePleine;
			Pile[Pos].Type=Type;
			Pile[Pos++].v.fonctionC=v;
		}

		void Ajouter(const chaineval * c)
		{
			if (Pos==TAILLEPILE)
				throw erreurPilePleine;
			Pile[Pos].Type=TYPE_CHAINE;
			Pile[Pos++].v.c=c;
		}
		void Ajouter(const carac*c)
		{
			if (Pos==TAILLEPILE)
				throw erreurPilePleine;
			Pile[Pos].Type=TYPE_CHAINE;
			Pile[Pos++].v.c=chaineval::Creer(c);
		}
		void Depiler(valeur & ret)
		{
			if (Pos==0)
				throw erreurPileVide;
			ret=Pile[--Pos];
			Pile[Pos].Effacer();
		}
		void EffacerDernier()
		{
			if (Pos==0)
				throw erreurPileVide;
			Pile[--Pos].Effacer();
		}
		void FinParametres()
		{
			if (Pos==0)
				throw erreurPileVide;
			Pos--;
			if (Pile[Pos].Type!=TYPE_DEBUTPARAM)
				throw TXT("pas le bon nombre de paramètres passés à la fonction");
		}
		int nParams();
		bool Vide()
		{
			return (Pos==0);
		}
		void Vider()
		{
			for (unsigned int p=0; p<Pos; p++)
				Pile[p].Effacer();
			Pos=0;
		}
		~CPile()
		{
			Vider();
		}
		valeur & operator[](index_t i)
		{
			return Pile[i];
		}
	};
}

#endif
