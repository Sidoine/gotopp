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
					 
Alors là c'est vraiment du beau boulot. Ca permet d'optimiser le
code.
*/
#include <gotopp/base.h>
#include <gotopp/compilateur.h>
#include <gotopp/erreur.h>
#include <gotopp/global.h>

#include "compriv.h"


using namespace GotoPP;

namespace GotoPP
{
	extern DescCode descCode[];
}

/*Ce tableau sert à vérifier rapidement si il y a suffisement
de paramètres dans la pile. Par exemple l'addition a besoin
de deux valeurs. Si elles n'y sont pas, le compilateur renverra
une erreur.*/

GotoPP::uint nMinParametres[]=
{
	0,//I_STOP,
	2,//I_DEFINIR,
	2,//I_AJOUTER,
	0,//I_CONSTANTE,
	0,//I_VARIABLE,
	0,//I_VALEURVAR,
	0,//I_DEBUTLIGNE,
	0,//I_CHANCES,
	2,//I_SOUSTRAIRE,
	1,//I_CONVTYPE,
	2,//I_FOIS,
	2,//I_DIVISE,
	1,//I_VALEUR,
	1,//I_OPPOSE,
	0,//I_RETOUR,
	2,//I_CONCAT,
	1,//I_TABLEAUREF,
	1,//I_TABLEAUVAL,
	0,//I_CTABLEAUREF,
	0,//I_CTABLEAUVAL,
	0,//I_DEBUTPARAM,
	1,//I_DUPLIQUER,
	1,//I_SUPPRIMER,
	0,//I_VARIABLELOC,
	0,//I_VALEURVARLOC,
	0,//I_ETIQUETTE,
	0,//I_GOTO,
	1,//I_GOTONONZERO,
	2,//I_HACHAGEREF,
	2,//I_HACHAGEVAL,
	2,//I_SUPERPLUS,
	2,//I_INCREMENTER,
	2,//I_DECREMENTER,
	2,//I_DIVISEREGAL,
	2,//I_MULTIPLIEREGAL,
	1,//I_TAILLE,
	1,//I_VALEURCLEF,
	0,//I_THISREF,
	0,//I_THISVAL,
	0,//I_PROBASAUTERLIGNE,
	0,//I_NOUVEAU,
	2,//I_INSTANCEDE,
	0,//I_GOTOTACHE,
	0,//I_FINTACHE,
	2,//I_EGAL,
	2,//I_DIFFERENT,
	2,//I_INFERIEUR,
	2,//I_SUPERIEUR,
	2,//I_INFEGAL,
	2,//I_SUPEGAL,
	1,//I_NON,
	1,//I_GOTOZERO,
	0,//I_DEFCONSTR,
	0,//I_THIS,
	1,//I_NONOPPOSE,
	0,//I_ENERREUR,
	1,//I_ERREUR,
	1,//I_OU,
	0,//I_FINOUET,
	1,//I_ET,
	0,//I_ENTIER,
	1,//I_TYPEDE,
	1,//I_SWITCH,
	0,//I_PARAMREF,
	0,//I_PARAMVAL,
	1,//I_ESTREFVALIDE,
	1,//I_EMPILER2,
	0,//I_DEPILER2,
	0,//I_VALEUR2,
	2,//I_DECALG,
	2,//I_DECALD,
	2,//I_MODULO,
	2,//I_ETBIN,
	2,//I_OUBIN,
	1,//I_EXISTE,
	0,//I_NPARAMS,
	2,//I_AJOUTERE,
	2,//I_SOUSTRAIREE,
	2,//I_FOISE,
	2,//I_DIVISERE
	0,//I_CARAC
	0,//GOTOR
	1,//GOTOZEROR
	1,//GOTONONZEROR
	0,//GOTOTACHER
	0,//ETIQUETTELOC{"GOTOR",1,{TYPE_ENTIER}},
	0,//ENERREURR
	1, //I_CONSTVAR
	0,//I_EXEC
	1,//I_EXECVAL,
	0,//I_EXECC
	1,//I_EXECTHIS
	1,//I_EXECMEMETHISVAL,
	1,//I_EXECTHISC,
	1,//I_EXECTHISMODULE,
	0,//I_FONCTIONC,
	0,//I_FONCTIONMODULE
	0, //I_ETIQUETTE_THIS
	0, //I_DEPLACERHAUTPILE
	0, //I_ALLOCATION
	0, //I_FONCTIONC_THIS
	0, //I_EXPREG
};



