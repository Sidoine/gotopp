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
					 
En-tête de la classe valeur
*/
#include "chaine.h"
#include "tabval.h"
#include "hachval.h"
#include "objetc.h"
#include "expreg.h"
#include "reference.h"
#include "type.h"

#ifndef _VALEUR_H
#define _VALEUR_H



namespace GotoPP
{
	union valeurunion
	{
		int i;
		entier64 i64;
		float f;
		const chaineval * c;
		tableauval * t;
		reference p;
		hachageval * h;
		IObjet * o;
		regexp * er;
		code * et;
		Symbole * ptr; //TODO: dans le compilateur on devrait utiliser autre chose que valeurunion
		voidf fonctionC;
	};

	struct valeur:public IValeur, public gc
	{
		valeurunion v;
		type Type;
		valeur()
		{
			Type=TYPE_INCONNU;
		}
		void Init()
		{
			Type=TYPE_INCONNU;
		}
		void Sauver(FILE *F);
		void Charger(FILE *F);
		void Effacer()
		{
			Type=TYPE_INCONNU;
			v.i=0;
		}
		valeur(int i)
		{
			Type=TYPE_ENTIER;
			v.i=i;
		}
		valeur(float f)
		{
			Type=TYPE_CHANCES;
			v.f=f;
		}
		valeur * Copier()
		{
			return new valeur(*this);
		}

		void CopieDe(const valeur &w)
		{
			switch(w.Type)
			{		
			case TYPE_CHAINE:
				{
					v.c=w.v.c;
					break;
				}
			case TYPE_TABLEAU:
				{
					v.t=w.v.t->Copier();
					break;
				}
			case TYPE_REFERENCE:
				{
					v.p.Init();
					v.p.Copier(w.v.p);
					break;
				}
			case TYPE_HACHAGE:
				{
					v.h=w.v.h->Copier();
					break;
				}
			case TYPE_OBJETC:
				{
					v.o=w.v.o->Copier();
					break;
				}
			case TYPE_ENTIER64:
				{
					v.i64=w.v.i64;
					break;
				}
			default:
					v.i=w.v.i;
			}
			Type=w.Type;
		}
		void operator=(const valeur &w)
		{	
			switch(w.Type)
			{		
			case TYPE_CHAINE:
				{
					v.c=w.v.c;
					break;
				}
			case TYPE_TABLEAU:
				{
					v.t=w.v.t;
					break;
				}
			case TYPE_REFERENCE:
				{
					v.p.Init();
					v.p.Copier(w.v.p);
					break;
				}
			case TYPE_HACHAGE:
				{
					v.h=w.v.h;
					break;
				}
			case TYPE_OBJETC:
				{
					v.o=w.v.o;
					break;
				}
			case TYPE_ENTIER64:
				{
					v.i64=w.v.i64;
					break;
				}
			default:
					v.i=w.v.i;
			}
			Type=w.Type;
		}
		const chaineval * Chaine() const
		{
			const chaineval * ret;
			switch(Type)
			{
			case TYPE_CHAINE:
				ret=v.c;
				break;
			case TYPE_CARAC:
				ret=chaineval::CreerCaractere(v.i);
				break;
			case TYPE_CHANCES:
				ret=chaineval::CreerFlottant(v.f);
				break;
			case TYPE_TABLEAU:
				ret=chaineval::Creer(_T("tableau"));
				break;
			case TYPE_HACHAGE:
				ret=chaineval::Creer(_T("hachage"));
				break;
			case TYPE_OBJETC:
				ret=chaineval::Creer(_T("objet"));
				break;
			default:
				ret=chaineval::CreerEntier(v.i);
			}
			return ret;
		}
		int Entier() const
		{
			switch(Type)
			{
			case TYPE_INCONNU:
				return 0;
			case TYPE_CHANCES:
				return (int)v.f;
			case TYPE_CHAINE:
				return v.c->Entier();
			case TYPE_TABLEAU:
				return int(v.t->l);
			case TYPE_ENTIER64:
				return (int)v.i64;
			default:
				return v.i;
			}
		}
		entier64 Entier64() const
		{
			if (Type==TYPE_ENTIER64)
				return v.i64;
			else if (Type==TYPE_ENTIER)
				return (entier64)v.i;
			else if (Type==TYPE_CHANCES)
				return (entier64)v.f;
			else
				throw TXT("impossible de convertir cette valeur en entier 64bits");
		}
		float Flottant() const
		{
			switch (Type)
			{
			case TYPE_CHANCES:
				return v.f;
			case TYPE_CHAINE:
				return v.c->Chances();
			case TYPE_TABLEAU:
				return (float)v.t->l;
			case TYPE_ENTIER64:
				return (float)v.i64;
			default:
				return (float)v.i;
			}
		}
		void SuperPlus(int i)
		{
			switch (Type)
			{
			case TYPE_ENTIER:
				v.i+=i;
				break;
			case TYPE_CHANCES:
				v.f+=i;
				break;
			case TYPE_REFERENCE:
				v.p.Decalage+=i;
				break;
			case TYPE_CARAC:
				v.i+=i;
				break;
			}
		}
		void Convertir(type T)
		{
			if (Type!=T)
			{
				if (T==TYPE_CHANCES)
					EnChances();
				else if (T==TYPE_CHAINE)
					EnChaine();
				else if (T==TYPE_REFERENCE)
				{
					EnEntier();
					int i=v.i;
					Type=TYPE_REFERENCE;
					v.p.Type=RefType::Global;
					if (i<0)
						i=0;
					v.p.Decalage=i;
				}
				else
				{
					EnEntier();
					Type=T;
				}
			}
		}

