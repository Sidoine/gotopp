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
					 
Classe "chaine", stockant une chaîne de caractères du même style
que string
*/

#ifndef _CHAINE_H
#define _CHAINE_H

namespace GotoPP
{
	class chaineval:public IChaine
	{
	public:
		static chaineval * Creer(size_t longueur)
		{
			chaineval * ret=(chaineval*) GC_MALLOC_ATOMIC((longueur+1)*sizeof(carac)+sizeof(size_t));
			ret->l=longueur;
			return ret;
		}

		static chaineval * Creer(const carac * c, size_t l)
		{
			chaineval * v=Creer(l);
			memcpy(v->c,c,l*sizeof(carac));
			v->c[l]=0;
			return v;
		}

		static chaineval * Creer(const carac * c)
		{
			return Creer(c,czlen(c));
		}
		
#ifdef _UNICODE
		static chaineval * Creer(const char * a);
		static chaineval *Creer(const char * a, size_t l);
#else
		static chaineval *Creer(const wchar_t *a);
#endif

		static chaineval * CreerFormat(const carac *c, size_t l);
		static chaineval * Creer(const chaineval * a, const chaineval * b)
		{
			size_t l=a->l+b->l;
			chaineval * v=chaineval::Creer(l);
			memcpy(v->c,a->c,a->l*sizeof(carac));
			memcpy(v->c+a->l,b->c,b->l*sizeof(carac));
			v->c[l]=0;
			return v;
		}

		static chaineval * CreerEntier(int i);
		static chaineval * CreerCaractere(carac c);
		static chaineval * CreerFlottant(float f);
		static chaineval * Copier(const chaineval * c)
		{
			return Creer(c->c,c->l);
		}
			
		int Entier() const
		{
			return cztoi(c);
		}
		float Chances() const
		{
			return float(cztof(c));
		}

		static void Construire(chaineval *&chaine, const chaineval * r, size_t a)
		{
			Construire(chaine,r->c,r->l,a);
		}
		static void Construire(chaineval *&chaine, const carac * c, size_t l, size_t a);
		static void Finaliser(chaineval *&chaine);
	};
}
#endif