carac * NomOpVV[]=
{
	_T("+"),_T("-"),_T("*"),_T("/"),_T("%"),_T("_"),
		_T("eg"),_T("diff"),_T("|+"),_T("|*"),_T("|<"),_T("|>")
};

SInstr & LigneDeCode::Inserer(uint pos, uint nvals)
{
	if (nIns==NMAXINSTRPARLIGNE || nVals+nvals>NMAXINSTRPARLIGNE)
		throw "nombre maximal d'instructions par ligne atteint";
	
	uint instr=pos+1;
	//On décale les instructions qui suivent
	uint v;
	if (instr<nIns)
	{
		//On décale aussi les valeurs de nvals
		v=ins[instr].pVal;
		if (v<nVals)
		{
			memmove(&val[v+nvals],&val[v],(nVals-v)*sizeof(val[0]));
			for (uint i=instr; i<nIns; i++)
				ins[i].pVal+=nvals;
		}
		memmove(&ins[instr+1],&ins[instr],(nIns-instr)*sizeof(ins[0]));
	}
	else
		v=nVals;

	ins[instr].pVal=v;
	ins[instr].nVal=nvals;
	nIns++; nVals+=nvals;
	return ins[instr];
}

SInstr & LigneDeCode::Ajouter(uint nvals)
{
	ins[nIns].pVal=nVals;
	ins[nIns].nVal=nvals;
	nVals+=nvals;
	return ins[nIns++];
}

void Compilateur::InsererChangementType(int posPile, Symbole * nouveauType)
{
	if (PileType[posPile].l>1 || !PileType[posPile].t[0]->IsPod()
		|| !nouveauType->IsPod())
		throw TXT("changement de type automatique impossible");

	SInstr &i=insTri.Inserer(PileType[posPile].instruction,2);

	i.Code=I_CONVTYPE;
	insTri.val[i.pVal].Type=TYPE_TYPE;
	insTri.val[i.pVal].v.i=PileType[posPile].Dernier()->pod;
	insTri.val[i.pVal+1].Type=TYPE_TYPE;
	insTri.val[i.pVal+1].v.i=nouveauType->pod;
}

