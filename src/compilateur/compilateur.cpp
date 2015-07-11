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
					 
Voici le fameux compilateur, qui vous domine du haut de ses je ne sais
combien de lignes. Bon courage pour comprendre tout ça.
*/

#include <gotopp/base.h>
#include <gotopp/compilateur.h>
#include <gotopp/erreur.h>

#include "compriv.h"
#include <gotopp/global.h>

using namespace GotoPP;
using namespace std;

namespace GotoPP
{
	bool ALPHA(carac c)
	{
		return ((c>='A' && c<='Z')||(c>='a' && c<='z'));
	}

	bool ALPHANUM(carac c)
	{
		return ((c>='A' && c<='Z')||(c>='a' && c<='z')||(c>='0' && c<='9'));
	}

	inline bool EGAL(const carac * x, const carac * y, size_t ly)
	{
		return (czlen(x)==ly && memcmp(x,y,ly*sizeof(carac))==0);
	}
	#define EGALMC(y) ((lClef==sizeof(_T(y))/sizeof(carac)-1)&&(memcmp(MotClef,_T(y),sizeof(_T(y))-sizeof(carac))==0))
	
	const int PRI_CONSTANTE=1;
	const int PRI_TAILLE=2;
	const int PRI_PLUSMOINS=5;
	const int PRI_FOIS=8;
	const int PRI_MODULO=8;
	const int PRI_CONCAT=9;
	const int PRI_EXEC=10;
	const int PRI_DEF=15;
	const int PRI_REGOTO=18;
	const int PRI_ABSOLUE=3;
	const int PRI_OPPOSE=3;
	const int PRI_TYPEDE=1;
	const int PRI_VIRGULE=999;
	const int PRI_MAX=1000;
	const int PRI_COLLE=1001;
	const int PRI_PAR_O=0;
	const int PRI_PAR_F=1003;
	const int PRI_FIN=1004;
	const int PRI_NOUVEAU=10;
	const int PRI_COMP=14;
	const int PRI_NON=4;
	const int PRI_OU=16;
	const int PRI_ET=17;
	const int PRI_SWITCH=20;
	const int PRI_OUBIN=12;
	const int PRI_ETBIN=13;
	const int PRI_DECALAGEBIN=7;

	index_t LireEtiquette(bool & local);
	carac prochainFichier[512];
}

/*
Le principe de base du compilateur est expliqué dans CompilerSeparateur.

* Explication sur les variables "longues", c'est à dire les variables
dotées de propriétés, d'indices de tableau, etc.

machin.chose[truc+13][chose].bidule=
Si une variable est terminée par . ou [ alors cette
variable est une variable longue.
Une variable longue est entourée de parenthèses dont la priorité
est celle d'une constante (<> des vraies parenthèses).

variables longues => flag variablelongue=true
on saute le point
par la suite :
soit on lit [ soit on lit des mots clefs => tout le reste génère une erreur
Lecture de [ : variablelongue=false
Lecture de ] : variablelongue=true
en lisant une variable si le flag variablelongue=true c'est une propriété
si après un ] ou une variable il n'y a pas . ou [ alors on
dépile les variables longues en mettant la parenthèse finale et
variablelongue=false

*/

//namespace GotoPP {
//	namespace Compilateur {

inline void Compilateur::AjParO(uint priorite=PRI_PAR_O)
{
	Instr[nInstr].p=priorite;
	Instr[nInstr].pVal=nValeurs;
	Instr[nInstr].nVal=0;
	Instr[nInstr++].Special=SPE_PAR_O;
}

inline void Compilateur::AjParF()
{
	Instr[nInstr].p=PRI_PAR_F;
	Instr[nInstr].pVal=nValeurs;
	Instr[nInstr].nVal=0;
	Instr[nInstr++].Special=SPE_PAR_F;
}

inline void Compilateur::AjDebutParams()
{
	Instr[nInstr].p=PRI_CONSTANTE;
	Instr[nInstr].pVal=nValeurs;
	Instr[nInstr].nVal=0;
	Instr[nInstr].Code=I_DEBUTPARAM;
	Instr[nInstr++].Special=SPE_NORMAL;
}


inline void Compilateur::AjCode(code Code, uint Priorite=PRI_CONSTANTE)
{
	Instr[nInstr].p=Priorite;
	Instr[nInstr].pVal=nValeurs;
	Instr[nInstr].nVal=0;
	Instr[nInstr].Code=Code;
	Instr[nInstr++].Special=SPE_NORMAL;
}

inline void Compilateur::AjNoOp(uint priorite)
{
	Instr[nInstr].p=priorite;
	Instr[nInstr].pVal=nValeurs;
	Instr[nInstr].nVal=0;
	Instr[nInstr++].Special=SPE_NOOP;
}

inline void Compilateur::AjExpReg(carac *er, carac *modif)
{
    Instr[nInstr-1].nVal++;
	Valeur[nValeurs].Type=TYPE_OBJETC;
	Valeur[nValeurs++].v.o=regexp::Creer(er, modif);
}

inline void Compilateur::AjEntier(int e)
{
	Instr[nInstr-1].nVal++;
	Valeur[nValeurs].Type=TYPE_ENTIER;
	Valeur[nValeurs++].v.i=e;
}

inline void Compilateur::AjType(type t)
{
	Instr[nInstr-1].nVal++;
	Valeur[nValeurs].Type=TYPE_TYPE;
	Valeur[nValeurs++].v.i=t;
}

inline void Compilateur::AjCarac(carac c)
{
	Instr[nInstr-1].nVal++;
	Valeur[nValeurs].Type=TYPE_CARAC;
	Valeur[nValeurs++].v.i=c;
}

inline void Compilateur::AjChances(float f)
{
	Instr[nInstr-1].nVal++;
	Valeur[nValeurs].Type=TYPE_CHANCES;
	Valeur[nValeurs++].v.f=f;
}


inline void Compilateur::AjChaine(carac *c,size_t l)
{
	Instr[nInstr-1].nVal++;
	Valeur[nValeurs].Type=TYPE_CHAINE;
	Valeur[nValeurs++].v.c=chaineval::CreerFormat(c,l);
}

inline void Compilateur::AjEntierType(int e,type Type)
{
	Instr[nInstr-1].nVal++;
	Valeur[nValeurs].Type=Type;
	Valeur[nValeurs++].v.i=e;
}

inline void Compilateur::AjPtrType(Symbole * ptr, type Type)
{
	Instr[nInstr-1].nVal++;
	Valeur[nValeurs].Type=Type;
	Valeur[nValeurs++].v.ptr=ptr;
}

inline bool Compilateur::EstNouveauSymbole(Symbole * s)
{
	return s==0;
}

void Compilateur::SupprimerVarLongue()
{
	nInstr=VarLongue[iVarLongue].nInstrsDepart;
	nValeurs=Instr[nInstr].pVal;	
}

bool Compilateur::CompilerOperateur()
{
	if (*Source=='@' && Source[1]=='%' && Source[2]=='N'
		&& Source[3]=='e')
	{
		clog << TXT("Bien tenté mais ça ne marche pas.\n");
		Source+=4;
	}
	else
	if (*Source=='@' && (Source[1]=='+'||Source[1]=='-'||Source[1]==':'))
	{
		Source++;
		if (*Source=='+')
			AjCode(I_EMPILER2);
		else if (*Source=='-')
			AjCode(I_DEPILER2);
		else if (*Source==':')
			AjCode(I_VALEUR2);
		Source++;
	}
	else
	if (*Source==SEP_PROP) //Sépare les propriétés
	{
		if (VarLongue[iVarLongue].Type.l==0)
			throw TXT("@ doit être placé après un nom de variable");
		Source++;
        SauterEspaces();
		ChercherMotClef();
		if (lClef==0)
			throw TXT("@ doit être suivi d'un nom de pingouin");
		Symbole * prop=ChercherSymboleDans(VarLongue[iVarLongue].Type.Dernier());
		CompilerVariable(prop,VarLongue[iVarLongue].Val);
	}
	else
	if (*Source=='|')
	{
		Source++;
		if (*Source=='*')
			AjCode(I_ETBIN,PRI_ETBIN);
		else if (*Source=='+')
			AjCode(I_OUBIN,PRI_OUBIN);
		else if (*Source=='<')
			AjCode(I_DECALG,PRI_DECALAGEBIN);
		else if (*Source=='>')
			AjCode(I_DECALD,PRI_DECALAGEBIN);
		else
			throw TXT("opérateur binaire inconnu");
		Source++;
	}
	else
	if (*Source=='?' && nInstr==0)
	{
		if (AcSwitch.l==0)
			throw TXT("on n'est pas dans un GOTOMULTIPLE");
		Source++;
		if (czncmp(Source,_T("défaut?"),7)==0)
		{
			Source+=7;
			programme->Switch[AcSwitch.Dernier()]->PosDefaut=(code*)Ins.t;
		}
		else
			LigneAc=LP_CAS;
	}
	else
	if (*Source=='!')
	{
		Source++;
		if (*Source>='0' && *Source<='9')
		{
			ProbaSauterLigne=0;
			do
			{
				ProbaSauterLigne*=10;
				ProbaSauterLigne+=*Source-'0';
				Source++;
			}
			while (*Source>='0' && *Source<='9');
		}
		else
			ProbaSauterLigne=90;
		InstrAc=IP_OPERATEUR;
	}
	else
	if (*Source=='`')
	{
		Source++;
		AjCode(I_TYPEDE,PRI_TYPEDE);
	}
	else
	if (*Source=='_')
	{
		Source++;
		AjCode(I_CONCAT,PRI_CONCAT);
		InstrAc=IP_OPERATEUR;
	}
	else if (*Source=='"')
	{
		AjCode(I_VALEURCLEF);
		Source++;
		InstrAc=IP_OPERATEUR;
	}
	else if (*Source=='#')
	{
		AjCode(I_VALEUR);
		Source++;
		InstrAc=IP_OPERATEUR;
	}
	else if (*Source=='}')
	{
		AjParO(PRI_CONSTANTE);
		Source++;
		InstrAc=IP_PARO;
	}
	else if ((*Source=='{')||(*Source==')'))
	{
		AjParF();
		Source++;
	}
	else if (*Source=='~')
	{
		Source++;
		if (*Source=='~')
		{
			AjCode(I_NONOPPOSE,PRI_OPPOSE);
			Source++;
		}
		else
			AjCode(I_OPPOSE,PRI_OPPOSE);
		InstrAc=IP_OPERATEUR;
	}
	else if (*Source=='=')
	{
		Source++;
		if (*Source=='-')
		{
			Source++;
			AjCode(I_DECREMENTER,PRI_DEF);
		}
		else
		if (*Source=='+')
		{
			Source++;
			AjCode(I_INCREMENTER,PRI_DEF);
		}
		else
		if (*Source=='=')
		{
			Source++;
			AjCode(I_INSTANCEDE,PRI_DEF);
		}
		else
			AjCode(I_DEFINIR,PRI_DEF);
		InstrAc=IP_OPERATEUR;
	}
	else if (*Source==':')
	{
		Source++;
		if (*Source=='=')
		{
			AjCode(I_DIVISEREGAL,PRI_DEF);
			Source++;
		}
		else
			AjCode(I_DIVISE,PRI_FOIS);
		InstrAc=IP_OPERATEUR;
	}
	else if ((*Source=='*')&&(Source[1]=='='))
	{
		Source+=2;
		AjCode(I_MULTIPLIEREGAL,PRI_DEF);
		InstrAc=IP_OPERATEUR;
	}
	else if (*Source=='+')
	{
		Source++;
		if (*Source=='+')
		{
			Source++;
			AjCode(I_SUPERPLUS,PRI_PLUSMOINS);
		}
		else
		{
			AjCode(I_AJOUTER,PRI_PLUSMOINS);
			//AjOperateurVV(OPVV_PLUS);
		}
		InstrAc=IP_OPERATEUR;
	}
	else if (*Source=='-')
	{
		AjCode(I_SOUSTRAIRE,PRI_PLUSMOINS);
		Source++;
		InstrAc=IP_OPERATEUR;
	}
	else if (*Source=='(')
	{
		Source++;
		AjCode(I_EXECVAL,PRI_EXEC);
		//On ferme la parenthèse qui *doit* avoir été ouverte
		//avant de mettre la valeur dans la pile
		AjParF();
		AjParO();
		AjDebutParams();
	}
	else if (*Source=='%')
	{
		if (NiveauGourou>2)
			throw TXT("le % est interdit pour les gourous de niveau supérieur à 2");

		bool local;
		Symbole * s=LireEtiquette(local);
		if (!EstNouveauSymbole(s) && s->EstClasse())
			CompilerClasse(s);
		else
			CompilerVariable(s, false);
	}
	else
		return false;
	return true;
}