		valeur operator+(const valeur & a) const
		{
			valeur ret;
			if (Type==TYPE_CHANCES || a.Type==TYPE_CHANCES)
			{
				ret.Type=TYPE_CHANCES;
				ret.v.f=Flottant()+a.Flottant();
			}
			else if (Type==TYPE_ENTIER64 || a.Type==TYPE_ENTIER64)
			{
				ret.Type=TYPE_ENTIER64;
				ret.v.i64=Entier64()+a.Entier64();
			}
			else
			{
				ret.Type=TYPE_ENTIER;
				ret.v.i=Entier()+a.Entier();
			}	
			return ret;
		}
		void operator+=(const valeur & a)
		{
			if (Type==TYPE_CHANCES || a.Type==TYPE_CHANCES)
			{
				v.f=Flottant()+a.Flottant();
				Type=TYPE_CHANCES;
			}
			else
			if (Type==TYPE_ENTIER64 || a.Type==TYPE_ENTIER64)
			{
				v.i64=Entier64()+a.Entier64();
				Type=TYPE_ENTIER64;
			}
			else
			{
				v.i=Entier()+a.Entier();
				Type=TYPE_ENTIER;
			}
		}
		void operator/=(const valeur & a)
		{
			if (Type==TYPE_CHANCES || a.Type==TYPE_CHANCES)
			{
				v.f=Flottant()/a.Flottant();
				Type=TYPE_CHANCES;
			}
			else
			{
				v.i=Entier()/a.Entier();
				Type=TYPE_ENTIER;
			}
		}
		void operator*=(const valeur & a)
		{
			if (Type==TYPE_CHANCES || a.Type==TYPE_CHANCES)
			{
				v.f=Flottant()*a.Flottant();
				Type=TYPE_CHANCES;
			}
			else
			{
				v.i=Entier()*a.Entier();
				Type=TYPE_ENTIER;
			}
		}
		void operator-=(const valeur & a)
		{
			if (Type==TYPE_CHANCES || a.Type==TYPE_CHANCES)
			{
				v.f=Flottant()-a.Flottant();
				Type=TYPE_CHANCES;
			}
			else
			if (Type==TYPE_ENTIER64 || a.Type==TYPE_ENTIER64)
			{
				v.i64=Entier64()-a.Entier64();
				Type=TYPE_ENTIER64;
			}
			else
			{
				v.i=Entier()-a.Entier();
				Type=TYPE_ENTIER;
			}
		}
		valeur operator-(valeur const & a) const
		{
			valeur ret;
			if (Type==TYPE_CHANCES || a.Type==TYPE_CHANCES)
			{
				ret.Type=TYPE_CHANCES;
				ret.v.f=Flottant()-a.Flottant();
			}
			else
			if (Type==TYPE_ENTIER64 || a.Type==TYPE_ENTIER64)
			{
				ret.Type=TYPE_ENTIER64;
				ret.v.i64=Entier64()-a.Entier64();
			}
			else
			{
				ret.Type=TYPE_ENTIER;
				ret.v.i=Entier()-a.Entier();
			}	
			return ret;
		}
		valeur operator/(valeur const & a) const
		{
			valeur ret;
			if (Type==TYPE_CHANCES || a.Type==TYPE_CHANCES)
			{
				ret.Type=TYPE_CHANCES;
				ret.v.f=Flottant()/a.Flottant();
			}
			else
			{
				ret.Type=TYPE_ENTIER;
				int div=a.Entier();
				if (div==0)
					throw "division entière par zéro";
				ret.v.i=Entier()/div;
			}	
			return ret;
		}

