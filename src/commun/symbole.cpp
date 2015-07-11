#include <gotopp/base.h>
#include <gotopp/programme.h>
#include <gotopp/global.h>

namespace GotoPP
{
	Symbole::Symbole(const carac * _nom, size_t lNom, Symbole * _parent, Type _type):parent(_parent),type(_type),
		attributs(0)
	{
		if (lNom>TMAXIDENTIFIEUR)
			lNom=TMAXIDENTIFIEUR;
		memcpy(nom,_nom,lNom*sizeof(carac));
		nom[lNom]=0;
		if (parent)
		{
			programme=parent->programme;
			suivant=parent->fils;
			parent->fils=this;
		}
		else
		{
			programme=0;
			suivant=0;
		}
		fils=0;
		if (EstClasse())
			attributs=Symbole::filsthis;
		if (programme)
			programme->symbole.Etendre(this);
	}

	ISymbole * Symbole::NouvelleMethodeC(voidf Fonc, const carac * nom)
	{
		CType T(symboleValeur);
		T.Empiler(symboleDebutParams);
		T.Empiler(symboleValeur);
		T.Empiler(symboleEtc);
		T.Empiler(symboleMethodeC);

		Symbole * symb=new Symbole(nom,this,Symbole::MethodeC);
		symb->typeDepart=symb->TypeAc=T;
		symb->attributs|=Symbole::constante;
		symb->Valeur.Type=TYPE_FONCTIONC_THIS;
		symb->Valeur.v.fonctionC=Fonc;
		return symb;
	}

	ISymbole * Symbole::NouvelleFonctionC(voidf fonc, const carac * nom)
	{
		CType T(symboleValeur);
		T.Empiler(symboleDebutParams);
		T.Empiler(symboleValeur);
		T.Empiler(symboleEtc);
		T.Empiler(symboleFonctionC);

		Symbole * symb=new Symbole(nom,this,Symbole::FonctionC);
		symb->typeDepart=symb->TypeAc=T;
		symb->attributs|=Symbole::constante;
		symb->Valeur.v.fonctionC=fonc;
		symb->Valeur.Type=TYPE_FONCTIONC;
		return symb;
	}
}