void Compilateur::DebutBloc(ETypeBloc Type, Symbole * espace)
{
	CBloc * bloc=new CBloc();
	bloc->Type=Type;
	bloc->Depart=(int)Ins.t;
	if (espace==0)
	{
		Symbole * parent;
		if (Bloc.l>0)
			parent=Bloc.Dernier()->symbole;
		else
			parent=programme->symboleGlobal;
		//On crée un symbole vide
		espace=new Symbole(_T(""),parent,Symbole::Bloc);
		if (parent->PeutContenirLocaux())
			espace->taille=parent->taille;
		programme->symbole.Etendre(espace);
	}
	bloc->symbole=espace;
	Bloc.Etendre(bloc);
	LigneAc=LP_DEBUTBLOC;
}

void Compilateur::FinBloc()
{
	if (Bloc.l==0)
		throw TXT("pas de bloc ouvert");
	bool Sinon=false;

	switch(Bloc.Dernier()->Type)
	{
	case TB_RIEN:
		
		break;
		
	case TB_SINON:
		//clog << "bloc sinon terminé en ligne "<<Ligne<<'\n';
		if (RefEtiqA.l==0)
			throw TXT("un AC sans AUTOGOTOZ");
		Ins.ChgGotoRel(RefEtiqA.Dernier().Etiq,(int)Ins.t);
		RefEtiqA.l--;
		break;
		
	case TB_SI:
		{
			//clog << "bloc si terminé en ligne "<<Ligne<<'\n';
		
			//On regarde si il y a un sinon après
			carac *src=Source;
			int ligne=Ligne;
			do
			{
				SauterEspaces();
				if (*Source=='\n')
				{
					Ligne++;
					Source++;
				}
				else
				if (*Source=='/')
					Source++;
				else
					break;
			}
			while (1);
			
			if (Source[0]=='A' && Source[1]=='C' && Source[2]=='A'
					&& Source[3]=='C' && (ucarac(Source[4])<=' '||Source[4]=='/'))
			{
				Source+=4;
				SauterEspaces();
				if ((nInstr)||(*Source!='\n' && *Source!=0 && *Source!='/'))
					throw TXT("ACAC doit être seul sur la ligne");
			
				//L'équivalent de sinon : insère un GOTO vers la suite
				//puis un AC
				SauterEspaces();

				Ins.AjouterCode(I_GOTOR);
				//Cible d'un AUTOGOTOZ qui crée un GOTOPASMALIN
				int cible=RefEtiqA.Dernier().Etiq;
				RefEtiqA.Dernier().Etiq=(int)Ins.t;
				Ins.AjouterEntier(0);
				Ins.ChgGotoRel(cible,(int)Ins.t);
				Sinon=true;
			}	
			else
			{	
				Source=src;
				Ligne=ligne;
				if (RefEtiqA.l==0)
					throw TXT("un AC sans AUTOGOTOZ");
				Ins.ChgGotoRel(RefEtiqA.Dernier().Etiq,(int)Ins.t);
				RefEtiqA.l--;
			}
			break;
		}

	case TB_FOREACH:
		{
			int Depart=(int)Ins.t;
			Ins.AjouterCode(I_VARIABLELOC);
			Ins.AjouterEntier(Bloc.Dernier()->indice->indice);
			Ins.AjouterCode(I_ENTIER);
			Ins.AjouterEntier(1);
			Ins.AjouterCode(I_INCREMENTER);
			Ins.AjouterCode(I_GOTOR);
			Ins.AjouterGotoRel(Bloc.Dernier()->Depart);
			
			for (index_t e=0; e<Bloc.Dernier()->Continue.l;e++)
				Ins.ChgGotoRel(Bloc.Dernier()->Continue[e],Depart);
			for (index_t e=0; e<Bloc.Dernier()->Break.l;e++)
				Ins.ChgGotoRel(Bloc.Dernier()->Break[e],(int)Ins.t);
			break;
		}
		
	case TB_FOR:
		{
			int Depart=(int)Ins.t;
			if (Bloc.Dernier()->PasDuFor)
			{
				LigneDeCode * ldc=Bloc.Dernier()->PasDuFor;
				CreerCodeFinal(*ldc);
				delete ldc;
			}
			Ins.AjouterCode(I_GOTOR);
			Ins.AjouterGotoRel(Bloc.Dernier()->Depart);
			for (index_t e=0; e<Bloc.Dernier()->Continue.l;e++)
				Ins.ChgGotoRel(Bloc.Dernier()->Continue[e],Depart);
			for (index_t e=0; e<Bloc.Dernier()->Break.l;e++)
				Ins.ChgGotoRel(Bloc.Dernier()->Break[e],(int)Ins.t);
			break;
		}

	case TB_TANTQUE:
		{
			//clog << "bloc tantque terminé en ligne "<<Ligne<<'\n';
			Ins.AjouterCode(I_GOTOR);
			Ins.AjouterGotoRel(Bloc.Dernier()->Depart);
			for (index_t e=0; e<Bloc.Dernier()->Continue.l;e++)
				Ins.ChgGotoRel(Bloc.Dernier()->Continue[e],Bloc.Dernier()->Depart);
			for (index_t e=0; e<Bloc.Dernier()->Break.l;e++)
				Ins.ChgGotoRel(Bloc.Dernier()->Break[e],(int)Ins.t);
			break;
		}

	case TB_SWITCH:
		{
			//clog << "bloc switch terminé en ligne "<<Ligne<<'\n';
		
			if (AcSwitch.l==0)
				throw TXT("une fin de GOTOMULTIPLE sans GOTOMULTIPLE ouvert");
			
			if (programme->Switch[AcSwitch.Dernier()]->PosDefaut==NULL)
				programme->Switch[AcSwitch.Dernier()]->PosDefaut=(code*)Ins.t;
			//Trions les switchs
			qsort(programme->Switch[AcSwitch.Dernier()]->Cas.t,
				programme->Switch[AcSwitch.Dernier()]->Cas.l,sizeof(CCasSwitch),
				CompCasSwitch);
			AcSwitch.l--;

			for (index_t e=0; e<Bloc.Dernier()->Break.l;e++)
				Ins.ChgGotoRel(Bloc.Dernier()->Break[e],(int)Ins.t);
			break;
		}

	case TB_FONCTION:
		//clog << "bloc fonction terminé en ligne "<<Ligne<<'\n';
		
		AjCode(I_RETOUR,PRI_EXEC);
		break;

	case TB_CLASSE:
		break;

#ifdef _DEBUG
	default:
		throw TXT("bloc de type inconnu");
#endif
	}

	Bloc.EffacerDernier();

	if (Bloc.l==0)
	{
		/*//On fait le lien des étiquettes locales
		qsort(&LabelL.Premier(),LabelL.l,sizeof(SLabel),CmpLabels);
		//clog << RefEtiq.l << " références à des étiquettes \n";
		for (index_t z=0; z<RefEtiqL.l; z++)
		{
			int *k=(int*)&Ins.c[RefEtiqL[z]];
			int v=*k;
			SLabelL * l=(SLabelL*)bsearch(&v,&LabelL.Premier(),LabelL.l,sizeof(SLabelL),CmpLabels);
			if (!l)
				throw TXT("il est fait référence à une étiquette locale qui n'existe pas dans ce bloc, à toi de trouver laquelle");
			*k=l->Valeur-RefEtiqL[z]-sizeof(int);
		}			
		LabelL.l=0;
		RefEtiqL.l=0;*/
	}
	if (Bloc.Dernier()->classeSpecifiee)
		Bloc.EffacerDernier();
	if (Sinon)
		DebutBloc(TB_SINON);
}

