#ifndef _GOTOPP_APPELFONCTION_H
#define _GOTOPP_APPELFONCTION_H

//Créé automatiquement par appelfonction.pl

namespace GotoPP
{

class CreateurAppelFonctionC_0
{
	typedef void Fonction();
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		interpreteur->FinParametres();
		fonction();
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M>
class CreateurAppelMethodeC_0
{
	typedef void (M::*Fonction)();
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		interpreteur->FinParametres();
		(((M*)interpreteur->GetThis())->*fonction)();
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

class CreateurAppelFonctionC_0V
{
	typedef void Fonction(int);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		interpreteur->FinParametres();
		fonction(nParams);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M>
class CreateurAppelMethodeC_0V
{
	typedef void (M::*Fonction)(int);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		interpreteur->FinParametres();
		(((M*)interpreteur->GetThis())->*fonction)(nParams);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class R>
class CreateurAppelFonctionC_R0
{
	typedef R Fonction();
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		interpreteur->FinParametres();
		interpreteur->Retourner<R>(fonction());
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class R>
class CreateurAppelMethodeC_R0
{
	typedef R (M::*Fonction)();
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		interpreteur->FinParametres();
		interpreteur->Retourner<R>((((M*)interpreteur->GetThis())->*fonction)());
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class R>
class CreateurAppelFonctionC_R0V
{
	typedef R Fonction(int);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		interpreteur->FinParametres();
		interpreteur->Retourner<R>(fonction(nParams));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class R>
class CreateurAppelMethodeC_R0V
{
	typedef R (M::*Fonction)(int);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		interpreteur->FinParametres();
		interpreteur->Retourner<R>((((M*)interpreteur->GetThis())->*fonction)(nParams));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class A>
class CreateurAppelFonctionC_1
{
	typedef void Fonction(A);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		A par0;
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		fonction(par0);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class A>
class CreateurAppelMethodeC_1
{
	typedef void (M::*Fonction)(A);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		A par0;
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		(((M*)interpreteur->GetThis())->*fonction)(par0);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class A>
class CreateurAppelFonctionC_1V
{
	typedef void Fonction(int,A);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		A par0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		fonction(nParams,par0);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class A>
class CreateurAppelMethodeC_1V
{
	typedef void (M::*Fonction)(int,A);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		A par0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		(((M*)interpreteur->GetThis())->*fonction)(nParams,par0);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class R,class A>
class CreateurAppelFonctionC_R1
{
	typedef R Fonction(A);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		A par0;
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		interpreteur->Retourner<R>(fonction(par0));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class R,class A>
class CreateurAppelMethodeC_R1
{
	typedef R (M::*Fonction)(A);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		A par0;
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		interpreteur->Retourner<R>((((M*)interpreteur->GetThis())->*fonction)(par0));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class R,class A>
class CreateurAppelFonctionC_R1V
{
	typedef R Fonction(int,A);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		A par0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		interpreteur->Retourner<R>(fonction(nParams,par0));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class R,class A>
class CreateurAppelMethodeC_R1V
{
	typedef R (M::*Fonction)(int,A);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		A par0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		interpreteur->Retourner<R>((((M*)interpreteur->GetThis())->*fonction)(nParams,par0));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class A,class B>
class CreateurAppelFonctionC_2
{
	typedef void Fonction(A,B);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		B par1;
		A par0;
		par1=interpreteur->Lire<B>();
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		fonction(par0,par1);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class A,class B>
class CreateurAppelMethodeC_2
{
	typedef void (M::*Fonction)(A,B);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		B par1;
		A par0;
		par1=interpreteur->Lire<B>();
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		(((M*)interpreteur->GetThis())->*fonction)(par0,par1);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class A,class B>
class CreateurAppelFonctionC_2V
{
	typedef void Fonction(int,A,B);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		B par1;
		A par0;
		if (nParams>=1+1)
			par1=interpreteur->Lire<B>();
		else
			par1=0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		fonction(nParams,par0,par1);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class A,class B>
class CreateurAppelMethodeC_2V
{
	typedef void (M::*Fonction)(int,A,B);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		B par1;
		A par0;
		if (nParams>=1+1)
			par1=interpreteur->Lire<B>();
		else
			par1=0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		(((M*)interpreteur->GetThis())->*fonction)(nParams,par0,par1);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class R,class A,class B>
class CreateurAppelFonctionC_R2
{
	typedef R Fonction(A,B);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		B par1;
		A par0;
		par1=interpreteur->Lire<B>();
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		interpreteur->Retourner<R>(fonction(par0,par1));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class R,class A,class B>
class CreateurAppelMethodeC_R2
{
	typedef R (M::*Fonction)(A,B);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		B par1;
		A par0;
		par1=interpreteur->Lire<B>();
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		interpreteur->Retourner<R>((((M*)interpreteur->GetThis())->*fonction)(par0,par1));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class R,class A,class B>
class CreateurAppelFonctionC_R2V
{
	typedef R Fonction(int,A,B);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		B par1;
		A par0;
		if (nParams>=1+1)
			par1=interpreteur->Lire<B>();
		else
			par1=0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		interpreteur->Retourner<R>(fonction(nParams,par0,par1));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class R,class A,class B>
class CreateurAppelMethodeC_R2V
{
	typedef R (M::*Fonction)(int,A,B);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		B par1;
		A par0;
		if (nParams>=1+1)
			par1=interpreteur->Lire<B>();
		else
			par1=0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		interpreteur->Retourner<R>((((M*)interpreteur->GetThis())->*fonction)(nParams,par0,par1));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class A,class B,class C>
class CreateurAppelFonctionC_3
{
	typedef void Fonction(A,B,C);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		C par2;
		B par1;
		A par0;
		par2=interpreteur->Lire<C>();
		par1=interpreteur->Lire<B>();
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		fonction(par0,par1,par2);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class A,class B,class C>
class CreateurAppelMethodeC_3
{
	typedef void (M::*Fonction)(A,B,C);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		C par2;
		B par1;
		A par0;
		par2=interpreteur->Lire<C>();
		par1=interpreteur->Lire<B>();
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		(((M*)interpreteur->GetThis())->*fonction)(par0,par1,par2);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class A,class B,class C>
class CreateurAppelFonctionC_3V
{
	typedef void Fonction(int,A,B,C);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		C par2;
		B par1;
		A par0;
		if (nParams>=2+1)
			par2=interpreteur->Lire<C>();
		else
			par2=0;
		if (nParams>=1+1)
			par1=interpreteur->Lire<B>();
		else
			par1=0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		fonction(nParams,par0,par1,par2);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class A,class B,class C>
class CreateurAppelMethodeC_3V
{
	typedef void (M::*Fonction)(int,A,B,C);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		C par2;
		B par1;
		A par0;
		if (nParams>=2+1)
			par2=interpreteur->Lire<C>();
		else
			par2=0;
		if (nParams>=1+1)
			par1=interpreteur->Lire<B>();
		else
			par1=0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		(((M*)interpreteur->GetThis())->*fonction)(nParams,par0,par1,par2);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class R,class A,class B,class C>
class CreateurAppelFonctionC_R3
{
	typedef R Fonction(A,B,C);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		C par2;
		B par1;
		A par0;
		par2=interpreteur->Lire<C>();
		par1=interpreteur->Lire<B>();
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		interpreteur->Retourner<R>(fonction(par0,par1,par2));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class R,class A,class B,class C>
class CreateurAppelMethodeC_R3
{
	typedef R (M::*Fonction)(A,B,C);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		C par2;
		B par1;
		A par0;
		par2=interpreteur->Lire<C>();
		par1=interpreteur->Lire<B>();
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		interpreteur->Retourner<R>((((M*)interpreteur->GetThis())->*fonction)(par0,par1,par2));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class R,class A,class B,class C>
class CreateurAppelFonctionC_R3V
{
	typedef R Fonction(int,A,B,C);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		C par2;
		B par1;
		A par0;
		if (nParams>=2+1)
			par2=interpreteur->Lire<C>();
		else
			par2=0;
		if (nParams>=1+1)
			par1=interpreteur->Lire<B>();
		else
			par1=0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		interpreteur->Retourner<R>(fonction(nParams,par0,par1,par2));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class R,class A,class B,class C>
class CreateurAppelMethodeC_R3V
{
	typedef R (M::*Fonction)(int,A,B,C);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		C par2;
		B par1;
		A par0;
		if (nParams>=2+1)
			par2=interpreteur->Lire<C>();
		else
			par2=0;
		if (nParams>=1+1)
			par1=interpreteur->Lire<B>();
		else
			par1=0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		interpreteur->Retourner<R>((((M*)interpreteur->GetThis())->*fonction)(nParams,par0,par1,par2));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class A,class B,class C,class D>
class CreateurAppelFonctionC_4
{
	typedef void Fonction(A,B,C,D);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		D par3;
		C par2;
		B par1;
		A par0;
		par3=interpreteur->Lire<D>();
		par2=interpreteur->Lire<C>();
		par1=interpreteur->Lire<B>();
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		fonction(par0,par1,par2,par3);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class A,class B,class C,class D>
class CreateurAppelMethodeC_4
{
	typedef void (M::*Fonction)(A,B,C,D);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		D par3;
		C par2;
		B par1;
		A par0;
		par3=interpreteur->Lire<D>();
		par2=interpreteur->Lire<C>();
		par1=interpreteur->Lire<B>();
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		(((M*)interpreteur->GetThis())->*fonction)(par0,par1,par2,par3);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class A,class B,class C,class D>
class CreateurAppelFonctionC_4V
{
	typedef void Fonction(int,A,B,C,D);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		D par3;
		C par2;
		B par1;
		A par0;
		if (nParams>=3+1)
			par3=interpreteur->Lire<D>();
		else
			par3=0;
		if (nParams>=2+1)
			par2=interpreteur->Lire<C>();
		else
			par2=0;
		if (nParams>=1+1)
			par1=interpreteur->Lire<B>();
		else
			par1=0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		fonction(nParams,par0,par1,par2,par3);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class A,class B,class C,class D>
class CreateurAppelMethodeC_4V
{
	typedef void (M::*Fonction)(int,A,B,C,D);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		D par3;
		C par2;
		B par1;
		A par0;
		if (nParams>=3+1)
			par3=interpreteur->Lire<D>();
		else
			par3=0;
		if (nParams>=2+1)
			par2=interpreteur->Lire<C>();
		else
			par2=0;
		if (nParams>=1+1)
			par1=interpreteur->Lire<B>();
		else
			par1=0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		(((M*)interpreteur->GetThis())->*fonction)(nParams,par0,par1,par2,par3);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class R,class A,class B,class C,class D>
class CreateurAppelFonctionC_R4
{
	typedef R Fonction(A,B,C,D);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		D par3;
		C par2;
		B par1;
		A par0;
		par3=interpreteur->Lire<D>();
		par2=interpreteur->Lire<C>();
		par1=interpreteur->Lire<B>();
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		interpreteur->Retourner<R>(fonction(par0,par1,par2,par3));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class R,class A,class B,class C,class D>
class CreateurAppelMethodeC_R4
{
	typedef R (M::*Fonction)(A,B,C,D);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		D par3;
		C par2;
		B par1;
		A par0;
		par3=interpreteur->Lire<D>();
		par2=interpreteur->Lire<C>();
		par1=interpreteur->Lire<B>();
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		interpreteur->Retourner<R>((((M*)interpreteur->GetThis())->*fonction)(par0,par1,par2,par3));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class R,class A,class B,class C,class D>
class CreateurAppelFonctionC_R4V
{
	typedef R Fonction(int,A,B,C,D);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		D par3;
		C par2;
		B par1;
		A par0;
		if (nParams>=3+1)
			par3=interpreteur->Lire<D>();
		else
			par3=0;
		if (nParams>=2+1)
			par2=interpreteur->Lire<C>();
		else
			par2=0;
		if (nParams>=1+1)
			par1=interpreteur->Lire<B>();
		else
			par1=0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		interpreteur->Retourner<R>(fonction(nParams,par0,par1,par2,par3));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class R,class A,class B,class C,class D>
class CreateurAppelMethodeC_R4V
{
	typedef R (M::*Fonction)(int,A,B,C,D);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		D par3;
		C par2;
		B par1;
		A par0;
		if (nParams>=3+1)
			par3=interpreteur->Lire<D>();
		else
			par3=0;
		if (nParams>=2+1)
			par2=interpreteur->Lire<C>();
		else
			par2=0;
		if (nParams>=1+1)
			par1=interpreteur->Lire<B>();
		else
			par1=0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		interpreteur->Retourner<R>((((M*)interpreteur->GetThis())->*fonction)(nParams,par0,par1,par2,par3));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class A,class B,class C,class D,class E>
class CreateurAppelFonctionC_5
{
	typedef void Fonction(A,B,C,D,E);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		E par4;
		D par3;
		C par2;
		B par1;
		A par0;
		par4=interpreteur->Lire<E>();
		par3=interpreteur->Lire<D>();
		par2=interpreteur->Lire<C>();
		par1=interpreteur->Lire<B>();
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		fonction(par0,par1,par2,par3,par4);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class A,class B,class C,class D,class E>
class CreateurAppelMethodeC_5
{
	typedef void (M::*Fonction)(A,B,C,D,E);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		E par4;
		D par3;
		C par2;
		B par1;
		A par0;
		par4=interpreteur->Lire<E>();
		par3=interpreteur->Lire<D>();
		par2=interpreteur->Lire<C>();
		par1=interpreteur->Lire<B>();
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		(((M*)interpreteur->GetThis())->*fonction)(par0,par1,par2,par3,par4);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class A,class B,class C,class D,class E>
class CreateurAppelFonctionC_5V
{
	typedef void Fonction(int,A,B,C,D,E);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		E par4;
		D par3;
		C par2;
		B par1;
		A par0;
		if (nParams>=4+1)
			par4=interpreteur->Lire<E>();
		else
			par4=0;
		if (nParams>=3+1)
			par3=interpreteur->Lire<D>();
		else
			par3=0;
		if (nParams>=2+1)
			par2=interpreteur->Lire<C>();
		else
			par2=0;
		if (nParams>=1+1)
			par1=interpreteur->Lire<B>();
		else
			par1=0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		fonction(nParams,par0,par1,par2,par3,par4);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class A,class B,class C,class D,class E>
class CreateurAppelMethodeC_5V
{
	typedef void (M::*Fonction)(int,A,B,C,D,E);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		E par4;
		D par3;
		C par2;
		B par1;
		A par0;
		if (nParams>=4+1)
			par4=interpreteur->Lire<E>();
		else
			par4=0;
		if (nParams>=3+1)
			par3=interpreteur->Lire<D>();
		else
			par3=0;
		if (nParams>=2+1)
			par2=interpreteur->Lire<C>();
		else
			par2=0;
		if (nParams>=1+1)
			par1=interpreteur->Lire<B>();
		else
			par1=0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		(((M*)interpreteur->GetThis())->*fonction)(nParams,par0,par1,par2,par3,par4);
		interpreteur->RetournerIndefini();
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class R,class A,class B,class C,class D,class E>
class CreateurAppelFonctionC_R5
{
	typedef R Fonction(A,B,C,D,E);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		E par4;
		D par3;
		C par2;
		B par1;
		A par0;
		par4=interpreteur->Lire<E>();
		par3=interpreteur->Lire<D>();
		par2=interpreteur->Lire<C>();
		par1=interpreteur->Lire<B>();
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		interpreteur->Retourner<R>(fonction(par0,par1,par2,par3,par4));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class R,class A,class B,class C,class D,class E>
class CreateurAppelMethodeC_R5
{
	typedef R (M::*Fonction)(A,B,C,D,E);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		E par4;
		D par3;
		C par2;
		B par1;
		A par0;
		par4=interpreteur->Lire<E>();
		par3=interpreteur->Lire<D>();
		par2=interpreteur->Lire<C>();
		par1=interpreteur->Lire<B>();
		par0=interpreteur->Lire<A>();
		interpreteur->FinParametres();
		interpreteur->Retourner<R>((((M*)interpreteur->GetThis())->*fonction)(par0,par1,par2,par3,par4));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

template<class R,class A,class B,class C,class D,class E>
class CreateurAppelFonctionC_R5V
{
	typedef R Fonction(int,A,B,C,D,E);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		E par4;
		D par3;
		C par2;
		B par1;
		A par0;
		if (nParams>=4+1)
			par4=interpreteur->Lire<E>();
		else
			par4=0;
		if (nParams>=3+1)
			par3=interpreteur->Lire<D>();
		else
			par3=0;
		if (nParams>=2+1)
			par2=interpreteur->Lire<C>();
		else
			par2=0;
		if (nParams>=1+1)
			par1=interpreteur->Lire<B>();
		else
			par1=0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		interpreteur->Retourner<R>(fonction(nParams,par0,par1,par2,par3,par4));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleFonctionC(&AppelFonction<fonction>,nom);
	}
};

template<class M,class R,class A,class B,class C,class D,class E>
class CreateurAppelMethodeC_R5V
{
	typedef R (M::*Fonction)(int,A,B,C,D,E);
public:
	template<Fonction fonction>
	static void AppelFonction(IInterpreteur * interpreteur)
	{
		int nParams=interpreteur->GetNombreParametres();
		E par4;
		D par3;
		C par2;
		B par1;
		A par0;
		if (nParams>=4+1)
			par4=interpreteur->Lire<E>();
		else
			par4=0;
		if (nParams>=3+1)
			par3=interpreteur->Lire<D>();
		else
			par3=0;
		if (nParams>=2+1)
			par2=interpreteur->Lire<C>();
		else
			par2=0;
		if (nParams>=1+1)
			par1=interpreteur->Lire<B>();
		else
			par1=0;
		if (nParams>=0+1)
			par0=interpreteur->Lire<A>();
		else
			par0=0;
		interpreteur->FinParametres();
		interpreteur->Retourner<R>((((M*)interpreteur->GetThis())->*fonction)(nParams,par0,par1,par2,par3,par4));
	}

	template<Fonction fonction>
	static ISymbole * AjouterFonction(ISymbole * symboleParent, const carac * nom)
	{
		return symboleParent->NouvelleMethodeC(&AppelFonction<fonction>,nom);
	}
};

}

#endif
