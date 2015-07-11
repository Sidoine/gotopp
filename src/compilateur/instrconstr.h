/*


	Instruction : un bout de code du programme créé par
	un appel à Compilateur::Compiler
*/

#ifndef _INSTRCONST_H
#define _INSTRCONST_H

namespace GotoPP
{
	class BoutCode;

	class InstrConstr
	{
	public:
		code * i;//Position actuelle
		code * c;//Départ
		size_t t,a;//Taille, taille allouée
		
		BoutCode * CreerBoutCode();
		InstrConstr()
		{
			c=NULL;
			t=a=0;
			i=NULL;
		}
		void Reinit();
		void Etendre()
		{
			if (t==a)
			{
	//			clog << "Tableau des instructions étendu\n";
				a+=1024;
				i=i-size_t(c);
				c=(code*)realloc(c,a);
				i=i+size_t(c);
			}
		}
		void Etendre(size_t l)
		{
			if (t+l>a)
			{
	//			clog << "Tableau des instructions étendu de l\n";
				a+=1024+l;
				i=i-size_t(c);
				c=(code*)realloc(c,a);
				i=i+size_t(c);
			}
		}
		void Ajouter(valeur & v)
		{
			switch(v.Type)
			{
			case TYPE_CHANCES:
				AjouterChances(v.v.f);
				break;
			case TYPE_CODE:
				AjouterCode(code(v.v.i));
				break;
			case TYPE_TYPE:
				AjouterType(type(v.v.i));
				break;
			case TYPE_CARAC:
				AjouterCarac(carac(v.v.i));
				break;
			case TYPE_CHAINE:
				AjouterChaine(v.v.c);
				break;
			case TYPE_OBJETC:
				AjouterObjetC(v.v.o);
				break;
			case TYPE_ETIQLOC:
			case TYPE_ETIQUETTE:
			case TYPE_ETIQUETTE_THIS:
				AjouterSymbole(v.v.ptr);
				break;
			case TYPE_FONCTIONC:
			case TYPE_FONCTIONC_THIS:
				AjouterFonctionC(v.v.fonctionC);
				break;
			default:
				AjouterEntier(v.v.i);
			}
			v.Effacer();
		}
		void AjouterSymbole(Symbole * s)
		{
			Etendre(sizeof(Symbole*));
			*((Symbole**)i)=s;
			i+=sizeof(Symbole*);
			t+=sizeof(Symbole*);
		}
		void AjouterFonctionC(voidf f)
		{
			Etendre(sizeof(f));
			*((voidf*)i)=f;
			i+=sizeof(f);
			t+=sizeof(f);
		}

		void AjouterObjetC(IObjet * e)
		{
			//TODO: garder le pointeur, pour la sérialisation du code
			Etendre(sizeof(e));
			*((IObjet**)i)=e;
			i+=sizeof(e);
			t+=sizeof(e);		
		}
		void AjouterChaine(const chaineval * c)
		{
			size_t longueur=(c->l+1)*sizeof(carac)+sizeof(size_t);
			Etendre(longueur);
			t+=longueur;
		
			*((size_t*)i)=c->l;
			i+=sizeof(size_t);
			memcpy(i,c->c,(c->l+1)*sizeof(carac));
			i+=(c->l+1)*sizeof(carac);
		}

		void AjouterPointeur(char *c)
		{
			Etendre(sizeof(char*));
			*((char**)i)=c;
			i+=sizeof(char*);
			t+=sizeof(char*);
		}

		void AjouterCarac(carac k)
		{
			Etendre();
			*((carac*)i)=k;
			i+=sizeof(carac);
			t+=sizeof(carac);
		}
		void AjouterEntier(int j)
		{
			Etendre(sizeof(int));
			*((int*)i)=j;
			i+=sizeof(int);
			t+=sizeof(j);
		}
		void AjouterGotoRel(int j)
		{
			AjouterEntier(j-(int)t-sizeof(int));
		}
		void AjouterChances(float f)
		{
			Etendre(sizeof(float));
			*((float*)i)=f;
			i+=sizeof(float);
			t+=sizeof(f);
		}
		void AjouterType(type t)
		{
			AjouterCode(t);
		}
		void AjouterCode(code c)
		{
			Etendre();
			*((code*)i)=c;
			i+=sizeof(code);
			t+=sizeof(code);
		}
		void AjouterFonction(int c)
		{
			AjouterEntier(c);
		}
		void Debut()
		{
			i=c;
		}
		void ChgGotoRel(int ou, int cible)
		{
			*((int*)&c[ou])=cible-ou-sizeof(int);
		}
	};
}

#endif