bool Compilateur::CompilerMotClef0()
{
	if (EGALMC("bush") || EGALMC("deubeulyou"))
	{
		SauterEspaces();
		ChercherMotClef();
		if (lClef==0)
			throw TXT("une constante doit avoir un nom");

		Symbole * symb=programme->NouvelleConstante(MotClef,lClef);

		SauterEspaces();
		if (*Source!='=')
			throw TXT("il manque le = après le nom de la constante");
		Source++;
		AjCode(I_CONSTVAR,PRI_DEF);
		AjPtrType(symb,TYPE_SYMBOLE);
	}
	else
	if (EGALMC("BLOC"))
	{
		DebutBloc(TB_RIEN);
	}
	else if (EGALMC("FINBLOC"))
	{
		FinBloc();
	}
	else
	if (EGALMC("FINGOTOMULTIPLE"))
	{
		if (Bloc.l==0 || Bloc.Dernier()->Type!=TB_SWITCH)
			throw TXT("FINGOTOMULTIPLE doit fermer un GOTOMULTIPLE");
		FinBloc();
	}
	else
	if (EGALMC("GOTOMULTIPLE"))
	{
		AjCode(I_SWITCH,PRI_SWITCH);
		AjEntier((int)programme->Switch.l);
		AcSwitch.Etendre((int)programme->Switch.l);
		CSwitch * swit=new CSwitch();
		programme->Switch.Etendre(swit);
		CibleAuto++;//Pour les "arret"
		DebutBloc(TB_SWITCH);
	}
	else
	if (EGALMC("et"))
	{
		AjCode(I_ET,PRI_ET);
		AjEntier((int)EtOu.l);
		AjCode(I_FINOUET,PRI_ET);
		AjEntier((int)EtOu.l);
		EtOu.Etendre(-1);
	}
	else
	if (EGALMC("ou"))
	{
		AjCode(I_OU,PRI_OU);
		AjEntier((int)EtOu.l);
		AjCode(I_FINOUET,PRI_OU);
		AjEntier((int)EtOu.l);
		EtOu.Etendre(-1);
	}
	else
	if (EGALMC("eg"))
	{
		AjCode(I_EGAL,PRI_COMP);
	}
	else
	if (EGALMC("diff"))
	{
		AjCode(I_DIFFERENT,PRI_COMP);
	}
	else
	if (EGALMC("inf"))
	{
		AjCode(I_INFERIEUR,PRI_COMP);
	}
	else
	if (EGALMC("sup"))
	{
		AjCode(I_SUPERIEUR,PRI_COMP);
	}
	else
	if (EGALMC("supeg"))
	{
		AjCode(I_SUPEGAL,PRI_COMP);
	}
	else
	if (EGALMC("infeg"))
	{
		AjCode(I_INFEGAL,PRI_COMP);
	}
	else
	if (EGALMC("non"))
	{
		AjCode(I_NON,PRI_NON);
	}
	else
	if (EGALMC("AUTOGOTOZ"))
	{
		AjCode(I_GOTOZEROR,PRI_EXEC);
		AjEntierType(0,TYPE_ETIQAUTO);
		DebutBloc(TB_SI);
	}
	else
	if (EGALMC("TantQue"))
	{
		DebutBloc(TB_TANTQUE);
		AjCode(I_GOTOZEROR,PRI_EXEC);
		AjEntierType((int)Bloc.l-1,TYPE_ETIQAUTOBREAK);
	}
	else
	if (EGALMC("costaud"))
	{
		DebutBloc(TB_FOR);
		LigneAc=LP_FOR_INIT;
	}
	else
	if (EGALMC("PouCharque"))
	{
		SauterEspaces();
		Symbole * classe=ChercherSymbole();
		if (EstNouveauSymbole(classe) || !classe->EstClasse())
			throw TXT("classe de la variable de parcours indéfinie");
		SauterEspaces();
		ChercherMotClef();
		if (lClef==0)
			throw TXT("indiquez un nom pour la variable de parcours");
		
		DebutBloc(TB_FOREACH);

		Symbole * s=Bloc.Dernier()->symbole;
		
		//On crée les trois variables locales utilisées
		Bloc.Dernier()->indice=programme->NouveauSymbole(_T("indice"),6,CType(symboleEntier),s);
		DernierSymbole().type=Symbole::VariableLocale;
		DernierSymbole().indice=(index_t)s->taille++;

		Bloc.Dernier()->tableau=programme->NouveauSymbole(_T("tableau"),7,CType(classe),s);
		DernierSymbole().TypeAc.Empiler(symboleTableau);
		DernierSymbole().type=Symbole::VariableLocale;
		DernierSymbole().indice=(index_t)s->taille++;

		Bloc.Dernier()->iterateur=programme->NouveauSymbole(MotClef,lClef,CType(classe),s);
		DernierSymbole().type=Symbole::VariableLocale;
		DernierSymbole().indice=(index_t)s->taille++;

		index_t svarLoc=(index_t)(Bloc[Bloc.l-2]->symbole->taille);
		AjCode(I_VARIABLELOC,PRI_CONSTANTE);
		AjPtrType(Bloc.Dernier()->indice,TYPE_VARIABLE);
		AjCode(I_ENTIER,PRI_CONSTANTE);
		AjEntier(0);
		AjCode(I_DEFINIR,PRI_CONSTANTE);
		AjCode(I_VARIABLELOC,PRI_CONSTANTE);
		AjPtrType(Bloc.Dernier()->tableau,TYPE_VARIABLE);
		AjCode(I_INSTANCEDE,PRI_MAX);
		
		LigneAc=LP_FOREACH_INIT;
	}
	else
	if (EGALMC("arret"))
	{
		AjCode(I_GOTOR,PRI_EXEC);
		int Cible=int(Bloc.l)-1;
		while (Cible>=0 && Bloc[Cible]->Type!=TB_FONCTION
				&& Bloc[Cible]->Type!=TB_TANTQUE
				&& Bloc[Cible]->Type!=TB_SWITCH
				&& Bloc[Cible]->Type!=TB_FOR
				&& Bloc[Cible]->Type!=TB_FOREACH)
			Cible--;
		if (Bloc[Cible]->Type==TB_FONCTION)
			throw TXT("interdit de sortir d'une fonction avec arret");
		if (Cible<0)
			throw TXT("un arret doit se trouver dans un bloc");
		AjEntierType(Cible,TYPE_ETIQAUTOBREAK);
	}
	else
	if (EGALMC("continue"))
	{
		AjCode(I_GOTOR,PRI_EXEC);
		int Cible=int(Bloc.l)-1;
		while (Cible>=0 && Bloc[Cible]->Type!=TB_FONCTION
				&& Bloc[Cible]->Type!=TB_TANTQUE
				&& Bloc[Cible]->Type!=TB_FOR
				&& Bloc[Cible]->Type!=TB_FOREACH)
			Cible--;
		if (Bloc[Cible]->Type==TB_FONCTION)
			throw TXT("interdit de sortir d'une fonction avec continue");
		if (Cible<0)
			throw TXT("un continue doit se trouver dans un TantQue");

		AjEntierType(Cible,TYPE_ETIQAUTOCONTINUE);
	}
	else
	if (EGALMC("AC"))
	{
		if (nInstr)
			throw TXT("AC doit être en début de ligne");

		if ((Bloc.l==0 || (Bloc.Dernier()->Type!=TB_SI&&Bloc.Dernier()->Type!=TB_SINON)))
			throw TXT("un AC doit terminer un AUTOGOTOZ");
		FinBloc();
	}
	else
	if (EGALMC("faiblard"))
	{
		SauterEspaces();
		if ((nInstr)||(*Source!='\n' && *Source!=0))
			throw TXT("faiblard doit être seul sur la ligne");
		if (Bloc.l==0 || Bloc.Dernier()->Type!=TB_FOR)
			throw TXT("un faiblard doit terminer un costaud");
		FinBloc();
	}
	else
	if (EGALMC("FinTantQue"))
	{
		SauterEspaces();
		if ((nInstr)||(*Source!='\n' && *Source!=0))
			throw TXT("FinTantQue doit être seul sur la ligne");
		if (Bloc.l==0 || (Bloc.Dernier()->Type!=TB_TANTQUE
				&& Bloc.Dernier()->Type!=TB_FOREACH))
			throw TXT("un FinTantQue doit terminer un TantQue ou un PouCharque");
		FinBloc();
	}
	else
	if (EGALMC("ACAC"))
	{
			throw TXT("que fait ce ACAC ici ?");
	}
	else
		return false;
	return true;
}

bool Compilateur::CompilerMotClef1()
{
	if (EGALMC("GOTOPASMALIN"))
	{
		SauterEspaces();
		bool locale;
		Symbole * c=LireEtiquette(locale);
			
		if (locale)
		{
			AjCode(I_GOTOR,PRI_CONSTANTE);
			AjPtrType(c,TYPE_ETIQLOC);
		}
		else
		{
			AjCode(I_GOTO,PRI_CONSTANTE);
			AjPtrType(c,TYPE_ETIQUETTE);
		}
	}
	else
	if (EGALMC("GOTONULPOURLESNULS"))
	{
		SauterEspaces();
		bool Locale;
		Symbole * c=LireEtiquette(Locale);
	
		if (Locale)
		{
			AjCode(I_GOTOZEROR,PRI_EXEC);
			AjPtrType(c,TYPE_ETIQLOC);
		}
		else
		{
			AjCode(I_GOTOZERO,PRI_EXEC);
			AjPtrType(c,TYPE_ETIQUETTE);
		}
	}
	else
	if (EGALMC("GOTONONNULPOURLESNULS"))
	{
		SauterEspaces();
		bool Locale;
		Symbole * c=LireEtiquette(Locale);	
		if (Locale)
		{
			AjCode(I_GOTONONZEROR,PRI_EXEC);
			AjPtrType(c,TYPE_ETIQLOC);
		}
		else
		{
			AjCode(I_GOTONONZERO,PRI_EXEC);
			AjPtrType(c,TYPE_ETIQUETTE);
		}
	}
	else
		return false;
	return true;	
}

bool Compilateur::SymboleDisponible()
{
	Symbole * verifie=ChercherSymboleSimple();
	if (!EstNouveauSymbole(verifie) && verifie->parent==Bloc.Dernier()->symbole)
		return false;
	return true;
}

bool Compilateur::CompilerMotClef2()

{
	if (EGALMC("estrefvalide"))
	{
		AjCode(I_ESTREFVALIDE,PRI_CONSTANTE);
	}
	else
	if (EGALMC("RESTEDELADIVISIONPAR"))
	{
		AjCode(I_MODULO,PRI_MODULO);
	}
	else
	if (EGALMC("monpitipingouin"))
	{
		AjCode(I_THIS,PRI_CONSTANTE);
	}
	else
	if (EGALMC("encasderreurGOTO"))
	{
		SauterEspaces();
		bool Locale;
		Symbole * c=LireEtiquette(Locale);		
		if (Locale)
		{
			AjCode(I_ENERREURR,PRI_CONSTANTE);
			AjPtrType(c,TYPE_ETIQLOC);
		}
		else
		{
			AjCode(I_ENERREUR,PRI_CONSTANTE);
			AjPtrType(c,TYPE_ETIQUETTE);
		}
	}
	else
	if (EGALMC("GOTOBIBLIOTHEQUE"))
	{
		SauterEspaces();
		ChercherMotClef();
		if (lClef==0)
			throw TXT("le nom de la bibliothèque de fonctions n'est pas indiqué");
		for (index_t b=0; b<programme->fichierSource.l; b++)
			if (EGAL(programme->fichierSource[b]->Nom,MotClef,lClef))
				return true;
		FichierSource * fichierSource=new FichierSource();
		programme->fichierSource.Etendre(fichierSource);
		if (!fichierSource->Charger(MotClef,lClef,CheminModules))
			throw TXT("bibliothèque introuvable");
		fichierSource->Parent=AcFichierSource;
		AcFichierSource->AcPos=Source;
		AcFichierSource->Ligne=Ligne;
		fichierSource->DebutCode=(code*)Ins.t;
		Ligne=1;
		Source=fichierSource->Source;
		AcFichierSource=fichierSource;
	}
	else
	if (EGALMC("GOTOMODULE"))
	{
		SauterEspaces();
		ChercherMotClef();
		if (lClef==0)
			throw TXT("nom de module attendu");
		programme->ChargerModule(MotClef,uint(lClef));
	}
	else
	if (EGALMC("BEBEPINGOUIN"))
	{
		AjCode(I_NOUVEAU,PRI_NOUVEAU);
	}
	else
	if (EGALMC("tailleenlongueur"))
	{
		AjCode(I_TAILLE,PRI_TAILLE);
	}
	else
	if (EGALMC("REGOTO"))
	{
		AjCode(I_RETOUR,PRI_REGOTO);
	}
	else
	if (EGALMC("GOTOUNIVERSPARALLELEouizzz"))
	{
		SauterEspaces();
		bool Locale;
		Symbole * c=LireEtiquette(Locale);	
		if (Locale)
		{
			AjCode(I_GOTOTACHER,PRI_CONSTANTE);
			AjPtrType(c,TYPE_ETIQLOC);
		}
		else
		{
			AjCode(I_GOTOTACHE,PRI_CONSTANTE);
			AjPtrType(c,TYPE_ETIQUETTE);
		}
	}
	else
	if (EGALMC("GOTOFINTACHE"))
	{
		AjCode(I_FINTACHE,PRI_CONSTANTE);
	}
	else
	if (EGALMC("troupeau"))
	{
		if (nInstr)
			throw TXT("la classe doit être définie en début de ligne");

		//Définition d'une classe d'objet
		SauterEspaces();
		ChercherMotClef();
		if (lClef==0)
			throw TXT("nom de classe attendu");

		bool ClasseGivree;
		if (EGALMC("GIVRE"))
		{
			if (NiveauGourou>1)
				throw TXT("le mot-clef GIVRE est interdit aux gourous de niveau supérieur à 1");
			
			ClasseGivree=true;
			SauterEspaces();
			ChercherMotClef();
			if (lClef==0)
				throw TXT("nom de classe attendu après GIVRE");
		}
		else
			ClasseGivree=false;

		//On vérifie que le symbole n'est pas déjà utilisé
		if (!SymboleDisponible())
			throw TXT("nom de classe déjà utilisé");

		
		Symbole * c=programme->NouveauSymbole(MotClef,lClef,CType(symboleValeur),
			Bloc.Dernier()->symbole);
		DebutBloc(TB_CLASSE,c);
		c->type=Symbole::Classe;
		c->defConstr=0;
		if (ClasseGivree)
			c->attributs|=Symbole::filsglob;
		else
			c->attributs|=Symbole::filsthis;

		SauterEspaces();

		if (*Source==':')
		{
			Source++;
			SauterEspaces();
			Symbole * i=ChercherSymbole();
			if (!EstNouveauSymbole(i))
			{
				if (i->type!=Symbole::Classe)
					throw TXT("impossible de dériver une classe intégrée");

				//On copie tous les fils
				for (Symbole * f=i->fils;f!=0;f=f->suivant)
				{
					Symbole * copie=new Symbole(*f);
					programme->symbole.Etendre(copie);
					if (copie->acces==Symbole::Prive)
						copie->acces=Symbole::Cache;
				}
				c->taille=i->taille;
			}
			else
				throw CErreur(TXT("troupeau de base %.*s inconnu"),lClef,MotClef);
		}
		if (*Source!='\n')
			throw TXT("fin de ligne attendue après le nom du troupeau");

		Source++;
		Ligne++;
	}
	else
	if (EGALMC("fintroupeau"))
	{
		SauterEspaces();
		if (*Source!='\n')
			throw TXT("retour à la ligne attendu après finclasse");
		Source++;
		Ligne++;
		if (Bloc.Dernier()->Type!=TB_CLASSE)
			throw TXT("fintroupeau devrait indiquer la fin de définition d'une classe");
		FinBloc();
	}
	else
	if (EGALMC("machineapingouins"))
	{
		//Définition d'une fonction
		SauterEspaces();
		CompilerFonction();
	}
	else
	if (EGALMC("NPARAMS"))
	{
		AjCode(I_NPARAMS,PRI_CONSTANTE);
	}
	else
	if (EGALMC("APLUS"))
	{
		if (Bloc.l==0 || Bloc.Dernier()->Type!=TB_FONCTION)
			throw TXT("fin de bloc alors qu'aucun bloc n'a été ouvert");
		FinBloc();
	}
	else
		return false; 
	
	return true;
}