		valeur operator*(valeur const & a) const
		{
			valeur ret;
			if (Type==TYPE_CHANCES || a.Type==TYPE_CHANCES)
			{
				ret.Type=TYPE_CHANCES;
				ret.v.f=Flottant()*a.Flottant();
			}
			else
			{
				ret.Type=TYPE_ENTIER;
				ret.v.i=Entier()*a.Entier();
			}	
			return ret;
		}
		int operator==(valeur const & a) const
		{
			if (Type==TYPE_CHANCES || a.Type==TYPE_CHANCES)
			{
				return int(Flottant()==a.Flottant());
			}
			else
			if (Type==TYPE_ENTIER64 || a.Type==TYPE_ENTIER64)
			{
				return int(Entier64()==a.Entier64());
			}
			else
			{
				return int(Entier()==a.Entier());
			}	
		}
		int operator!=(valeur const & a) const
		{
			if (Type==TYPE_CHANCES || a.Type==TYPE_CHANCES)
			{
				return int(Flottant()!=a.Flottant());
			}
			else
			if (Type==TYPE_ENTIER64 || a.Type==TYPE_ENTIER64)
			{
				return int(Entier64()!=a.Entier64());
			}
			else
			{
				return int(Entier()!=a.Entier());
			}	
		}
		int operator>(valeur const & a) const
		{
			if (Type==TYPE_CHANCES || a.Type==TYPE_CHANCES)
			{
				return int(Flottant()>a.Flottant());
			}
			else
			if (Type==TYPE_ENTIER64 || a.Type==TYPE_ENTIER64)
			{
				return int(Entier64()>a.Entier64());
			}
			else
			{
				return int(Entier()>a.Entier());
			}	
		}
		int operator>=(valeur const & a) const
		{
			if (Type==TYPE_CHANCES || a.Type==TYPE_CHANCES)
			{
				return int(Flottant()>=a.Flottant());
			}
			else
			if (Type==TYPE_ENTIER64 || a.Type==TYPE_ENTIER64)
			{
				return int(Entier64()>=a.Entier64());
			}
			else
			{
				return int(Entier()>=a.Entier());
			}	
		}
		int operator!() const
		{
			if (Type==TYPE_CHANCES)
			{
				return int(v.f==0);
			}
			else
			{
				return int(Entier()==0);
			}	
		}
		void operator=(int i)
		{
			Type=TYPE_ENTIER;
			v.i=i;
		}
		void operator=(float f)
		{
			Type=TYPE_CHANCES;
			v.f=f;
		}
		void operator=(const chaineval * c)
		{
			Type=TYPE_CHAINE;
			v.c=c;
		}
		void Oppose()
		{
			if (Type==TYPE_CHANCES)
				v.f=-v.f;
			else
			{
				if (Type==TYPE_CHAINE)
					EnEntier();
				v.i=-v.i;
			}
		}
		void Absolue()
		{
			if (Type==TYPE_CHANCES)
				v.f=fabsf(v.f);
			else
			{
				if (Type==TYPE_CHAINE)
					EnEntier();
				v.i=abs(v.i);
			}
		}

		void EnChaine()
		{
			if (Type!=TYPE_CHAINE)
			{
				v.c=Chaine();
				Type=TYPE_CHAINE;
			}
		}
		void EnTableau()
		{
			if (Type!=TYPE_TABLEAU)
			{
				Type=TYPE_TABLEAU;
				v.t=new tableauval();
			}
		}
		void EnTableHachage()
		{
			if (Type!=TYPE_HACHAGE)
			{
				Type=TYPE_HACHAGE;
				v.h=new hachageval();
			}
		}

		void EnNombre()
		{
			if (Type!=TYPE_ENTIER && Type!=TYPE_CHANCES)
				EnEntier();
		}
		void EnEntier()
		{
			if (Type!=TYPE_ENTIER)
			{
				v.i=Entier();
				Type=TYPE_ENTIER;
			}
		}
		void EnChances()
		{
			if (Type!=TYPE_CHANCES)
			{
				v.f=Flottant();
				Type=TYPE_CHANCES;
			}
		}
		void EcrireBin(FILE *F);
		void Ecrire(FILE * F);
		void EcrireU(FILE * F);

		//friend std::basic_ostream<carac>& operator<<(std::basic_ostream<carac> &o,const valeur &v);
	};
}
#endif
