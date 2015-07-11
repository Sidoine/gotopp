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
					 
De supers fonctions qui permettent de sauver ou de charger des
variables dans un fichier.
*/
#include <gotopp/base.h>
#include <gotopp/erreur.h>
#include <gotopp/programme.h>

#include "sauveur.h"

using namespace GotoPP;
using namespace GotoPP;

//Enregistre le contenu de la mémoire dans un fichier
void valeur::Charger(FILE *F)
{
	bool Nouveau=false;
	fread(&Type,sizeof(Type),1,F);

	switch(Type)
	{
	case TYPE_INCONNU:
	case TYPE_DEBUTPARAM:
		break;
	case TYPE_FONCTIONC:
	case TYPE_CARAC:
	case TYPE_VARIABLE:
	case TYPE_ENTIER:
	case TYPE_ETIQUETTE:
	case TYPE_FONCTIONMODULE:
		fread(&v.i,sizeof(int),1,F);
		break;
	case TYPE_CHANCES:
		fread(&v.f,sizeof(float),1,F);
		break;
	case TYPE_ENTIER64:
		fread(&v.i64,sizeof(entier64),1,F);
		break;
	case TYPE_CHAINE:
		{
			fread(&v.c,sizeof(v.c),1,F);
			int l;
			fread(&l,sizeof(l),1,F);//Pour pouvoir allouer la bonne taille
			Nouveau=Pointeur.Nouveau((void*&)v.c,TYPE_CHAINE,sizeof(size_t)+(l+1)*sizeof(carac));
			break;
		}
	case TYPE_TABLEAU:
		fread(&v.t,sizeof(v.t),1,F);
		Nouveau=Pointeur.Nouveau((void*&)v.t,TYPE_TABLEAU,sizeof(tableauval));
		break;
	case TYPE_REFERENCE:
		fread(&v.p.Type,sizeof(v.p.Type),1,F);
		if (v.p.Type!=RefType::Global && v.p.Type!=RefType::Local)
			fread(&v.p.Base,sizeof(v.p.Base),1,F);
		if (v.p.Type!=RefType::Bebe)
			fread(&v.p.Decalage,sizeof(v.p.Decalage),1,F);
		switch(v.p.Type)
		{
		case RefType::Tableau:
			Nouveau=Pointeur.Nouveau((void*&)v.p.Base,TYPE_TABLEAU,sizeof(tableauval));
			break;
		case RefType::Hachage:
			Nouveau=Pointeur.Nouveau((void*&)v.p.BaseH,TYPE_HACHAGE,sizeof(hachageval));
			break;
		case RefType::Bebe:
			Nouveau=Pointeur.Nouveau((void*&)v.p.BaseB,TYPE_BEBE,sizeof(valeur));
			break;
		}
		break;
	case TYPE_HACHAGE:
		fread(&v.h,sizeof(v.h),1,F);
		Nouveau=Pointeur.Nouveau((void*&)v.h,TYPE_HACHAGE,sizeof(hachageval));
		break;
	default:
		throw "type de variable à charger non supporté";
	}
	if (Nouveau)
		Pointeur.Dernier()->Charger(F);
}

void valeur::Sauver(FILE *F)
{
	bool Nouveau=false;
	fwrite(&Type,sizeof(Type),1,F);
	switch(Type)
	{
	case TYPE_INCONNU:
	case TYPE_DEBUTPARAM:
		break;
	case TYPE_FONCTIONC:
	case TYPE_CARAC:
	case TYPE_VARIABLE:
	case TYPE_ENTIER:
	case TYPE_ETIQUETTE:
	case TYPE_FONCTIONMODULE:
		fwrite(&v.i,sizeof(int),1,F);
		break;
	case TYPE_ENTIER64:
		fwrite(&v.i64,sizeof(entier64),1,F);
		break;
	case TYPE_CHANCES:
		fwrite(&v.f,sizeof(float),1,F);
		break;
	case TYPE_CHAINE:
		fwrite(&v.c,sizeof(v.c),1,F);
		fwrite(&v.c->l,sizeof(v.c->l),1,F);//Pour pouvoir allouer la bonne taille
		Nouveau=Pointeur.Nouveau((void*)v.c,TYPE_CHAINE);
		break;
	case TYPE_TABLEAU:
		fwrite(&v.t,sizeof(v.t),1,F);
		Nouveau=Pointeur.Nouveau((void*)v.t,TYPE_TABLEAU);
		break;
	case TYPE_REFERENCE:
		fwrite(&v.p.Type,sizeof(v.p.Type),1,F);
		if (v.p.Type!=RefType::Global && v.p.Type!=RefType::Local)
			fwrite(&v.p.Base,sizeof(v.p.Base),1,F);
		if (v.p.Type!=RefType::Bebe)
			fwrite(&v.p.Decalage,sizeof(v.p.Decalage),1,F);
		switch(v.p.Type)
		{
		case RefType::Tableau:
			Nouveau=Pointeur.Nouveau((void*)v.p.Base,TYPE_TABLEAU);
			break;
		case RefType::Hachage:
			Nouveau=Pointeur.Nouveau((void*)v.p.BaseH,TYPE_HACHAGE);
			break;
		case RefType::Bebe:
			Nouveau=Pointeur.Nouveau((void*)v.p.BaseB,TYPE_BEBE);
			break;
		}
		break;
	case TYPE_HACHAGE:
		fwrite(&v.h,sizeof(v.h),1,F);
		Nouveau=Pointeur.Nouveau((void*)v.h,TYPE_HACHAGE);
		break;
	default:
		throw CErreur(TXT("la sauvegarde du type %s n'est supportée"),
			NomType(Type));
	}
	if (Nouveau)
		Pointeur.Dernier()->Sauver(F);
}