/*
Cette fonction traite les instructions stockées dans
le tableau Instr. Elle prend les instructions les unes
après les autres et les met en sortie dans le code
final, dans InsTri. Instr est accompagné de Valeur, 
qui contient toutes les valeurs associées aux
instructions. Par exemple I_ENTIER est associé à un entier
dans le tableau Valeur. v est l'indice de la première valeur
associée à i (=>Valeur[v].)
Parfois on aura à optimiser en tenant compte des instructions
déjà dans InsTri (nInsTri-1 et nInsTri-2). Par commodité
Val1 et Val2 sont les premières valeurs associées à ces
instructions.
*/
void Compilateur::AjouterInstruction(uint i)
{
	if (Instr[i].Special==SPE_NOOP)
		return;
	//On prend dans Instr/Valeur et on rajoute dans
	//InsTri/ValTri
	ASSERT(Instr[i].Special==SPE_NORMAL);
	uint v=Instr[i].pVal;
/*	ASSERT(Valeur[v].Type==TYPE_CODE || Valeur[v].Type==TYPE_OPERATEURVV
			|| Valeur[v].Type==TYPE_OPERATEURRV || Valeur[v].Type==TYPE_OPERATEURV);*/

//	if (!PasOptimiser)
//	{

	/*if (Valeur[v].Type==TYPE_OPERATEURVV)
	{
		int op=Instr[i].Code;
		if (PosPileType<2)
			throw CErreur("l'opérateur binaire %s doit avoir deux paramètres",NomOpVV[op]);
		PosPileType-=2;
		type t1=PileType[PosPileType].LireClasse();
		TypeAvecRef t2=PileType[PosPileType+1];
		//On recherche si cet opérateur est défini
		for (index_t o=0; o<Classe[t1].io.nOpVV; o++)
			if (Classe[t1].io.OpVV[o].Op==op && Classe[t1].io.OpVV[o].Parametre==t2)
			{
				switch(Classe[t1].io.OpVV[o].Type)
				{
				case TYPE_CODE:
					{
						InsTri[nInsTri].nVal=1;
						InsTri[nInsTri++].pVal=nValTri;
						ValTri[nValTri].Type=TYPE_CODE;
						ValTri[nValTri++].v.i=Classe[t1].io.OpVV[o].Val;
						PileType[PosPileType++]=Classe[t1].io.OpVV[o].Retour;
						break;
					}
				}
				return;
			}
		throw CErreur("l'opérateur binaire %s n'est pas défini pour les types %s et %s",NomOpVV[op],
				Classe[t1].Nom,Classe[t2.Classe].Nom);
	}
	else*/
	//{
		
	uint Val1;
	if (insTri.nIns>=1)
		Val1=insTri.ins[insTri.nIns-1].pVal;
	uint Val2;
	if (insTri.nIns>=2)
		Val2=insTri.ins[insTri.nIns-2].pVal;

	int Code=Instr[i].Code;
	if (PosPileType<nMinParametres[Code])
		throw CErreur(TXT("le code %s n'a pas assez de paramètres"),descCode[Code].Nom);

	bool aRetour=true;

	//TODO: globalement tout ça est très pourri
	switch(Code)
	{
	case I_DEFINIR:
		{
			PosPileType-=2;
			aRetour=false;
			break;
		}
	case I_EXEC:
	case I_EXECVAL:
	case I_EXECC:
	case I_EXECTHIS:
	case I_EXECMEMETHISVAL:
	case I_EXECTHISC:
	case I_EXECTHISMODULE:
		{
			CType * t=&PileType[--PosPileType];
			Symbole * c=t->Depiler();
			if (c==symboleValeur)
			{
				//Fonction inconnue, retourne une valeur
				while (!PileType[PosPileType].EstType(symboleDebutParams))
				{
					if (PosPileType==0)
						throw TXT("interne : pas de début de paramètre");
					PosPileType--;
				}
				PileType[PosPileType++]=symboleValeur;
			}
			else
			{
				if (c!=symboleFonctionC && c!=symboleFonctionGPP && c!=symboleMethodeGPP
					&& c!=symboleMethodeC)
					throw TXT("fonction attendue pour un I_EXEC");

				Symbole * d;
				int nparams=1;
				//Les paramètres définis
				do
				{
					if (t->Dernier()==symboleEtc)
						break;
					//Le paramètre suivant
					CType *u=&PileType[--PosPileType];
					uint al=u->l;
					ASSERT(PosPileType>=0);
					//On compare les références/tableaux/hachage
					bool conversionPossible=true;
					do
					{
						c=t->Depiler();
						if (c!=symboleTableHachage && c!=symboleReference && c!=symboleTableau)
							break;
						conversionPossible=false;
						d=u->Depiler();
						if (c!=d)
							throw CErreur(TXT("tableau ou référence ou hachage attendu"));
					}
					while (u->l);
					if (!u->l)
						throw CErreur(TXT("type de variable incorrect pour le parametre %d"),
							nparams);
					
					//On compare les deux types
					d=u->Depiler();
					u->l=al;
					if (c!=d)
					{
						if (conversionPossible)
							InsererChangementType(PosPileType,c);
						else
							CErreur(TXT("changement de type impossible quand on passe des références pour le paramètre %d"),
								nparams);
					}
					nparams++;
				}
				while (c!=symboleDebutParams && d!=symboleDebutParams);

				//Les paramètres des fonctions variables
				if (t->Dernier()==symboleEtc)
				{
					t->Depiler();
					c=t->Depiler();
					while(1)
					{
						d=PileType[--PosPileType].Depiler();
						ASSERT(PosPileType>=0);
						if (d==symboleDebutParams)
							break;
						if (c!=symboleValeur && d!=c)
							throw CErreur(TXT("le type du paramètre %d de la fonction est incorrect"),
								nparams);
					}
					c=t->Depiler();
				}
					
				if (c!=d)
				{
					if (c==symboleDebutParams)
						throw TXT("trop de paramètres passés à la fonction");
					else
						throw TXT("pas assez de paramètres passés à la fonction");
				}
				//Enfin on empile le type du retour
				if (t->l)
					PileType[PosPileType++]=*t;
				else
					aRetour=false;

				/*{
					do
					{
						PosPileType--;
					}
					while (PosPileType && !PileType[PosPileType].EstType(TYPE_DEBUTPARAM));
					if (!PileType[PosPileType].EstType(TYPE_DEBUTPARAM))
						throw TXT("y'a eu un problème quelque part, y'a plus l'initiateur de paramètres");
					PileType[PosPileType++].Def(TYPE_PINGOUIN);
				}*/
			}
			break;
		}
	/*case I_EXECTHISC:
		{
			PileType[PosPileType++].Def(TYPE_PINGOUIN);
			break;
		}*/
	
	case I_CONSTANTE:
		{
			PileType[PosPileType++].Def(Valeur[v].v.ptr);
			break;
		}
	case I_EXPREG:
		{
			PileType[PosPileType++].Def(symboleExpReg);
			break;
		}
	case I_VARIABLE:
		{
			PileType[PosPileType]=Valeur[v].v.ptr->TypeAc;
			//Valeur[v].v.i=GetSymbole(Valeur[v].v.i).indice;
			PileType[PosPileType++].Empiler(symboleReference);
			//AfficherType(PileType[PosPileType-1]);
			break;
		}
	case I_VALEURVAR:
		{
			PileType[PosPileType++]=Valeur[v].v.ptr->TypeAc;
			//Valeur[v].v.i=GetSymbole(Valeur[v].v.i).indice;
			break;
		}
	case I_CHANCES:
		{
			PileType[PosPileType++].Def(symboleFlottant);
			break;
		}
	case I_ENTIER:
		{
			PileType[PosPileType++].Def(symboleEntier);
			break;	
		}
	case I_DIVISE:
	case I_FOIS:
	case I_SOUSTRAIRE:
	case I_AJOUTER:
		{
			if (PosPileType<2)
				throw TXT("pas assez de paramètres pour l'opérateur");
			PosPileType--;

			//Si les deux instructions précédentes étaient constantes
			//on les remplace par la bonne valeur
			if (insTri.nIns>=2
				&&
				(insTri.ins[insTri.nIns-2].Code==I_ENTIER
				|| insTri.ins[insTri.nIns-2].Code==I_CHANCES)
				&&
				(insTri.ins[insTri.nIns-1].Code==I_ENTIER
				|| insTri.ins[insTri.nIns-1].Code==I_CHANCES))
			{
				//On fait l'opération
				switch(Code)
				{
				case I_AJOUTER:
					insTri.val[Val2]+=insTri.val[Val1];
					break;
				case I_SOUSTRAIRE:
					insTri.val[Val2]-=insTri.val[Val1];
					break;
				case I_DIVISE:
					insTri.val[Val2]/=insTri.val[Val1];
					break;
				case I_FOISE:
				case I_FOIS:
					insTri.val[Val2]*=insTri.val[Val1];
					break;
				}	
				//On supprime la dernière instruction
				insTri.nVals=Val1;
				insTri.nIns--;
				//On remplace l'avant-dernière par la bonne
				if (insTri.val[Val2].Type==TYPE_ENTIER)
					insTri.ins[insTri.nIns-1].Code=I_ENTIER;
				else
					insTri.ins[insTri.nIns-1].Code=I_CHANCES;
				return;
			}

	/*		if (!PileType[PosPileType].EstType(TYPE_ENTIER))
				InsererChangementType(PosPileType,TYPE_ENTIER);
			if (!PileType[PosPileType-1].EstType(TYPE_ENTIER))
				InsererChangementType(PosPileType-1,TYPE_ENTIER);*/
			
			//On choisit le bon opérateur
			//TODO: C'est absolument pourri en l'état
			if (PileType[PosPileType].EstType(symboleEntier))
			{
				Symbole * symb=PileType[PosPileType-1].Dernier();
				
				if (symb==symboleFlottant)
					PileType[PosPileType-1].Def(symboleFlottant);
				else if (symb==symboleEntier)
				{
					PileType[PosPileType-1].Def(symboleEntier);
					switch (Code)
					{
					case I_AJOUTER:
						Instr[i].Code=I_AJOUTERE;
						break;
					case I_SOUSTRAIRE:
						Instr[i].Code=I_SOUSTRAIREE;
						break;
					case I_FOIS:
						Instr[i].Code=I_FOISE;
						break;
					case I_DIVISE:
						Instr[i].Code=I_DIVISERE;
						break;
					}
				}
				else
					PileType[PosPileType-1].Def(symboleValeur);
			}
			else if (PileType[PosPileType].EstType(symboleFlottant))
			{
				if (PileType[PosPileType].l==1)
				{
					Symbole * symb=PileType[PosPileType-1].Dernier();
					if (symb==symboleFlottant || symb==symboleEntier)
						PileType[PosPileType-1].Def(symboleFlottant);
					else
						PileType[PosPileType-1].Def(symboleValeur);
				}
				else
					PileType[PosPileType-1].Def(symboleValeur);
			}	
					
			
			break;		
		}
	case I_CONVTYPE:
		{
			PileType[PosPileType-1].Def(symboleValeur);
			if (insTri.nIns)
			{
				if (Valeur[v+1].v.i==TYPE_ENTIER)
				{
					if (insTri.ins[insTri.nIns-1].Code==I_CARAC)
					{
						insTri.ins[insTri.nIns-1].Code=I_ENTIER;
						insTri.val[Val1].Type=TYPE_ENTIER;
						PileType[PosPileType-1].Def(symboleEntier);
						return;
					}
				}
			}					
			break;
		}
	case I_VALEUR:
		{
			if (!PileType[PosPileType-1].EstType(symboleValeur))
			{
				if (PileType[PosPileType-1].Depiler()!=symboleReference)
					throw TXT("on doit prendre la valeur d'une référence");
			}
			break;
		}
	case I_NONOPPOSE:
	case I_OPPOSE:
		{
			if (!PileType[PosPileType-1].EstType(symboleValeur))
			{
				if (!PileType[PosPileType-1].EstType(symboleFlottant))
					PileType[PosPileType-1].Def(symboleEntier);
			}
			break;
		}
	case I_RETOUR:
		{
			if (PosPileType)
				PosPileType--;
			aRetour=false;
			break;
		}
	case I_CONCAT:
		{
			PosPileType--;
			PileType[PosPileType-1].Def(symboleChaine);
			break;
		}
	case I_TABLEAUREF:
	case I_TABLEAUVAL:
		{
			Symbole * c=PileType[PosPileType-1].Depiler();
			if (c!=symboleValeur && c!=symboleEntier)
				throw TXT("entier attendu comme indice de tableau");

			PosPileType--;
			
			if (!PileType[PosPileType-1].EstType(symboleValeur))
			{
				Symbole * ty=PileType[PosPileType-1].Depiler();
				if (ty!=symboleReference)
					throw TXT("référence sur un tableau attendue");
				ty=PileType[PosPileType-1].Depiler();
				if (ty==symboleTableau)
				{
				}
				else if (ty==symboleValeur)
				{
					PileType[PosPileType-1].Def(symboleValeur);
				}
				else if (!ty->IsPod())
				{
					//PileType[PosPileType-1]=Classe[ty].TypeTableau;
					//PileType[PosPileType-1]=
					//throw TXT("accès par indice aux membres d'un troupeau non implémenté");
					PileType[PosPileType-1].Def(symboleValeur);
				}
				else
					throw TXT("tableau attendu");
			}
			if (Code==I_TABLEAUREF)
				PileType[PosPileType-1].Empiler(symboleReference);
			
/*			if (nInsTri>=1
				&&
				InsTri[nInsTri-1].Code==I_ENTIER)
			{
				switch(Instr[i].Code)
				{
				case I_TABLEAUREF:
					InsTri[nInsTri-1].Code=I_CTABLEAUREF;
					break;
				case I_TABLEAUVAL:
					InsTri[nInsTri-1].Code=I_CTABLEAUVAL;
					break;
				}
				return;
			}*/
			break;
		}
	case I_CTABLEAUREF:
	case I_CTABLEAUVAL:
		if (!PileType[PosPileType-1].EstType(symboleValeur))
		{
			Symbole * ty=PileType[PosPileType-1].Depiler();
			if (ty!=symboleReference)
				throw TXT("référence sur un tableau attendue");
			ty=PileType[PosPileType-1].Depiler();
			if (ty==symboleTableau)
			{
				//OK
			}
			else if (!ty->IsPod())
			{
			//	ASSERT(ty<(classe)Classe.l);
				int prop=Valeur[v].v.i;
			/*	ASSERT(prop<(int)Classe[ty].Prop.l);
				PileType[PosPileType-1]=Classe[ty].Prop[prop].Type;*/
				//throw TXT("non implémenté");
				//Pas génial
				Symbole * symbole=0;
				for (Symbole * s=ty->fils; s!=0; s=s->suivant)
				{
					if (s->type==Symbole::VariableThis && s->indice==prop)
					{
						symbole=s;
						break;
					}
				}
				if (symbole)
					PileType[PosPileType-1]=symbole->TypeAc;
				else
					throw _T("erreur interne : propriété introuvable");
			}
			else
				throw TXT("tableau attendu");
		}
		if (Code==I_CTABLEAUREF)
			PileType[PosPileType-1].Empiler(symboleReference);
		break;
	case I_DEBUTPARAM:
		PileType[PosPileType++].Def(symboleDebutParams);
		break;
	case I_DUPLIQUER:
		{
			PileType[PosPileType++]=PileType[PosPileType-1];
			break;
		}
	case I_SUPPRIMER:
		PosPileType--;
		aRetour=false;
		break;
	case I_THISREF:
	case I_VARIABLELOC:
		PileType[PosPileType]=Valeur[v].v.ptr->TypeAc;
		PileType[PosPileType++].Empiler(symboleReference);
		break;
	case I_THISVAL:
	case I_VALEURVARLOC:
		PileType[PosPileType++]=Valeur[v].v.ptr->TypeAc;
		break;
	case I_ETIQUETTELOC:
		PileType[PosPileType++].Def(symboleFonctionGPP);
		break;
	case I_GOTO:
	case I_GOTOR:
		aRetour=false;
		break;
	case I_GOTONONZERO:
	case I_GOTONONZEROR:
		PosPileType--;
		aRetour=false;
		break;
	case I_HACHAGEREF:
		{
			Symbole * c=PileType[PosPileType-1].Depiler();
			if (c!=symboleValeur && c!=symboleChaine)
				throw CErreur(TXT("chaîne attendue comme indice de hachage"));
			PosPileType--;
			if (!PileType[PosPileType-1].EstType(symboleValeur))
			{
				if (PileType[PosPileType-1].Depiler()!=symboleReference)
					throw TXT("référence sur un hachage attendu");
				Symbole * sy=PileType[PosPileType-1].Depiler();
				if (sy==symboleValeur)
					PileType[PosPileType-1].Def(symboleValeur);
				else if (sy!=symboleTableHachage)
					throw TXT("hachage attendu");
				PileType[PosPileType-1].Empiler(symboleReference);
			}
			break;
		}
	case I_HACHAGEVAL:
		{
			Symbole * c=PileType[PosPileType-1].Depiler();
			if (c!=symboleValeur && c!=symboleChaine)
				throw TXT("chaîne attendue comme indice de hachage");
			PosPileType--;
			if (!PileType[PosPileType-1].EstType(symboleValeur))
			{
				if (PileType[PosPileType-1].Depiler()!=symboleReference)
					throw TXT("référence sur un hachage attendu");
				Symbole * sy=PileType[PosPileType-1].Depiler();
				if (sy==symboleValeur)
					PileType[PosPileType-1].Def(symboleValeur);
				else if (sy!=symboleTableHachage)
					throw TXT("hachage attendu");
				PileType[PosPileType-1].Empiler(symboleReference);
			}
			break;
		}
	case I_SUPERPLUS:
		PosPileType--;
		break;
	case I_INCREMENTER:
	case I_DECREMENTER:
	case I_DIVISEREGAL:
	case I_MULTIPLIEREGAL:
		PosPileType-=2;
		aRetour=false;
		break;
	case I_TAILLE:
		PileType[PosPileType-1].Def(symboleEntier);
		break;
	case I_VALEURCLEF:
		PileType[PosPileType-1].Def(symboleChaine);
		break;
	case I_PROBASAUTERLIGNE:
		aRetour=false;
		break;
	case I_NOUVEAU:
		PileType[PosPileType].Def(symboleValeur);
		PileType[PosPileType++].Empiler(symboleReference);
		break;
	case I_INSTANCEDE:
		PosPileType-=2;
		aRetour=false;
		break;
	case I_GOTOTACHER:
	case I_GOTOTACHE:
		PileType[PosPileType++].Def(symboleEntier);
		break;
	case I_FINTACHE:
		aRetour=false;
		break;
	case I_EGAL:
	case I_DIFFERENT:
	case I_INFERIEUR:
	case I_SUPERIEUR:
	case I_INFEGAL:
	case I_SUPEGAL:
		PosPileType--;
		PileType[PosPileType-1].Def(symboleEntier);
		break;
	case I_NON:
		PileType[PosPileType-1].Def(symboleEntier);
		break;
	case I_GOTOZERO:
	case I_GOTOZEROR:
		PosPileType--;
		aRetour=false;
		break;
	case I_DEFCONSTR:
		PosPileType--;
		aRetour=false;
		break;
	case I_THIS:
		PileType[PosPileType].Def(Bloc.Dernier()->symbole);
		PileType[PosPileType++].Empiler(symboleReference);
		break;
	case I_ENERREUR:
	case I_ENERREURR:
		aRetour=false;
		break;
	case I_ERREUR:
		PosPileType--;
		aRetour=false;
		break;
	case I_OU:
	case I_ET:
	case I_FINOUET:
		PileType[PosPileType-1].Def(symboleEntier);
		break;
	case I_TYPEDE:
		PileType[PosPileType-1].Def(symboleEntier);
		break;
	case I_SWITCH:
		PosPileType--;
		aRetour=false;
		break;
	case I_PARAMREF:
		PileType[PosPileType]=Valeur[v].v.ptr->TypeAc;
		PileType[PosPileType++].Empiler(symboleReference);
		break;
	case I_PARAMVAL:
		PileType[PosPileType++]=Valeur[v].v.ptr->TypeAc;
		break;
	case I_ESTREFVALIDE:
		PileType[PosPileType-1].Def(symboleEntier);
		break;
	case I_EMPILER2:
		PosPileType--;
		aRetour=false;
		break;
	case I_VALEUR2:
	case I_DEPILER2:
		PileType[PosPileType++].Def(symboleValeur);
		break;
	case I_DECALG:
	case I_DECALD:
	case I_MODULO:
	case I_ETBIN:
	case I_OUBIN:
		PosPileType--;
		PileType[PosPileType-1].Def(symboleEntier);
		break;
	case I_EXISTE:
		PileType[PosPileType-1].Def(symboleEntier);
		break;
	case I_NPARAMS:
		PileType[PosPileType++].Def(symboleEntier);
		break;
	case I_CARAC:
		PileType[PosPileType++].Def(symboleCarac);
		break;
	case I_STOP:
		aRetour=false;
		break;
	case I_CONSTVAR:
		{
			Symbole * u=Valeur[v].v.ptr;
			PosPileType--;
				
			if (u->Valeur.Type!=TYPE_INCONNU)
				throw TXT("redéfinition d'une constante");

			if (insTri.nIns>0)
			{
				switch (insTri.ins[insTri.nIns-1].Code)
				{
				case I_ENTIER:
					u->Valeur=insTri.val[Val1];
					break;
				default:
					throw TXT("Ce type de constante n'est pas supporté");
				}
				insTri.nIns--;//On supprime l'instruction qui rajoute une constante
				insTri.nVals=insTri.ins[insTri.nIns-1].pVal+insTri.ins[insTri.nIns-1].nVal;
				return;//On ignore l'instruction I_CONSTVAR
			}
			else
				throw TXT("La valeur de la constante ne peut être calculée");
			aRetour=false;
			break;
		}

	case I_FONCTIONC_THIS:
	case I_FONCTIONC:
		//PileType[PosPileType++].Def(TYPE_FONCTIONC);
		//break;
		{
			Symbole * symbole=Valeur[v].v.ptr;
			PileType[PosPileType++]=symbole->TypeAc;
			Valeur[v].Type=symbole->Valeur.Type;
			Valeur[v].v.fonctionC=symbole->Valeur.v.fonctionC;
			break;
		}
	case I_FONCTIONMODULE:
		//PileType[PosPileType++].Def(TYPE_FONCTIONMODULE);
		//break;
	case I_ETIQUETTE:
	case I_ETIQUETTE_THIS:
	//PileType[PosPileType++].Def(TYPE_ETIQUETTE_THIS);
	{
		Symbole * symbole=Valeur[v].v.ptr;
		PileType[PosPileType++]=symbole->TypeAc;
		break;
	}
	case I_ALLOCATION:
		{
			//On transforme le type en taille à allouer
			PileType[PosPileType++]=Valeur[v].v.ptr;
			Valeur[v]=(int)Valeur[v].v.ptr->taille;
			break;
		}

	default:
		throw CErreur(TXT("code %d inconnu"),Code);
	}
	
	//}//Pas optimiser
//}
	if (aRetour)
		PileType[PosPileType-1].instruction=insTri.nIns;

	SInstr &in=insTri.Ajouter(Instr[i].nVal);
	in.Code=Instr[i].Code;
	if (Instr[i].nVal)
	{
		memcpy(&insTri.val[in.pVal],&Valeur[v],sizeof(valeur)*Instr[i].nVal);
		for (index_t i=0; i<Instr[i].nVal; i++)
			Valeur[v].Effacer();
	}
}					
