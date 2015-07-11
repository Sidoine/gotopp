#include <gotopp/base.h>
#include <gotopp/compilateur.h>
#include <gotopp/programme.h>
#include <gotopp/mutex.h>
#include <gotopp/erreur.h>
#include <gotopp/global.h>

using namespace GotoPP;

Symbole * Programme::NouvelleVariable(const carac *Nom, size_t lNom,
								   CType type, Symbole * parent)
{
	if (!parent)
		parent=symboleGlobal;
	Symbole * symb=new Symbole(Nom,lNom,parent,Symbole::VariableGlobale);
	symb->typeDepart=type;
	symb->TypeAc=type;
	symb->indice=varGlob.l;
	varGlob.Etendre(valeur());
	return symb;
}

Symbole * Programme::NouvelleVariable(const carac *Nom, CType type, Symbole * parent)
{
	return NouvelleVariable(Nom,czlen(Nom),type,parent);
}

Symbole * Programme::NouvelleVariable(const carac *Nom, Symbole * cl, Symbole * parent)
{
	return NouvelleVariable(Nom,CType(cl),parent);
}

ISymbole * Programme::NouvelleFonctionC(voidf f,const carac *Nom, ISymbole * parent)
{
	CType T(symboleValeur);
	T.Empiler(symboleDebutParams);
	T.Empiler(symboleValeur);
	T.Empiler(symboleEtc);
	T.Empiler(symboleFonctionC);

	Symbole * symb=NouveauSymbole(Nom,czlen(Nom),T,(Symbole*)parent);
	symb->type=Symbole::FonctionC;
	symb->attributs|=Symbole::constante;
	symb->Valeur.v.fonctionC=f;
	symb->Valeur.Type=TYPE_FONCTIONC;
	/*varGlob.Dernier().Type=TYPE_FONCTIONC;
	varGlob.Dernier().v.fonctionC=f;*/
	return symb;
}

ISymbole * Programme::NouvelleFonctionC(voidf f,const carac *Nom,carac*Type,ISymbole * parent)
{
	CType T;
	carac * t=Type;//Ouais, j'abuse pour les noms de variable
	while (*t)
	{
		switch(*t)
		{
		case 'f':
			T.Empiler(symboleFlottant);
			break;
		case 'e':
			T.Empiler(symboleEntier);
			break;
		case 'c':
			T.Empiler(symboleCarac);
			break;
		case 's':
			T.Empiler(symboleChaine);
			break;
		case 'v':
			T.Empiler(symboleValeur);
			break;
		case ':':
			T.Empiler(symboleDebutParams);
			break;
		default:
			throw TXT("bouh !");
		}
		t++;
	}
	T.Empiler(symboleFonctionC);

	Symbole * symb=NouvelleVariable(Nom,T,(Symbole*)parent);
	symb->type=Symbole::FonctionC;
	varGlob.Dernier().Type=TYPE_FONCTIONC;
	varGlob.Dernier().v.fonctionC=f;
	return symb;
}

Symbole * Programme::NouveauSymbole(const carac *Nom, size_t lNom, CType type, Symbole * parent)
{
	if (!parent)
		parent=symboleGlobal;
	Symbole * symb=new Symbole(Nom,lNom,parent);
	symb->typeDepart=type;
	symb->TypeAc=type;
	return symb;
}

Symbole * Programme::ChercherClasse(const carac *Nom, Symbole * parent)
{
	for (Symbole * a=parent->fils;a!=0;a=a->suivant)
	{
		if (czcmp(a->nom,Nom)==0)
			return a;
	}
	throw CErreur(TXT("classe %s introuvable"),Nom);
}

ISymbole * Programme::NouvelleClasseC(const carac *Nom, size_t lNom, voidf constructeur, ISymbole * parent)
{
	Symbole * symb=NouveauSymbole(Nom,lNom,static_cast<Symbole*>(parent));
	symb->type=Symbole::ClasseC;
	symb->attributs|=Symbole::filsthis;
	symb->defConstr=(Symbole*)NouvelleFonctionC(constructeur,_T("berger"),symb);
	return symb;
}

