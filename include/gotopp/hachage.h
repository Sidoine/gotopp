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
					 
Patron pour une table de hachage
*/

#ifndef _HACHAGE_H
#define _HACHAGE_H

namespace GotoPP
{
	class CBaseHacheValue
	{
	public:
		carac *Nom;
		void Detruire(){};
	};

	struct IndexHachage
	{
		index_t i;
		unsigned int hache;
	};

	template <class T>
	class THachage
	{
		index_t l,a;
		T * v;				
		IndexHachage * Index; //Index vers les clefs triées

		int ObtId(const carac * clef)const
		{
			return ObtId(clef,czlen(clef));
		}
		int ObtId(const carac * clef, size_t lclef) const;
	public:
		~THachage()
		{
			if (Index)
				free(Index);
			for (index_t i=0; i<l; i++)
			{
				delete v[i].Nom;
				v[i].Detruire();
			}
			if (v)
				free(v);
		}
		THachage()
		{
			Index=NULL;
			v=NULL;
			l=a=0;
		}

		bool Existe(const carac * clef)const
		{
			return (ObtId(clef)>=0);
		}
		T & operator[](const carac * clef)
		{
			return Ref(clef,czlen(clef));
		}
		T & Ref(const carac * clef, size_t lclef);
		T* ObtPtr(const carac * clef,size_t lclef) const;
		T * Ajouter(const carac * clef, size_t lclef, int ou);
		T * AjouterSiAbsent(const carac * clef, size_t lclef);
	};

	/*
	Cette fonction est prise à Bob Jenkins
	(http://burtleburtle.net/bob/)
	Merci à lui !
	*/
	inline void mix(uint& a, uint& b, uint& c)
	{
	  a -= b; a -= c; a ^= (c>>13);
	  b -= c; b -= a; b ^= (a<<8);
	  c -= a; c -= b; c ^= (b>>13);
	  a -= b; a -= c; a ^= (c>>12);
	  b -= c; b -= a; b ^= (a<<16);
	  c -= a; c -= b; c ^= (b>>5);
	  a -= b; a -= c; a ^= (c>>3);
	  b -= c; b -= a; b ^= (a<<10);
	  c -= a; c -= b; c ^= (b>>15);
	}

	inline uint CalculerHache(const char * k, size_t length, uint initval=0)
	{
		uint a,b,c;
		size_t len;
	    len = length;
		a = b = 0x9e3779b9;    // le ratio d'or (c'est Bob qui le dit),
							//une valeur arbitraire (je veux bien le croire)
		c = initval;

	   //On s'occupe des octets de trop
	   if (((size_t)k)&3)
	   {
     	 while (len >= 12)    /* unaligned */
     	 {
     	    a += (k[0] +((uint)k[1]<<8) +((uint)k[2]<<16) +((uint)k[3]<<24));
     	    b += (k[4] +((uint)k[5]<<8) +((uint)k[6]<<16) +((uint)k[7]<<24));
     	    c += (k[8] +((uint)k[9]<<8) +((uint)k[10]<<16)+((uint)k[11]<<24));
     	    mix(a,b,c);
     	    k += 12; len -= 12;
     	 }
	   }
	   else
	   {
     	 while (len >= 12)    /* aligned */
     	 {
     	    a += *(uint *)(k+0);
     	    b += *(uint *)(k+4);
     	    c += *(uint *)(k+8);
     	    mix(a,b,c);
     	    k += 12; len -= 12;
     	 }
	   }

	   /*------------------------------------- handle the last 11 bytes */
	   c += (uint) length;
	   switch(len)              /* all the case statements fall through */
	   {
	   case 11: c+=((uint)k[10]<<24);
	   case 10: c+=((uint)k[9]<<16);
	   case 9 : c+=((uint)k[8]<<8);
     	 /* the first byte of c is reserved for the length */
	   case 8 : b+=((uint)k[7]<<24);
	   case 7 : b+=((uint)k[6]<<16);
	   case 6 : b+=((uint)k[5]<<8);
	   case 5 : b+=k[4];
	   case 4 : a+=((uint)k[3]<<24);
	   case 3 : a+=((uint)k[2]<<16);
	   case 2 : a+=((uint)k[1]<<8);
	   case 1 : a+=k[0];
     	/* case 0: nothing left to add */
	   }
	   mix(a,b,c);
	   /*-------------------------------------------- report the result */
	   return c;
	}

	template<class T>
	int THachage<T>::ObtId(const carac * clef, size_t lclef) const
	{
		if (l==0)
			return -1;

		int hache=CalculerHache((char*)clef,lclef*sizeof(carac),0);

		int inf=-1;
		int sup=int(l);
		IndexHachage * idx=Index;

		while (inf<sup-1)
		{
			int mil=(inf+sup)/2;
			int h=idx[mil].hache;
			int ret=(hache<h?-1:(hache>h?1:0));
			if (ret==0)
				ret=memcmp(clef,v[idx[mil].i].Nom,lclef*sizeof(carac));
			if (ret<0)
				sup=mil;
			else if (ret>0)
				inf=mil;
			else
				return int(idx[mil].i);
		}
		return -sup-1;//Où est-ce qu'il faut insérer
	}

	template<class T>
	T * THachage<T>::Ajouter(const carac * clef, size_t lclef, int OuInserer)
	{
		if (l==a)
		{
			a+=128;
			v=(T*) realloc(v,a*sizeof(T));
			Index=(IndexHachage*)realloc(Index,a*sizeof(IndexHachage));
		}
		if (OuInserer!=l)
		{
			memmove(&Index[OuInserer+1],&Index[OuInserer],
				(l-OuInserer)*sizeof(IndexHachage));
		}
		Index[OuInserer].i=l;
		Index[OuInserer].hache=CalculerHache((char*)clef,lclef*sizeof(carac),0);

		v[l].Nom=new carac[lclef+1];
		memcpy(v[l].Nom,clef,lclef*sizeof(carac));
		v[l].Nom[lclef]=0;
		int ret=int(l++);
		return &v[ret];
	}

	template<class T>
	T * THachage<T>::AjouterSiAbsent(const carac * clef, size_t lclef)
	{
		int id=ObtId(clef,lclef);
		if (id<0)
			return Ajouter(clef,lclef,id);
		else
			return NULL;
	}

	template<class T>
	T * THachage<T>::ObtPtr(const carac * clef, size_t lclef) const
	{
		int id=ObtId(clef,lclef);
		if (id<0)
			return NULL;
		else
			return &v[id];
	}

	template<class T>
	T & THachage<T>::Ref(const carac * clef, size_t lclef)
	{
		int id=ObtId(clef,lclef);
		if (id<0)
			return *Ajouter(clef,lclef,-id-1);
		else
			return &v[id];
	}
}
#endif