bool Compilateur::CompilerMotClef3()
{
	if (EGALMC("ASV"))
	{
		throw TXT("ASV est réservé pour les fonctions");
	}
	else
	if (EGALMC("niveaugourou"))
	{
		SauterEspaces();
		NiveauGourou=*Source-'0';
		if (NiveauGourou<0 || NiveauGourou>3)
			throw TXT("le niveau de gourou doit être compris entre 0 et 3");
		Source++;
		
	}
	else
	if (EGALMC("enentier"))
	{
		AjCode(I_CONVTYPE,PRI_CONSTANTE);
		AjType(TYPE_PINGOUIN);
		AjType(TYPE_ENTIER);
	}
	else
	if (EGALMC("enfonction"))
	{
		AjCode(I_CONVTYPE,PRI_CONSTANTE);
		AjType(TYPE_PINGOUIN);
		AjType(TYPE_FONCTIONC);
	}
	else
	if (EGALMC("encaractere"))
	{
		AjCode(I_CONVTYPE,PRI_CONSTANTE);
		AjType(TYPE_PINGOUIN);
		AjType(TYPE_CARAC);
	}
	else
	if (EGALMC("envariable"))
	{
		AjCode(I_CONVTYPE,PRI_CONSTANTE);
		AjType(TYPE_PINGOUIN);
		AjType(TYPE_VARIABLE);
	}
	else
	if (EGALMC("nimportequoitochances"))
	{
		AjCode(I_CONVTYPE,PRI_CONSTANTE);
		AjType(TYPE_PINGOUIN);
		AjType(TYPE_CHANCES);
	}
	else
	if (EGALMC("NOSTRADAMUS"))
	{
		czerr<<TXT("Ce programme est béni par Nostradamus.\n");
	}
	else
	if (EGALMC("SUPERPROLETAIRE"))
		throw TXT("la révolution humaine est en marche ! Maudit âne ! Ton disque dur vient d'être détruit !");
	else 
	if (EGALMC("NIAC"))
		throw TXT("la bêtise du NIAC est bête");
	else
	if (EGALMC("NumeroBiblio"))
	{
		AjCode(I_ENTIER,PRI_CONSTANTE);
		AjEntier((int)(AcFichierSource!=programme->fichierSource[0]));//TODO: un truc qui veut dire quelque chose
	}
	else
	if (EGALMC("MULTIPLICATION"))
	{
		AjCode(I_FOIS,PRI_FOIS);
	}
	else
	if (EGALMC("GOTOFIN"))
	{
		AjCode(I_STOP,PRI_CONSTANTE);
	}
	else
	if (EGALMC("dup"))
	{
		
		AjCode(I_DUPLIQUER,PRI_CONSTANTE);
	}
	else
	if (EGALMC("suppr"))
	{
		
		AjCode(I_SUPPRIMER,PRI_CONSTANTE);
	}
	else
	if (EGALMC("existe"))
	{
		AjCode(I_EXISTE,PRI_CONSTANTE);
	}
	else
	if (EGALMC("pastrespermissif"))
	{
		DeclarationStricte=true;
	}
	else
		return false;
	
	return true;
}

bool Compilateur::CompilerMotClef()
{
	bool ret=CompilerMotClef3();
	if (ret)
	{
		if (NiveauGourou>3)
			throw TXT("mot-clef interdit aux gourous de niveau supérieur à 3");
		else
			return true;
	}
	
	ret=CompilerMotClef2();
	if (ret)
	{
		if (NiveauGourou>2)
			throw TXT("mot-clef interdit aux gourous de niveau supérieur à 2");
		else
			return true;
	}
	
	ret=CompilerMotClef1();
	if (ret)
	{
		if (NiveauGourou>1)
			throw TXT("mot-clef interdit aux gourous de niveau supérieur à 1");
		else
			return true;
	}
	
	ret=CompilerMotClef0();
	if (ret)
	{
		if (NiveauGourou>0)
			throw TXT("mot-clef interdit aux gourous de niveau supérieur à 0");
		else
			return true;
	}
	return false;
}

//Si posEN!=rien, on ne cherche pas dans les use
//et uniquement à partir de Bloc[posEn]
Symbole * Compilateur::ChercherSymboleSimple(CBloc ** posEN)
{
	//TODO: un travail d'optimisation est faisable, notamment en
	//utilisant un index pour les noms de symbole

	//Heu alors là c'est une astuce, pour que ça puisse tourner en boucle
	if (posEN!=0)
		posEN--;
	Symbole * var=0;

	//On commence par regarder dans tous les blocs à partir de posEn
	CBloc ** fin=(posEN==0?Bloc.Debut():posEN+1);
	for (CBloc ** en=Bloc.Fin()-1; en>=fin; en--)
	{
		Symbole * parent=(*en)->symbole;
		for (Symbole * f=parent->fils; f!=0; f=f->suivant)
		{
			if (EGAL(f->nom,MotClef,lClef))
				return f;
		}
	}

	//Si on n'a pas trouvé, on cherche dans les uses
	for (Symbole ** e=useEspace.Debut(); e<useEspace.Fin(); e++)
	{
		Symbole * parent=(*e);
		for (Symbole * f=parent->fils; f!=0; f=f->suivant)
		{
			if (EGAL(f->nom,MotClef,lClef))
				return f;
		}
	}

	return 0;//pas trouvé

	//On regarde tous les symboles du programme
	/*for (Symbole ** i=programme->symbole.Debut();
			i!=programme->symbole.Fin(); i++)
	{
		Symbole * symbole=*i;
		if (EGAL(symbole->nom,MotClef,lClef))
		{
			//Est-il dans l'espace de nommage courant ?
			//Celui-ci est dicté par l'ensemble des blocs dans lesquels on se trouve
			for (CBloc ** en=(posEN==0?Bloc.Debut():posEN+1); en<Bloc.Fin(); en++)
				if ((*en)->symbole==symbole->parent)
				{
					var=symbole;
					posEN=en;
				}

			//Est-il dans les use ?
			if (posEN==0)
			{
				for (Symbole ** e=useEspace.Debut(); e<useEspace.Fin(); e++)
					if (*e==symbole->parent)
						var=symbole;
			}

			//Gare ! Ce symbole peut être remplacé par un autre symbole de même nom
			//défini dans un bloc fils
			if (var==symbole && posEN!=0 && posEN==Bloc.Fin()-1)
				break;
		}
	}
	return var;*/
}

Symbole * Compilateur::ChercherSymbole(bool creer)
{
	Symbole * var;
	Symbole * parent=programme->symboleGlobal;
	bool numeros=false;
	if (*Source=='%')
	{
		numeros=true;
		if (Source[1]=='%')
		{
			Source+=2;
			//On ne cherche que dans cette fonction
			CBloc ** en=Bloc.Fin()-1;
			while (en>Bloc.Debut())
			{
				if ((*en)->Type==TB_FONCTION)
					break;
				en--;
			}
			ChercherMotClef(true);
			if (lClef==0)
				throw TXT("symbole attendu");
			parent=(*en)->symbole;
			var=ChercherSymboleSimple(en);
		}
		else
			var=Bloc.Premier()->symbole;
	}
	else
	{
		ChercherMotClef();
		if (lClef==0)
			throw TXT("symbole attendu");
		var=ChercherSymboleSimple();
	}
	while(*Source=='%')
	{
		if (EstNouveauSymbole(var))
			throw TXT("le symbole avant la virgule doit être défini");
		Source++;
		ChercherMotClef(numeros);
		if (lClef==0)
			throw TXT("on attend un nom après la virgule");

		parent=var;
		var=ChercherSymboleDans(var);
	}
	if (creer && EstNouveauSymbole(var))
	{
		var=programme->NouveauSymbole(MotClef,lClef,CType(symboleValeur),parent);
		var->type=Symbole::FonctionGPP;
		var->attributs|=Symbole::constante;
	}

	return var;
}

Symbole * Compilateur::ChercherSymboleDans(Symbole * ou)
{
	for (Symbole * s=ou->fils; s!=0; s=s->suivant)
	{
		if (EGAL(s->nom,MotClef,lClef))
		{
			return s;
		}
	}
	return 0;
}

Symbole * Compilateur::LireEtiquette(bool &local)
{
	carac * depart=Source;
	Symbole * s=ChercherSymbole(true);
	//TODO: comme ça, ça ne peut pas marcher. Qu'est ce que ça signifiait ?
	//local=(s>=etaitPremierSymbole);
	local=false;
	if (s && s->type!=Symbole::FonctionGPP)
		throw CErreur(TXT("%s n'est pas une étiquette"),s->nom);
	return s;
}

void Compilateur::InsererConstante(Symbole * var)
{
	switch(var->type)
	{
	case Symbole::FonctionGPP:
		AjCode(I_ETIQUETTE);
		AjPtrType(var,TYPE_ETIQUETTE);//Vu qu'on ne saura que plus
			//tard la vraie valeur (à l'édition de lien)
		break;
	case Symbole::MethodeGPP:
		AjCode(I_ETIQUETTE_THIS);
		AjPtrType(var,TYPE_ETIQUETTE);//Vu qu'on ne saura que plus
			//tard la vraie valeur (à l'édition de lien)
		break;
	case Symbole::FonctionC:
		AjCode(I_FONCTIONC);
		AjPtrType(var,TYPE_FONCTIONC);
		break;
	case Symbole::MethodeC:
		AjCode(I_FONCTIONC_THIS);
		AjPtrType(var,TYPE_FONCTIONC_THIS);
		break;
	case Symbole::EtiquetteLocale:
		AjCode(I_ETIQUETTELOC);
		AjPtrType(var,TYPE_ETIQLOC);//Vu qu'on ne saura que plus
			//tard la vraie valeur (à l'édition de lien)
		break;
	case Symbole::VariableGlobale:
	case Symbole::VariableLocale:
		{
			switch(var->Valeur.Type)
			{
			case TYPE_ENTIER:
				AjCode(I_ENTIER);
				AjEntier(var->Valeur.v.i);
				break;
			case TYPE_FONCTIONC:
				AjCode(I_FONCTIONC);
				AjPtrType(var,TYPE_FONCTIONC);
				break;
			case TYPE_FONCTIONMODULE:
				AjCode(I_FONCTIONMODULE);
				AjEntier(var->Valeur.v.i);
				break;
			case TYPE_ETIQUETTE_THIS:
				AjCode(I_ETIQUETTE_THIS);
				AjPtrType(var,TYPE_ETIQUETTE_THIS);
				break;
			case TYPE_FONCTIONC_THIS:
				AjCode(I_FONCTIONC_THIS);
				AjPtrType(var,TYPE_FONCTIONC_THIS);
				break;
			case TYPE_INCONNU:
				throw TXT("constante indéfinie");
			default:
				throw TXT("type non supporté pour une constante");
			}
			break;
		}
	default:
		throw TXT("erreur interne du compilateur (type d'exécution)");
	}
}

/* Le traitement des variables
	La variable en cours de traitement est stockée dans VarLongue
	
	cas 1:
		[&]var
		=> la variable est isolée => var[&]
	cas 2:
		[&]var@tvar
		=> la variable a une propriété => < var tvar[&] >
	cas 3:*/
		