void SPointeur::Sauver(FILE * F)
{
	fwrite(&Type,sizeof(Type),1,F);
	switch (Type)
	{
	case TYPE_TABLEAU:
		{
			fwrite(&t->l,sizeof(t->l),1,F);
			for (index_t v=0; v<t->l;v++)
				((valeur*)t->v)[v].Sauver(F);
			break;
		}
	case TYPE_HACHAGE:
		{
			fwrite(&h->l,sizeof(h->l),1,F);
			fwrite(h->Index,sizeof(h->Index[0]),h->l,F);
			pairehachage *k=(pairehachage*)h->v;
			for (index_t v=0; v<h->l;v++)
			{
				size_t lc=k->Clef->l;
				fwrite(&lc,sizeof(lc),1,F);
				fwrite(k->Clef,lc*sizeof(carac),1,F);
				k->Val.Sauver(F);
				k++;
			}
			break;
		}
	case TYPE_CHAINE:
		{
			fwrite(&c->l,sizeof(c->l),1,F);
			fwrite(c->c,c->l,sizeof(carac),F);
			break;
		}
	/*case TYPE_EXPREG:
		{
			fwrite(&er->Longueur,sizeof(er->Longueur),1,F);
			fwrite(&er->Chaine,sizeof(er->Chaine),1,F);
			fwrite((void*)&er->Chaine->l,sizeof(int),1,F);
			if (Pointeur.Nouveau((void*)er->Chaine,TYPE_CHAINE))
				Pointeur.Dernier()->Sauver(F);
			for (uint i=0; i<NSUBEXP; i++)
			{
				int s,f;
				if (er->startp[i]==NULL)
					s=-1;
				else
					s=int(er->startp[i]-((chaineval*)er->Chaine)->c);
				if (er->endp[i]==NULL)
					f=-1;
				else
					f=int(er->endp[i]-((chaineval*)er->Chaine)->c);
				fwrite(&s,sizeof(s),1,F);
				fwrite(&f,sizeof(f),1,F);
			}
			fwrite(&er->regstart,sizeof(er->regstart),1,F);
			fwrite(&er->reganch,sizeof(er->reganch),1,F);
			fwrite(&er->regmust,sizeof(er->regmust),1,F);
			fwrite(&er->regmlen,sizeof(er->regmlen),1,F);
			fwrite(er->program,er->Longueur*sizeof(carac)-sizeof(regexp)+sizeof(carac),1,F);
			break;
		}*/
	case TYPE_BEBE:
		{
			b->Sauver(F);
			break;
		}
	default:
		throw "type de pointeur à sauver non supporté";
	}
}

void SPointeur::Charger(FILE * F)
{
	fread(&Type,sizeof(Type),1,F);
	switch (Type)
	{
	case TYPE_TABLEAU:
		{
			fread(&ici.t->l,sizeof(t->l),1,F);
			ici.t->v=(valeur*)malloc(sizeof(valeur)*ici.t->l);
			for (index_t v=0; v<ici.t->l;v++)
				((valeur*)ici.t->v)[v].Charger(F);
			break;
		}
	case TYPE_HACHAGE:
		{
			fread(&ici.h->l,sizeof(h->l),1,F);
			ici.h->a=ici.h->l;
			ici.h->Index=(indexhachage*)malloc(sizeof(indexhachage)*ici.h->l);
			fread(ici.h->Index,sizeof(h->Index[0]),h->l,F);
			ici.h->v=(pairehachage*)malloc(sizeof(pairehachage)*ici.h->l);
			pairehachage *k=(pairehachage*)ici.h->v;
			for (index_t v=0; v<ici.h->l;v++)
			{
				size_t lc;
				fread(&lc,sizeof(lc),1,F);
				k->Clef=chaineval::Creer(lc);
				fread((void*)k->Clef->c,lc*sizeof(carac),1,F);
				k->Val.Charger(F);
				k++;
			}
			break;
		}
	case TYPE_CHAINE:
		{
			fread(&ici.c->l,sizeof(c->l),1,F);
			fread(ici.c->c,ici.c->l,sizeof(carac),F);
			ici.c->c[ici.c->l]=0;
			break;
		}
/*	case TYPE_EXPREG:
		{
			fread(&er->Longueur,sizeof(er->Longueur),1,F);
			fread(&er->Chaine,sizeof(er->Chaine),1,F);
			int l;
			fread(&l,sizeof(int),1,F);
			if (Pointeur.Nouveau((void*&)er->Chaine,TYPE_CHAINE,sizeof(size_t)*2+(l+1)*sizeof(carac)))
				Pointeur.Dernier()->Charger(F);
			for (uint i=0; i<NSUBEXP; i++)
			{
				int s,f;
				fread(&s,sizeof(s),1,F);
				fread(&f,sizeof(f),1,F);
				if (s==-1)
					er->startp[i]=NULL;
				else
					er->startp[i]=&((chaineval*)er->Chaine)->c[s];
				if (f==-1)
					er->endp[i]=NULL;
				else
					er->endp[i]=&((chaineval*)er->Chaine)->c[f];
			}
			fread(&er->regstart,sizeof(er->regstart),1,F);
			fread(&er->reganch,sizeof(er->reganch),1,F);
			fread(&er->regmust,sizeof(er->regmust),1,F);
			fread(&er->regmlen,sizeof(er->regmlen),1,F);
			fread(er->program,er->Longueur*sizeof(carac)-sizeof(regexp)+sizeof(carac),1,F);
			break;
		}*/
	case TYPE_BEBE:
		{
			ici.b->Charger(F);
			break;
		}
	default:
		throw "type de pointeur à charger non supporté";
	}
}

void GotoPP::InitSauveur()
{
	Pointeur.l=0;
}