ISymbole * Programme::NouvelleMethodeC(voidf Fonc, const carac *Nom, ISymbole * parent)	
{
/*
	c->Methode.Etendre();
	czcpy(c->Methode.Dernier().Nom,Nom);
	c->Methode.Dernier().Val=(int)FonctionInt.l;
	c->Methode.Dernier().Type.Def(TYPE_PINGOUIN);
	c->Methode.Dernier().Type.Empiler(TYPE_DEBUTPARAM);
	c->Methode.Dernier().Type.Empiler(TYPE_PINGOUIN);
	c->Methode.Dernier().Type.Empiler(TYPE_ETC);
	c->Methode.Dernier().Type.Empiler(TYPE_FONCTIONC);
	RajouterFonction(Fonc);
//	c->Methode.Dernier().Type=TYPEFONC_C;
	c->Methode.Dernier().Prive=PROP_PUBLIQUE;
	c->Methode.Dernier().Givree=Givree;	*/

	CType T(symboleValeur);
	T.Empiler(symboleDebutParams);
	T.Empiler(symboleValeur);
	T.Empiler(symboleEtc);
	T.Empiler(symboleMethodeC);

	Symbole * symb=NouveauSymbole(Nom,czlen(Nom),T,(Symbole*)parent);
	symb->Valeur.Type=TYPE_FONCTIONC_THIS;
	symb->Valeur.v.fonctionC=Fonc;
	symb->attributs|=Symbole::constante;
	symb->type=Symbole::MethodeC;
	return symb;
}
	
Symbole * Programme::NouvelleConstante(const carac * Nom, size_t lNom, Symbole * parent)
{
	Symbole * symb=NouveauSymbole(Nom,lNom,CType(symboleEntier),parent);
	symb->attributs=Symbole::constante;
	return symb;
}

Symbole * Programme::NouvelleConstante(const carac *Nom, size_t lNom,
									int val, Symbole * parent)
{
	Symbole * symb=NouvelleConstante(Nom,lNom,parent);
	symb->Valeur=val;
	return symb;
}

carac * Programme::NomVarGlob(index_t i)
{
	for (index_t s=0; s<symbole.l; s++)
	{
		if ((symbole[s]->type==Symbole::VariableGlobale)
			&& (symbole[s]->indice==i))
				return symbole[s]->nom;
	}
	return _T("<symbole non défini>");
}

carac * Programme::NomEtiquette(code *c)
{
	for (index_t s=0; s<symbole.l; s++)
	{
		if ((symbole[s]->type==Symbole::FonctionGPP ||symbole[s]->type==Symbole::MethodeGPP )
			&& (symbole[s]->Valeur.v.et==c))
				return symbole[s]->nom;
	}
	return _T("<étiquette non définie>");
}

void Programme::DefClasses()
{
	symboleGlobal=NouveauSymbole(_T(""),0,CType(symboleValeur),0);
	symboleGlobal->type=Symbole::Classe;
	symboleGlobal->attributs=Symbole::filsglob;
	symboleGlobal->programme=this;
}


Programme::Programme()
{
	DefClasses();
	compilateur=ICompilateur::Factory(this);
}

void Programme::InitVarGlob()
{
	for (index_t v=0; v<symbole.l; v++)
	{
		if (symbole[v]->type!=Symbole::VariableGlobale)
			continue;

		index_t vg=symbole[v]->indice;

		if (varGlob[vg].Type!=TYPE_INCONNU)
			continue;
		
		if (symbole[v]->typeDepart.l==1)
		{
			if (symbole[v]->typeDepart.t[0]->IsPod())
			{
				if (symbole[v]->typeDepart.t[0]==symboleEntier)
					varGlob[vg]=rand()%100;
				else
					varGlob[vg]=float(rand())/(RAND_MAX+1.0f)*100;
			}
			else
			{
				//On crée un troupeau de la bonne taille
				varGlob[vg].Type=TYPE_TABLEAU;
				varGlob[vg].v.t=new tableauval();
				varGlob[vg].v.t->Classe=symbole[v]->typeDepart.t[0];
				varGlob[vg].v.t->Dim((index_t)symbole[v]->typeDepart.t[0]->taille);
			}

		}
		/*
		else
		{
			Un truc plus complexe, on n'initialise pas la variable	
		}
		*/
	}
}

code * Programme::ChercherEtiquette(int i)
{
	for (index_t s=0; s<symbole.l; s++)
		if ((symbole[s]->type==Symbole::FonctionGPP || symbole[s]->type==Symbole::MethodeGPP)
			&& symbole[s]->etiquette==i)
			return symbole[s]->Valeur.v.et;
	throw CErreur(TXT("étiquette %d introuvable"),i);
}

float Programme::GetVersion()
{
	return 0.30f;
}