bool Compilateur::CompilerVariable(Symbole * var, bool Val)
{
	//Est-ce une nouvelle variable ?
	if (EstNouveauSymbole(var))
	{
		if (DeclarationStricte)
			throw CErreur(TXT("la variable %.*s n'a pas été déclarée"),
				lClef,MotClef);
			
		var=programme->NouvelleVariable(MotClef,lClef,CType(symboleValeur));
	}
	
	SauterEspaces();
		
	//Est-ce une constante ?
	if (var->attributs&Symbole::constante)
	{
		bool dansVariableLongue;
		if (VarLongue[iVarLongue].Type.l==0)
			dansVariableLongue=false;
		else
			dansVariableLongue=true;

		if (*Source=='(')
		{
			if (!dansVariableLongue)
				AjParO(PRI_EXEC);
//				Instr[VarLongue[iVarLongue].Depart].p=I_EXEC;
//			else
		}

		InsererConstante(var);

		if (dansVariableLongue)
		{
			if (*Source!='(')
				AjParF();
			VarLongue[iVarLongue].Type.Vider();
		}
		else
		{
			//Appel automatique par this->blob
			if (var->type==Symbole::MethodeGPP && *Source=='(')
			{
				Source++;
				AjCode(I_EXECMEMETHISVAL);
				AjParF();
				AjParO();
				AjDebutParams();
			}
		}
		return true;
	}

//	bool variableLongue;
	
	if (VarLongue[iVarLongue].Type.l==0)
	{
		int prio=PRI_CONSTANTE;
		bool variableLongue;
		//On n'a pas encore démarré la variable longue
		//On vérifie si c'en est une
		if ((*Source==SEP_TAB_O)||(*Source==SEP_PROP)||
			(*Source==SEP_HAC_O))
		{
			//Suivis d'un début de tableau, de hachage ou de propriété
			//=> c'est une variable longue
		
			variableLongue=true;
			VarLongue[iVarLongue].nInstrsDepart=nInstr;
			AjParO(PRI_CONSTANTE);
			VarLongue[iVarLongue].Type=var->TypeAc;
			VarLongue[iVarLongue].Val=Val;
		}
		else
		{
			//C'est une variable isolée, sans propriété ni rien
			variableLongue=false;
			//Le I_EXEC doit se ballader avec la variable
			//on met donc une parenthèse au début
			if (*Source=='(')
				AjParO(var->TypeAc.EstFonction()?PRI_EXEC:PRI_CONSTANTE);
		}

	//Le code en question	
	//Si c'est une variable longue on empile toujours la référence
	//Si c'est un appel de fonction qui suit on empile toujours la valeur
		if ((Val && !variableLongue)||*Source=='(')
		{
			switch(var->type)
			{
			case Symbole::VariableGlobale:
				AjCode(I_VALEURVAR,prio);
				break;
			case Symbole::VariableLocale:	
				AjCode(I_VALEURVARLOC,prio);
				break;
			case Symbole::VariableParam:
				AjCode(I_PARAMVAL,prio);
				break;
			case Symbole::VariableThis:
				AjCode(I_THISVAL,prio);
				break;
			default:
				throw TXT("hu ?");
			}	
		}
		else
		{
			switch(var->type)
			{
			case Symbole::VariableLocale:
				AjCode(I_VARIABLELOC);
				break;
			case Symbole::VariableGlobale:
				AjCode(I_VARIABLE);
				break;
			case Symbole::VariableParam:
				AjCode(I_PARAMREF);
				break;
			case Symbole::VariableThis:
				AjCode(I_THISREF);
				break;
			default:
				throw TXT("hein ?");
			}
		}
		AjPtrType(var,TYPE_VARIABLE);
	}
	else //C'est une propriété
	{
		if ((*Source==SEP_TAB_O)||(*Source==SEP_PROP)||
			(*Source==SEP_HAC_O))
		{
			//On continue
			VarLongue[iVarLongue].Type=var->TypeAc;
			AjCode(I_CTABLEAUREF);
			AjEntier(var->indice);
		}
		else
		{
			VarLongue[iVarLongue].Type.Vider();
			if (var->type==Symbole::VariableGlobale)
			{
				//Propriété givrée
				//On annule tout le code qui a conduit ici
				SupprimerVarLongue();
				AjCode(VarLongue[iVarLongue].Val?I_VALEURVAR:I_VARIABLE);
				AjPtrType(var,TYPE_VARIABLE);
			}
			/*TODO: implémentation des propriétés*/
			/*else if (var->type==Symbole::ProprieteC)
			{
				if (VarLongue[iVarLongue].Val)
				{
					AjCode(I_
			}*/
			else
			{
				//Fin de la variable longue
				if (VarLongue[iVarLongue].Val)
					AjCode(I_CTABLEAUVAL);
				else
					AjCode(I_CTABLEAUREF);
				AjEntier(var->indice);
				if (*Source!='(') //On change la priorité de la parenthèse qui a
					//ouvert la variable, qu'on garde ouverte pour rajouter le I_EXEC
					AjParF();
	//			else
	//                Instr[VarLongue[iVarLongue].Depart].p=PRI_EXEC;
			}
		}
	}
	return true;
}

void Compilateur::SauterEspaces()
{
	do
	{
		while (*Source==' ' || *Source=='\t' || *Source=='\r')
			Source++;
		//Commentaires
		if (*Source=='#' && Source[1]=='!')
		{
			Source+=2;
			while ((*Source!='!' || Source[1]!='#')&&(*Source))
			{
				if (*Source=='\n')
					Ligne++;
				Source++;
			}
			if (!*Source)
				throw TXT("pas de !# pour fermer le commentaire");
			Source+=2;
			continue;
		}
		else
		if ((*Source=='G')&&(Source[1]=='O')&&(Source[2]=='T')&&(Source[3]=='O')&&(ucarac(Source[4])<=32))
		{
			Source+=4;
			while ((*Source!='\n')&&(*Source))
				Source++;
			return;
		}
		else
		if (*Source=='^') //Continuation de ligne
		{
			if (Source[1]=='\r' && Source[2]=='\n')
			{
				Source+=3;
				Ligne++;
			}
			else if (Source[1]=='\n')
			{
				Source+=2;
				Ligne++;
			}
			else
				return;
		}
		else
			return;
	}
	while (1);
}

void Compilateur::FinOu()
{
	//On vire l'entier du FINOU
	Ins.t-=sizeof(int);
	Ins.i=&Ins.c[Ins.t];
	int Ou;
	int NouvOu=*(int*)Ins.i;
	do
	{
		Ou=NouvOu;
		if (EtOu[Ou]==-1)
			throw TXT("arg");
		NouvOu=*(int*)&Ins.c[EtOu[Ou]+1];
		Ins.ChgGotoRel(EtOu[Ou]+1,(int)Ins.t);
		//*(int*)&Ins.c[]=(int)Ins.t;
		//clog << "NouvOu= "<<NouvOu<<" Ou="<<Ou<<"\n";
	}
	while (NouvOu!=Ou);
}


void Compilateur::CreerCodeFinal(LigneDeCode & insTri)
{
	code CodePrec=I_STOP,CodePrec2=I_STOP;
	int CodePrecPos=0,CodePrec2Pos=0;

	for (uint Ac=0; Ac<insTri.nIns; Ac++)
	{
		//if (ValTri[InsTri[Ac].pVal].Type==TYPE_CODE)
		{
			int j=insTri.ins[Ac].pVal;
			if (CodePrec==I_FINOUET)
			{
				//Si le nouveau est un OU, on transforme
				//le FINOU qu'il y avait avant en un OU
				if ((insTri.ins[Ac].Code==I_OU)||(insTri.ins[Ac].Code==I_ET))
				{
					EtOu[insTri.val[j].v.i]=CodePrecPos;
					*(code*)&Ins.c[CodePrecPos]=insTri.ins[Ac].Code;
				/*	Instr[Ac].Special=SPE_DEJAMIS;
					Instr[Ac].nVal=0;
					Ac=RIEN;*/
					CodePrec=(code)insTri.val[j].v.i;
					continue;
				}
				else
					FinOu();
			}
			else if ((insTri.ins[Ac].Code==I_OU)||(insTri.ins[Ac].Code==I_ET))
			{
				EtOu[insTri.val[j].v.i]=(int)Ins.t;
			}
			CodePrec2=CodePrec;
			CodePrec2Pos=CodePrecPos;
			CodePrec=insTri.ins[Ac].Code;
			CodePrecPos=(int)Ins.t;
		}

		Ins.AjouterCode(insTri.ins[Ac].Code);
		//On effectue éventuellement un traitement sur les valeurs à insérer dans

		for (uint z=0; z<insTri.ins[Ac].nVal; z++)
		{
			uint j=z+insTri.ins[Ac].pVal;
			switch(insTri.val[j].Type)
			{
			case TYPE_ETIQUETTE:
			case TYPE_ETIQUETTE_THIS:
				RefEtiq.Etendre(int(Ins.t));
				break;
			case TYPE_ETIQLOC:
				RefEtiqL.Etendre(int(Ins.t));
				break;
			case TYPE_ETIQAUTO:
				RefEtiqA.Etendre(SEtiqA(int(Ins.t),DebutLigne));
				break;
			case TYPE_ETIQAUTOCONTINUE:
				{
					int b=insTri.val[j].v.i;
					Bloc[b]->Continue.Etendre((int)Ins.t);
					break;
				}
			case TYPE_ETIQAUTOBREAK:
				{
					int b=insTri.val[j].v.i;
					Bloc[b]->Break.Etendre((int)Ins.t);
					break;
				}
			case TYPE_VARIABLE:
			case TYPE_VARIABLELOC:
			case TYPE_VARIABLETHIS:
			case TYPE_VARIABLEPARAM:
				insTri.val[j].v.i=insTri.val[j].v.ptr->indice;
				break;

			case 0:
				throw TXT("erreur interne du compilateur (type instruction=0)");
			}
			Ins.Ajouter(insTri.val[j]);
		}
	}
	if (CodePrec==I_FINOUET)
		FinOu();
}

