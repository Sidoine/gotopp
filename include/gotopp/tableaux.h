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
					 
Deux patrons que j'utilise pour mes tableaux. Ils sont bien pratiques.
*/

#ifndef _TABLEAUX_H
#define _TABLEAUX_H
namespace GotoPP
{
	//Équivalent de std::vector
	template <class T, index_t grow=128>
	class Tableau
	{
	public:
		class Iterateur
		{
			Tableau<T,grow> * tableau;
			index_t pos;
		public:
			operator T *()
			{
				return tableau->t;
			}
			Iterateur():tableau(0),pos(0){}
			Iterateur(Tableau<T,grow> * t, index_t _pos):tableau(t),pos(_pos){}
		};

		T * t;
		index_t a;
		index_t l;

		Tableau():l(0),a(0),t(0)
		{
		}
		void Vider()
		{
			T * u=t;
			while (l)
			{
				*(u++)=0;
				l--;
			}
		}
		//Réserve de la mémoire pour au moins n
		//éléments de long
		//Ne fait rien si c'est déjà le cas
		//Evite des réallocations si on sait
		//déjà quelle taille fera le tableau
		//Ne pas accéder directement aux éléments
		//d'indice >=l !
		void Reserve(index_t n)
		{
			if (n<a)
				return;
			a+=grow;
			t=(T*)GC_REALLOC(t,sizeof(T)*a);
		}

		//Redimentionne le tableau pour qu'il
		//contienne au moins n éléments
		void Redim(index_t n)
		{
			if (n>a)
			{
				a=n+grow;
				t=(T*)GC_REALLOC(t,sizeof(T)*a);
			}
			l=n;
		}

		//Ajoute un élément
		void Etendre(T nouveau)
		{
			if (l==a)
			{
				a+=128;
				t=(T*)GC_REALLOC(t,sizeof(T)*a);
			}
			t[l++]=nouveau;
		}

		T & Dernier()
		{
			return t[l-1];
		}
		T * Debut()
		{
			return t;
		}
		T * Fin()
		{
			return t+l;
		}
		Iterateur FinI()
		{
			return Iterateur(this,l);
		}

		T & operator[](index_t i)
		{
			ASSERT(i<l);
			return t[i];
		}
		T & Premier()
		{
			return t[0];
		}
		void EffacerDernier()
		{
			l--;
		}
		T * Chercher(T val)
		{
			for (T * it=t; it<t+l; it++)
			{
				if (*it==val)
					return it;
			}
			return 0;
		}

		void Supprimer(T val)
		{
			Supprimer(Chercher(val));
		}

		void Supprimer(T * position)
		{
			ASSERT(position);
			if (position<Fin())
			{
				if (position<Fin()-1)
					memmove(position,position+1,sizeof(T)*(Fin()-position+1));
				l--;
			}
		}
	};

	//Tableau avec des trous
	template <class TB, index_t grow=128>
	class TableauV
	{
		class T:public TB
		{
		public:
			index_t _vide;
		};
		T * t;
		index_t a;
		index_t l;
		index_t Vide;

	public:
		
		TableauV():Vide(0),l(0),a(0),t(0)
		{
		}

		~TableauV()
		{
			T * u=t;
			for (index_t i=0; i<l; i++)
			{
				if (u->_vide!=~0)
					u->~T();
				u++;
			}
		}

		void Vider()
		{
			l=0;
		}

		index_t Nouveau()
		{
			index_t ret=Vide;
			if (Vide==l)
			{
				if (l==a)
				{
					a+=grow;
					t=(T*)GC_REALLOC(t,a*sizeof(T));
				}
				l++;
				Vide++;
			}
			else
				Vide=t[Vide]._vide;
			//new ((void*)&t[ret]) T();
			t[ret]._vide=~0;
			return ret;
		}

		void Supprimer(index_t i)
		{
			t[i]._vide=Vide;
			t[i].~T();
			Vide=i;
		}

		T & operator[](index_t i)
		{
			return t[i];
		}
	};
}

#endif