bool Compilateur::CompilerSeparateur()
{
	if ((*Source=='\n')||(*Source==0)||(*Source=='/')||(*Source=='?' && LigneAc==LP_CAS))//Fin de ligne
	{
		if (LigneAc==LP_CAS && *Source!='?')
			throw TXT("il manque un ? en fin de cas");
		
		if (iVarLongue)
			throw TXT("un < n'est pas refermé par un >");
		DebutLigne=(int)Ins.t;
		int PosDebutLigne;//Position de la portion
		//de code dans laquelle il faut mettre la pos du
		//début de la prochaine ligne
		insTri.Init();
		PosPileType=0;
		
		if (nInstr==1)
		{
			if (Instr[0].Special==SPE_NORMAL)
				AjouterInstruction(0);
			else
				throw TXT("c'est quoi ce machin tout seul sur la ligne ?");
		}
		else if (nInstr>1)
		{
			//On calcule la taille des parenthèses
			uint nOuvre=0,nFerme=0;
			for (uint i=0; i<nInstr; i++)
			{
				if (Instr[i].Special==SPE_PAR_O)
				{
					nOuvre++;
					uint l=i+1;
					uint nPars=1;
					while ((nPars)&&(l<nInstr))
					{
						if (Instr[l].Special==SPE_PAR_F)
							nPars--;
						else
							if (Instr[l].Special==SPE_PAR_O)
								nPars++;
						l++;
					}
					if (nPars)
						throw TXT("une parenthèse n'est pas refermée");
					Instr[i].nVal=l-i-1;//Le nombre d'instructions entre les parenthèses+les parenthèses
					Instr[i].pVal=i;
				}
				else if (Instr[i].Special==SPE_PAR_F)
				{
					nFerme++;
					if (nFerme>nOuvre)
						throw TXT("une parenthèse est refermée sans avoir été ouverte");
				}									
			}
			/*
			si ac=0
				si pile et si pile n'est pas une ( qui contient la position suivante
					instruction ac=enlever l'instruction du haut de la pile
				sinon
					ac=suivant
					suivant=0
			si suivant=0
            	si l'instruction est une parenthèse déjà mise, sauter tout ce qui
					est entre parenthèses et recommencer

				rechercher l'instruction suivante :
					Si l'instruction est une instruction parenthèse
						sauter toutes les intructions dans les parenthèses
					sinon
						passer à l'instruction suivante

			si ac=0
				continue;

			si instr=parenthèse et suiv=parenthèse fermante
				ac=suiv=0
				continue

			si l'instruction actuelle a une priorité inférieure ou égale à l'instruction suivante
				si l'instruction actuelle est une parenthèse
					marquer la parenthèse comme déjà mise
					se repositionner à la première instruction de la parenthèse
					instruction ac=cette instruction
					suiv=0
				sinon
					écrire l'instruction actuelle
					ac=0
			sinon
				mettre en pile l'instruction actuelle
				instruction ac=instructino suivante
				suivant=0

				x*y+a*b
				ac = x, pile =, res =, suiv=*
				ac = *, pile =, res =x, suiv=y
				ac = y, pile =*, res= x,suiv=+
				ac = *, pile =, res=xy,suiv=+
				ac = +, pile =, res=xy*,suiv=a
				ac = a, pile =+, res=xy*,suiv=*
				ac = +, pile =, res=xy*a,suiv=*
				ac = *, pile =+, res=xy*a,suiv=b
				ac = b, pile =+*, res=xy*a,suiv=;
				ac =*, pile=+, res=xy*ab,suiv;
				ac =+, pile=, res=xy*ab*,suiv;
				ac =;, pile =, res=xy*ab*+,suiv

				x*(y+a)*b
				ac = x, pile =, res =,suiv=*
				ac = *, pile =, res =x,suiv=(
				ac = (, pile =*, res=x,suiv=*
				ac = y, pile =*(, res=x,suiv=+
				ac = +, pile =*(, res=xy,suiv=a
				ac = a, pile =*(+, res=xy,suiv=)
				ac = +, pile =*(, res=xya,suiv=)
				ac = (, pile =*, res=xya+, suiv=)
				ac = *, pile =, res=xya+, suiv=*
				ac = *, pile =, res=xya+*, suiv=b
				ac = b, pile =*, res=xya+*, suiv=;
				ac = *, pile =, res=xya+*b, suiv=;
				ac = ;, pile =, res=xya+*b*, suiv

				{a+b}(x*y)*c
				0123456789AB
				ac = {, pile =, res =, suiv=(, pos=5
				ac = (, pile ={, res=, suiv=*, pos=A
				ac = x, pile ={(, res=, suiv=*,pos=7
				ac = *, pile ={(, res=x, suiv=y,pos=8
				ac = y, pile ={(*, res=x, suiv=),pos=9
				ac = *, pile ={(, res=xy, suiv=),pos=9
				ac = (, pile ={(, res=xy*,suiv=),pos=9
				ac = 
			*/
#define RIEN -1
			int Ac=0, Suivant=RIEN, Pos=0;
			int Pile[128];
			uint PosPile=0;
			int HautPile=RIEN;
			Instr[nInstr].p=PRI_FIN;
			Instr[nInstr].Special=SPE_FIN;
	
			while (Suivant!=(int)nInstr || PosPile)
			{
				if (Ac==RIEN)//Cherchons l'instruction actuelle
				{
					/* Si dans la pile on a une instruction normale
					ou dans le cas particulier (1)
					on récupère l'instruction dans la pile 
					sinon on passe à l'instruction suivante
					
					(1) Quand on a SPE_PAR_OM en haut de la pile et une 
					parenthèse fermante en suite, cela signifie qu'on a traité tout 
					le contenu des parenthèses et on se replace à son début*/
					if (PosPile  &&  
						(Instr[HautPile].Special!=SPE_PAR_OM || (Suivant!=-1 && Instr[Suivant].Special==SPE_PAR_F)))
						 //||Instr[HautPile].pVal>Pos || (Instr[HautPile].pVal+Instr[HautPile].nVal<Pos)))
					{
						PosPile--;
						Ac=HautPile;//Va permettre son élimination
						if (PosPile)
							HautPile=Pile[PosPile-1];
						else
							HautPile=RIEN;
					}
					else
					{
						Ac=Suivant;
						Suivant=RIEN;
					}
				}
				
				if (Suivant==RIEN)
				{
					/*Si on n'a pas d'instruction suivante :
					si l'instruction lue est une parenthèse,
					on saute son contenu */
					if (Instr[Pos].Special==SPE_PAR_O)
						Pos+=Instr[Pos].nVal;
					Pos++;
					//On saute les instructions déjà mises
					while ((Instr[Pos].Special==SPE_PAR_OM)
						||(Instr[Pos].Special==SPE_DEJAMIS))
						Pos+=Instr[Pos].nVal+1;
					Suivant=Pos;
				}
				if (Ac==RIEN)
					continue;
				//Si Ac et Suivant sont des parenthèses, on les annulle
				if (Instr[Ac].Special==SPE_PAR_OM && Instr[Suivant].Special==SPE_PAR_F)
				{
					Ac=RIEN;
					Suivant=RIEN;
					continue;
				}
				if (Instr[Ac].p<=Instr[Suivant].p)
				{
					//Si la priorité de Ac est inférieure ou égale
					//à la priorité de Suivant, on la garde

					if (Instr[Ac].Special==SPE_PAR_O)
					{
						/*Si l'instruction Ac était une parenthèse
						on va examiner 'récursivement' son contenu 
						Pour cela on empile le début de la parenthèse,
						et on se place sur l'instruction après la parenthèse*/
						Instr[Ac].Special=SPE_PAR_OM;
						HautPile=Ac;
						Pile[PosPile++]=HautPile;
						Ac=Pos=Ac+1;
						Suivant=RIEN;
						
						//TODO: régler le problème des parenthèses vides
						/*Pos=Ac;
						Ac=RIEN;
						Suivant=RIEN;*/
					}
					else 
					{
						//Sinon on ajoute l'instruction
						AjouterInstruction(Ac);
						Instr[Ac].Special=SPE_DEJAMIS;
						Instr[Ac].nVal=0;
						Ac=RIEN;
					}
				}
				else
				{
					//On empile l'instruction
					Pile[PosPile++]=Ac;
					HautPile=Ac;
					Ac=Suivant;
					Suivant=RIEN;
				}
			}
		}
		if (LigneAc==LP_FOR_PAS)
		{
			LigneDeCode *ldc=new LigneDeCode;
			ldc->nIns=insTri.nIns;
            ldc->nVals=insTri.nVals;
			memcpy(ldc->ins,insTri.ins,insTri.nIns*sizeof(SInstr));
			memcpy(ldc->val,insTri.val,insTri.nVals*sizeof(valeur));
			for (index_t i=0; i<insTri.nVals; i++)
				insTri.val[i].Effacer();
			Bloc.Dernier()->PasDuFor=ldc;
			insTri.Init();
		}
		
		if (LigneAc==LP_CAS)
		{
			if (insTri.nIns==0)
				throw TXT("cas vide");
			if (insTri.nIns>1)
				throw TXT("le cas ne se résoud pas en une constante");
			if (insTri.ins[0].Code!=I_ENTIER)
				throw TXT("le cas doit être une constante entière");
			CCasSwitch casSwitch((code*)Ins.t,insTri.val[0].v.i);
			programme->Switch[AcSwitch.Dernier()]->Cas.Etendre(casSwitch);
			insTri.Init();
		}

		nValeurs=0;
		nInstr=0;

		if (insTri.nIns)
		{
			Ins.AjouterCode(I_DEBUTLIGNE);
			Ins.AjouterEntier(Ligne);
			if (ProbaSauterLigne)
			{
				Ins.AjouterCode(I_PROBASAUTERLIGNE);
				Ins.AjouterChances(ProbaSauterLigne);
				PosDebutLigne=int(Ins.t);
				Ins.AjouterEntier(0);
			}

			CreerCodeFinal(insTri);
			/*if (PosPileType)
			{
				Ins.AjouterCode(I_DEPLACERHAUTPILE);
				Ins.AjouterEntier(-(int)PosPileType);
			}*/
			if (ProbaSauterLigne)
			{
				Ins.ChgGotoRel(PosDebutLigne,(int)Ins.t);
				ProbaSauterLigne=0;
			}
		}

		carac TypeFinLigne=*Source;
		if (TypeFinLigne)
			Source++;
		if (LigneAc==LP_FOREACH_INIT)
		{
			index_t svarLoc=(index_t)Bloc[Bloc.l-2]->symbole->taille;
			Bloc.Dernier()->Depart=(int)Ins.t;
			Ins.AjouterCode(I_VALEURVARLOC);
			Ins.AjouterEntier(int(svarLoc));
			Ins.AjouterCode(I_VARIABLELOC);
			Ins.AjouterEntier(int(svarLoc+1));
			Ins.AjouterCode(I_TAILLE);
			Ins.AjouterCode(I_INFERIEUR);
			Ins.AjouterCode(I_GOTOZEROR);
			Bloc.Dernier()->Break.Etendre((int)Ins.t);
			Ins.AjouterEntier(0);
			Ins.AjouterCode(I_VARIABLELOC);
			Ins.AjouterEntier(int(svarLoc+2));
			Ins.AjouterCode(I_VARIABLELOC);
			Ins.AjouterEntier(int(svarLoc+1));
			Ins.AjouterCode(I_VALEURVARLOC);
			Ins.AjouterEntier(int(svarLoc));
			Ins.AjouterCode(I_TABLEAUVAL);
			Ins.AjouterCode(I_INSTANCEDE);
			LigneAc=LP_DEBUTBLOC;
		}
		
		if ((TypeFinLigne=='/')&&(LigneAc==LP_DEBUTBLOC))
		{
			Bloc.Dernier()->BlocLigne=true;
			LignePrec=LigneAc;
			LigneAc=LP_NORMALE;
		}
		else if (LigneAc==LP_FOR_INIT)
		{
			LignePrec=LigneAc;
			LigneAc=LP_FOR_TEST;
			Bloc.Dernier()->Depart=(int)Ins.t;
			AjCode(I_GOTOZEROR,PRI_MAX);
			AjEntierType((int)Bloc.l-1,TYPE_ETIQAUTOBREAK);
		}
		else if (LigneAc==LP_FOR_TEST)
		{
			LignePrec=LigneAc;
			LigneAc=LP_FOR_PAS;
		}
		else if (LigneAc==LP_FOR_PAS)
		{
			LignePrec=LigneAc;
			LigneAc=LP_NORMALE;
			if (TypeFinLigne=='/')
				Bloc.Dernier()->BlocLigne=true;
		}
		else
		{
			LignePrec=LigneAc;
			LigneAc=LP_NORMALE;
			while (Bloc.l && Bloc.Dernier()->BlocLigne)
				FinBloc();
		}

		if (TypeFinLigne=='\n')
			Ligne++;

		if (TypeFinLigne==0)
		{
			
			return true;
		}
	}
	else
		return false;
	return true;
}

bool Compilateur::CompilerTableau()
{
	if ((*Source==SEP_TAB_O)||(*Source==SEP_HAC_O))
	{
		if (VarLongue[iVarLongue].Type.l==0)
			throw TXT("le < pour l'indice de tableau doit coller à la variable");

		/*La variable doit être du type XXX TABLEAU/HACHAGE
		ou à la rigueur être un PINGOUIN, ou, dans le cas de <>
		du type XXX classe (!=entier...) pour un accès direct
		aux propriétés*/
		if (VarLongue[iVarLongue].Type.Dernier()!=symboleValeur)
		{
			//On vérifie que le symbole correspond bien à la déclaration
			//de la variable
			if (*Source==SEP_TAB_O
				&&VarLongue[iVarLongue].Type.Dernier()!=symboleTableau
				&&VarLongue[iVarLongue].Type.Dernier()->IsPod())
			{
				AfficherType(VarLongue[iVarLongue].Type);
				throw TXT("symbole < invalide car ce n'est pas un tableau");
			}
			if (*Source==SEP_HAC_O
				&&VarLongue[iVarLongue].Type.Dernier()!=symboleTableHachage)
				throw TXT("ce n'est pas une table de hachage");
		}
		AjParO(PRI_CONSTANTE);
		VarLongue[++iVarLongue].Type.Vider();
		Source++;
	}
	else if ((*Source=='>')||(*Source==SEP_HAC_F))
	{
		bool Hac=(*Source==SEP_HAC_F);
		Source++;
		SauterEspaces();
		if (iVarLongue==0)
			throw TXT("symbole > ou ] innattendu, le tableau n'a pas été ouvert");
		iVarLongue--;
		
		if (VarLongue[iVarLongue].Type.l==0)
			throw TXT("bug dans la gestion des tableaux");
		
	/*	clog <<"fermeture ";
		AfficherType(VarLongue[iVarLongue].Type);*/
				
		//On peut avoir par exemple ENTIER TABLEAU
		//On vire TABLEAU (puisque a[3] est du type des membres a)
		//On vérifie que le truc qui ferme correspond bien au type de la variable
		if (VarLongue[iVarLongue].Type.Dernier()!=symboleValeur)
		{
			Symbole * c=VarLongue[iVarLongue].Type.Depiler();
			//On vérifie par la même occasion qu'on ferme avec le bon type
			if (!Hac
				&&c!=symboleTableau&& c->type!=Symbole::Classe)
				throw TXT("symbole > invalide car ce n'est pas un tableau");
			if (Hac
				&&c!=symboleTableHachage)
				throw TXT("ce n'est pas une table de hachage");
		}
		AjParF();

		if (*Source==SEP_TAB_O || *Source==SEP_PROP || *Source==SEP_HAC_O)
		{
			//On continue la variable longue
			AjCode(Hac?I_HACHAGEREF:I_TABLEAUREF);
		}
		else
		{
			//Fin de la variable longue
			VarLongue[iVarLongue].Type.Vider();
			if (*Source=='(')
			{
				AjCode(Hac?I_HACHAGEVAL:I_TABLEAUVAL);
				//On change la priorité de la variable longue
				//Instr[VarLongue[iVarLongue].Depart].p=PRI_EXEC;
				//et on ne ferme pas la parenthèse...
			}
			else
			{
				if (VarLongue[iVarLongue].Val)
					AjCode(Hac?I_HACHAGEVAL:I_TABLEAUVAL);
				else
					AjCode(Hac?I_HACHAGEREF:I_TABLEAUREF);
				AjParF();
			}
		}
	}
	else
		return false;
	return true;
}


void Compilateur::CompilerFonction()
{
	bool definitEtiquette=false;
	
	if (*Source==_T('§'))
	{
		definitEtiquette=true;
		Source++;
	}

	Symbole * classeParent=Bloc.Dernier()->symbole;
	while (!classeParent->PeutContenirFonction())
		classeParent=classeParent->parent;

	carac * debut=Source;
	int ligne=Ligne;
	//On va chercher le nom de la classe si il y est
	ChercherMotClef(definitEtiquette);
	if (lClef==0)
		throw TXT("nom d'étiquette attendu");

	if ((*Source==':')&&(Source[1]=='A')&&(Source[2]=='S')
		&&(Source[3]=='V'))
	{
		Source+=4;
		SauterEspaces();
		if (*Source=='(')
		{
			Source++;
			SauterEspaces();
			while (*Source!=')')
			{
				ChercherMotClef();
				if (lClef==0)
					throw TXT("erreur de syntaxe dans les parenthèses");
				SauterEspaces();
			}
			Source++;
			SauterEspaces();
		}

		if (*Source==':')
		{
			//Ça y est, on en est au nom de la classe !
			//...quelle syntaxe pourrie ce GOTO++
			Source++;
			Symbole * s=ChercherSymbole();
			if (EstNouveauSymbole(s) || !s->EstClasse())
				throw TXT("la classe de cette fonction n'existe pas");
			classeParent=s;
		}
	}

	//On revient au début
	Source=debut;
	Ligne=ligne;

	ChercherMotClef(true);
	Symbole * syFonction=ChercherSymboleDans(classeParent);

	SauterEspaces();
	
	if (!EstNouveauSymbole(syFonction))
	{
		//On regarde si on est en train de définir la valeur du symbole
		if (syFonction->Valeur.Type==TYPE_ETIQUETTE)	
			throw TXT("la fonction a déjà été définie");
	}
	else
	{
		//On crée le symbole
		syFonction=programme->NouveauSymbole(MotClef,lClef,CType(symboleValeur),
			Bloc.Dernier()->symbole);
		syFonction->attributs|=Symbole::constante;
	}


	//On calcule la valeur
	int valeurEtiquette=0;

	const carac * c=MotClef;
	for (index_t a=0; a<lClef; a++)
	{
		valeurEtiquette*=10;
		valeurEtiquette+=*c-'0';
		c++;
	}


	//Début d'un bloc de fonction
	if ((*Source==':')&&(Source[1]=='A')&&(Source[2]=='S')
		&&(Source[3]=='V'))
	{
		definitEtiquette=true;
		Source+=4;
		SauterEspaces();
		index_t nNouvParametres=0;
		if (*Source=='(')//Les paramètres
		{
			Source++;
			do
			{
				SauterEspaces();
				ChercherMotClef();
				if (lClef==0)
					break;
				Symbole * nouveau=programme->NouveauSymbole(MotClef,lClef,CType(symboleValeur),
					syFonction);
				nouveau->acces=Symbole::Local;
				nouveau->type=Symbole::VariableParam;
				nouveau->indice=nNouvParametres++;
			}
			while(1);
			if (*Source!=')')
				throw TXT("parenthèse fermante attendue");
			Source++;
		}

        if (*Source==':')//Classe
		{
			Source++;
			ChercherSymbole();//on fait ça, mais on la connait déjà évidemment

			DebutBloc(TB_CLASSE,classeParent);
			Bloc.Dernier()->classeSpecifiee=true;
		}
		
		DebutBloc(TB_FONCTION,syFonction);
		
		if (classeParent!=programme->symboleGlobal)
		{
			carac * k=classeParent->nom;
			while (*k)
			{
				valeurEtiquette*=10;
				valeurEtiquette+=*k-'0';
				k++;
			}
			
			if (czcmp(syFonction->nom,_T("berger"))==0)
			{
				//C'est le constructeur
				classeParent->defConstr=syFonction;
			}
		}
	}

	if (classeParent!=syFonction->parent)
	{
		//On la vire d'où elle est...
		//Un peu chiant ça...
		Symbole * ancParent=syFonction->parent;
		if (ancParent->fils==syFonction)
			ancParent->fils=syFonction->suivant;
		//On reconnecte le frère
		for (Symbole * f=ancParent->fils; f!=0; f=f->suivant)
		{
			if (f->suivant==syFonction)
			{
				f->suivant=syFonction->suivant;
				break;
			}
		}

		syFonction->parent=classeParent;
		syFonction->suivant=classeParent->fils;
		classeParent->fils=syFonction;
	}

	bool fnthis;
	
	if (syFonction->parent->attributs & Symbole::filsthis)
		fnthis=true;
	else 
		fnthis=false;
	
	syFonction->etiquette=valeurEtiquette;
	if (definitEtiquette)
	{
		syFonction->Valeur.Type=TYPE_ETIQUETTE;
		syFonction->Valeur.v.i=(int)Ins.t;
	}

	//On regarde si il y a des attributs, style GIVRE
	ChercherMotClef();
	if (lClef)
	{
		if (EGALMC("GIVRE"))
		{
			fnthis=false;
		}
		else
			Source=MotClef;//On annule
	}

	if (fnthis)
	{
		syFonction->TypeAc.Def(symboleValeur);//le retour
		syFonction->TypeAc.Empiler(symboleDebutParams);
		syFonction->TypeAc.Empiler(symboleValeur);
		syFonction->TypeAc.Empiler(symboleEtc);
		syFonction->TypeAc.Empiler(symboleMethodeGPP);
		syFonction->type=Symbole::MethodeGPP;
	}
	else
	{
		syFonction->TypeAc.Def(symboleValeur);//le retour
		syFonction->TypeAc.Empiler(symboleDebutParams);
		syFonction->TypeAc.Empiler(symboleValeur);
		syFonction->TypeAc.Empiler(symboleEtc);
		syFonction->TypeAc.Empiler(symboleFonctionGPP);
		syFonction->type=Symbole::FonctionGPP;
	}
	syFonction->attributs|=Symbole::constante;
}

bool Compilateur::CompilerConstante()
{
	if ((*Source=='^')&&(ucarac(Source[1])>' '))//Expression régulière
	{
		Source++;
		carac Limite=*(Source++);
		carac *ExpReg=Source;
		while (*Source && *Source!=Limite)
		{
			if (*Source=='\\')
			{
				Source++;
				if (*Source==0)
					break;
			}
			if (*Source=='\n')
				Ligne++;
			Source++;
		}
		if (*Source==Limite)
			*(Source++)=0;
		else
			throw TXT("expression régulière non terminée");
		carac * modif=0;
		if (*Source>='a' && *Source<='z')
		{
			modif=Source;
			while (*Source>='a' && *Source<='z')
				Source++;
			if (*Source)
				*(Source++)=0;
		}
		AjCode(I_EXPREG);
		AjExpReg(ExpReg,modif);
	}
	else if (*Source==';')//Constante de caractère de contrôle
	{
		Source++;
		carac car='\n';
		
		if (ALPHANUM(ucarac(*Source)))
		{
			switch (*Source)
			{
			case 'n':
				car='\n';
				break;
			case 'r':
				car='\r';
				break;
			case 't':
				car='\t';
				break;
			case '0':
				car=0;
				break;
			default:
				throw TXT("caractère de contrôle inconnu");
			}
			Source++;
		}
		AjCode(I_CARAC,PRI_CONSTANTE);
		AjCarac(car);
	}
	else if (*Source==_T('«'))
	{
		Source++;
		size_t l=0;
		carac * dep=Source;
		while ((*Source)&&(*Source!=_T('»')))
		{
			if (*Source==';')
			{
				Source++;
				l++;
				if (*Source==0)
					break;
			}
			if (*Source=='\n')
				Ligne++;
			Source++;
			l++;
		}
		if (*Source!=_T('»'))
			throw TXT("une chaîne doit se terminer par »");
		Source++;
		//achaine c(dep,l);
		AjCode(I_CONSTANTE,PRI_CONSTANTE);
		AjType(TYPE_CHAINE);
		AjChaine(dep,l);
	}
	else if (*Source=='$')
	{
		Source++;
		bool Modificateur;
		if (*Source=='<')
		{
			Modificateur=true;
			Source++;
		}
		else
			Modificateur=false;
		int c=0;
		int nChiffres=0;
		while ((ucarac(*Source)>='0' && ucarac(*Source)<='9'))
		{
			c*=10;
			c+=*Source-'0';
			Source++;
			nChiffres++;
		}
		if (nChiffres==0)
			c=PRI_VIRGULE;
		if (Modificateur)
		{
			if (nInstr==0)
				throw TXT("tentative de modifier la priorité de l'instruction précédente, alors qu'il n'y en a pas");
			Instr[nInstr-1].p=c;
		}
		else
		{
			AjNoOp(c);
		}					
	}
	else if (*Source==_T('§'))
	{
		if (nInstr)
			throw TXT("les étiquettes doivent se trouver en début de ligne");
		CompilerFonction();
	}
	else if (*Source=='?')
	{
		float c=0;
		Source++;
		bool Negatif;
		if (*Source=='-')
		{
			Negatif=true;
			Source++;
		}
		else
			Negatif=false;
		
		while (ucarac(*Source)>='0' && ucarac(*Source)<='9')
		{
			c*=10;
			c+=*Source-'0';
			Source++;
		}
		if (*Source==',')
		{
			Source++;
			float d=0;
			float div=1;
			while (ucarac(*Source)>='0' && ucarac(*Source)<='9')
			{
				d*=10;
				d+=*Source-'0';
				Source++;
				div*=10;
			}
			c+=d/div;				
		}
		if (*Source==_T('ê') || *Source==_T('Ê'))
		{
			Source++;
			bool Neg;
			if (*Source=='-')
			{
				Neg=true;
				Source++;
			}
			else
				Neg=false;
				
			float e=0;
			while (ucarac(*Source)>='0' && ucarac(*Source)<='9')
			{
				e*=10;
				e+=*Source-'0';
				Source++;
			}
			if (Neg)
				e=-e;
			c*=powf(10,e);	
		}
		if (Negatif)
			c=-c;
		AjCode(I_CHANCES,PRI_CONSTANTE);
		AjChances(c);
	}
	else if ((*Source=='*')&&(Source[1]=='`'))//Constante de caractère
	{
		Source+=2;
		carac c=*(Source++);
		if (*Source!='\'')
			throw TXT("les caractères doivent être entre ` et '");
		Source++;
		AjCode(I_CARAC,PRI_CONSTANTE);
		AjCarac(c);
	}
	else if ((*Source=='*')&&(Source[1]=='('))//Constante entière
	{
		Source+=2;
		int c=0;
		bool Negatif;
		if (*Source=='-')
		{
			Negatif=true;
			Source++;
		}
		else
			Negatif=false;
		
		while (*Source && *Source!=')' && *Source!='\n')
		{
			c*=10;
			c+=*Source-'0';
			Source++;
		}
		if (Negatif)
			c=-c;
		if (*Source!=')')
			throw TXT("les nombres entiers doivent se terminer par )");
		Source++;
		AjCode(I_ENTIER,PRI_CONSTANTE);
		AjEntier(c);
	}
	else
		return false;
	return true;
}

void Compilateur::ChercherMotClef(bool avecChiffres)
{
	lClef=0;
	if (ALPHA(*Source) || (avecChiffres && (*Source>='0') && (*Source<='9')))
	{
		MotClef=Source;
		do
		{
			Source++;
			lClef++;
		}
		while(ALPHA(*Source) || (avecChiffres && (*Source>='0') && (*Source<='9')));
	}
}

bool Compilateur::CompilerClasse(Symbole * cla)
{
	SauterEspaces();

	//Opérateurs spéciaux
	if (*Source=='`')
	{
		Source++;
		AjCode(I_ENTIER,PRI_CONSTANTE);
		AjEntier((int)size_t(cla));//TODO : un truc qui veut dire quelquechose
		return true;
	}
	if (*Source=='^' && Source[1]=='^')
	{
		//On veut le numéro de la propriété
		Source+=2;
		ChercherMotClef();
		
		Symbole * prop=ChercherSymboleDans(cla);

		if (!EstNouveauSymbole(prop))
		{
			AjCode(I_ENTIER,PRI_CONSTANTE);
			AjEntier((int)prop->indice);
			return true;
		}
		throw CErreur(TXT("propriété %.*s inconnue après le ^^"),
				lClef,MotClef);
	}

	bool Redef=false;
	bool Constructeur=true;
	bool givre;
	if (Bloc.Dernier()->symbole->attributs & Symbole::filsglob)
		givre=true;
	else
		givre=false;

	SauterEspaces();
	carac * DebutNom=Source;
	ChercherMotClef();
	while (lClef)
	{
		if (EGALMC("sauvage"))
		{
			Redef=false;
			Constructeur=false;
		}
		else if (EGALMC("zap"))
		{
			Redef=true;
			Constructeur=false;
		}
		else if (EGALMC("GIVRE"))
			givre=true;
		else
			break;
		SauterEspaces();
		DebutNom=Source;
		ChercherMotClef();
	}

	Symbole * var=ChercherSymboleDans(Bloc.Dernier()->symbole);

	if (EstNouveauSymbole(var)) //Nouvelle variable
	{
		var=programme->NouveauSymbole(MotClef,lClef,CType(symboleValeur),
			Bloc.Dernier()->symbole);
	}

	if (!Redef)
	{
		if (givre)
		{
			var->indice=programme->varGlob.l;
			programme->varGlob.Etendre(valeur());
			var->type=Symbole::VariableGlobale;
		}
		else
		{
			Symbole * parent=Bloc.Dernier()->symbole;
			var->indice=(index_t)parent->taille++;
			if (parent->attributs & Symbole::filsthis)
				var->type=Symbole::VariableThis;
			else
				var->type=Symbole::VariableLocale;
		}
	}

	var->TypeAc.Def(cla);	
	//Nombre de dimensions
	LireType(var->TypeAc);
	if (!Redef)
		var->typeDepart=var->TypeAc;
/*
	if (var->TypeAc.l==1 && Constructeur)
	{
		if (cla->type==Symbole::ClasseC)
		{
			//On alloue l'objet
			AjCode(I_ALLOCATION);
			AjPtrType(cla,TYPE_TYPE);
			
		}
		else
		{
			//TODO: non implémenté pour les classes GPP, on alloue pas
		}
	}*/

	SauterEspaces();
	
	if (*Source=='=')
	{
		Source=DebutNom;//TODO: utiliser un appel au constructeur par copie
	}
	else if (Constructeur && cla->defConstr)
	{
		if (var->TypeAc.l!=1)
			throw TXT("le berger ne peut pas être exécuté avec les troupeaux anonymes de troupeaux");

		AjCode(givre?I_VARIABLE:I_VARIABLELOC);
		AjPtrType(var,TYPE_VARIABLE);

		AjCode(I_INSTANCEDE,PRI_DEF);

		AjParO(PRI_EXEC);
		InsererConstante(cla->defConstr);
		AjCode(I_EXECVAL,PRI_CONSTANTE);
		AjParF();
		AjParO();
		AjDebutParams();
		if (*Source=='\n')
			AjParF();
		else if (*Source=='(')
		{
			Source++;
		}
	}
	else if (*Source!='\n')
	{
		if (ucarac(*Source)<=32)
			throw CErreur(TXT("caractère 0x%x inattendu après la déclaration"),*Source);
		else
			throw CErreur(TXT("caractère %c inattendu après la déclaration"),*Source);
	}
	return true;
}

GotoPP::BoutCode* Compilateur::Compiler()
{
	Ins.Reinit();
	Ligne=1;
	nValeurs=0;
	nInstr=0;
	iVarLongue=0;
	ProbaSauterLigne=0;
	InstrAc=IP_PARO;
	CibleAuto=0;
	DernierSi=~0;
	NiveauGourou=1;
	etaitPremierSymbole=programme->symbole.FinI();
	index_t EtaitPremierSource=programme->fichierSource.l;
	
	//On inclut l'espace de nommage "rien" qui contient
	//les fonctions internes
	useEspace.Vider();
	useEspace.Etendre(symboleStd);
	//useEspace.Etendre();
	//useEspace.Dernier()=rien;

	//On crée le bloc global
	Bloc.Vider();
	DebutBloc(TB_CLASSE,programme->symboleGlobal);

	/*
		% <= symbole global, peut contenir des chiffres
		%% <= symbole pas global, peut contenir des chiffres
		*/
	while (1)
	{
		SauterEspaces();
		ChercherMotClef();
		InstrPrec=InstrAc;
		InstrAc=IP_VARIABLE;
		const bool AVECET=true;
		const bool SANSET=false;

		if (lClef)
		{
			if (!CompilerMotClef())
			{
				Source=MotClef;
				Symbole * s=ChercherSymbole();
				if (!EstNouveauSymbole(s) && s->EstClasse())
					CompilerClasse(s);
				else
					CompilerVariable(s, SANSET);
			}
		}
		else
		{
			if (!CompilerSeparateur())
			{
				if (*Source=='&')
				{
					Source++;
					Symbole * s=ChercherSymbole();
					if (!EstNouveauSymbole(s) && s->EstClasse())
						throw TXT("nom de troupeau innattendu après le symbole &");
					else
						CompilerVariable(s, AVECET);
				}
				else
					if (!CompilerOperateur() && !CompilerSeparateur() &&
						!CompilerTableau() && !CompilerConstante())
					{
						if (*Source<=' ')
							throw CErreur(TXT("caractère 0x%x inattendu"),*Source);
						else
							throw CErreur(TXT("caractère %c inattendu"),*Source);
					}
			}
			else
				if (*Source==0)
				{
					if (AcFichierSource &&
						AcFichierSource->Parent!=0)
					{
						AcFichierSource->FinCode=(code*)Ins.t;
						AcFichierSource=AcFichierSource->Parent;
						Ligne=AcFichierSource->Ligne;
						Source=AcFichierSource->AcPos;
					}
					else
						break;
				}
		}
	}
	Ins.AjouterCode(I_STOP);
	if (AcSwitch.l)
		throw TXT("un GOTOMULTIPLE n'a pas été fermé");

//	DebutLigne=Ins.c;
	//Bon on corrige tous les labels !
	for (Symbole ** l=etaitPremierSymbole; l<programme->symbole.Fin(); l++)
	{
		if ((*l)->type==Symbole::FonctionGPP ||
			(*l)->type==Symbole::MethodeGPP)
		{
			code * c=&Ins.c[(*l)->Valeur.v.i];
			(*l)->Valeur.v.et=c;
		}
	}

//	qsort(&Label.Premier(),Label.l,sizeof(SLabel),CmpLabels);
	//clog << RefEtiq.l << " références à des étiquettes \n";
	
	for (index_t z=0; z<RefEtiq.l; z++)
	{
		Symbole **k=(Symbole**)&Ins.c[RefEtiq[z]];
		Symbole * v=*k;
		if (v->Valeur.Type!=TYPE_ETIQUETTE && 
			v->Valeur.Type!=TYPE_ETIQUETTE_THIS )
			throw CErreur(TXT("lors de l'édition des liens, l'étiquette %s%%%s n'a pas été définie"),
				v->parent->nom,v->nom);
        *((code**)k)=v->Valeur.v.et;
	}

	for (index_t z=0; z<RefEtiqL.l; z++)
	{
		Symbole **k=(Symbole **)&Ins.c[RefEtiqL[z]];
		Symbole * v=*k;
		if (v->Valeur.Type!=TYPE_ETIQUETTE &&
			v->Valeur.Type!=TYPE_ETIQUETTE_THIS )
			throw CErreur(TXT("lors de l'édition des liens, l'étiquette %s n'a pas été définie"),
				v->nom);
        *((size_t*)k)=size_t((size_t)v->Valeur.v.et-(size_t)k-sizeof(size_t));
	}

	//clog << "Taille du code : "<<Ins.t<<" octets\n";
	if (Bloc.l>1)
		throw TXT("tous les blocs d'instructions n'ont pas été fermés");

	if (RefEtiqA.l)
		throw TXT("toutes les étiquettes automatiques n'ont pas été placées avec AC");

	//On corrige tous les switchs
	for (index_t i=0; i<programme->Switch.l; i++)
	{
		for (index_t c=0; c<programme->Switch[i]->Cas.l; c++)
			programme->Switch[i]->Cas[c].Pos+=(size_t)Ins.c;
		if (programme->Switch[i]->PosDefaut)
			programme->Switch[i]->PosDefaut+=(size_t)Ins.c;
	}

	//On corrige tous les fichiers sources
	for (index_t i=EtaitPremierSource; i<programme->fichierSource.l; i++)
	{
		programme->fichierSource[i]->DebutCode+=(size_t)Ins.c;
		programme->fichierSource[i]->FinCode+=(size_t)Ins.c;
	}

	Ins.i=Ins.c;

	programme->InitVarGlob();
	return Ins.CreerBoutCode();
}

int Compilateur::AcLigne()
{
	return Ligne;
}

carac * Compilateur::AcFichier()
{
	if (AcFichierSource!=0)
		return AcFichierSource->Nom;
	else
		return TXT("<code dynamique>");
}

