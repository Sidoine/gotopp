/*

En-tête de développement des modules du GOTO++.
Copyright 2002 Sidoine de Wispelaere
	
La modification de ce fichier est totalement interdite. Et je
ne parle même pas de sa distribution. D'ailleurs, j'engagerai des
tueurs pour vous poursuivre si vous le faites. Et n'espérez pas
vous cacher dans un pays d'amérique latine, j'ai des moyens
collossaux.

Non, je blague... Ahaha. Voilà le vrai texte bien légal :
	
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

Bon après cet instant de détente, passons aux choses sérieuses. Pour
faire un module vous devez inclure ce fichier d'en-tête (à l'exclusion
de tout autre en-tête du GOTO++) ET vous devez lier le programme avec
rienmodule.cpp. Vous aurez été prévenus, ne vous plaignez pas si ça
marche pas.
*/


#ifndef _GPPMODULE_H
#define _GPPMODULE_H

#include <new>

namespace GotoPP
{
	class IInterpreteur;
#ifdef _UNICODE
	const bool Unicode=true;
#else
	const bool Unicode=false;
#endif

#ifdef _T
	#undef _T
#endif

#ifdef _UNICODE
	typedef wchar_t carac;
	#define _T(x) L##x
#else
	typedef char carac;
	#define _T(x) x
#endif
	typedef void FonctionC(IInterpreteur * interpreteur);
	typedef FonctionC * voidf;
	typedef unsigned int index_t;
#ifdef _WIN32
	typedef __int64 entier64;
#else
	typedef long long entier64;
#endif
	typedef unsigned char code;
}

#include "version.h"

#ifdef _WIN32
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT extern "C" 
#endif

namespace GotoPP
{

	const float VersionMin=0.30f;

	class IChaineA
	{
	public:
		size_t l;
		char c[1];
	};
	
	class IChaine
	{
	public:
		size_t l;
		carac c[1];
	};

	class chaineval;

	class IObjet
	{
	public:
		virtual IObjet * Copier() { return this; };
		virtual ~IObjet(){}
	};

	class IValeur:public IObjet
	{
	public:
		virtual float Flottant() const =0;
	};

	class ITableauValeurs:public IObjet
	{
	public:
		virtual const IValeur * GetValeur(index_t index)=0;
		virtual index_t Taille()=0;
	};

	class regexp;

	class IGotoPP
	{
	public:
		virtual void * Allouer(size_t taille)=0;
		virtual void * Reallouer(void * data, size_t taille)=0;
#ifdef _UNICODE
		virtual IChaine * AllouerChaine(const char * texte)=0;
		virtual IChaine * AllouerChaine(const char * texte,size_t longueur)=0;
#endif
		virtual IChaine * AllouerChaine(const carac * texte)=0;
		virtual IChaine * AllouerChaine(const carac * texte,size_t longueur)=0;
		virtual bool TesterVersion(float version=Version, bool unicode=Unicode)=0;
	};

	extern IGotoPP * iGotoPP;

	class IInterpreteur
	{
	public:
		template<class T>
		T Lire(){return (T)LireObjet();}
				
		template<class T>
		void Retourner(T param){RetournerObjet(param);}
		
		virtual IObjet * GetThis()=0;
		virtual const IChaine * LireChaine()=0;
		virtual int LireEntier()=0;
		virtual float LireFlottant()=0;
		virtual void * LirePointeur()=0;
		virtual void * LireVariable(int v)=0;
		virtual GotoPP::IChaineA * LireChaineA()=0;
		virtual void FinParametres()=0;
		virtual IObjet * LireObjet()=0;
		virtual ITableauValeurs * LireTableauValeurs()=0;
		virtual regexp * LireRegExp()=0;
		virtual IValeur * LireValeur()=0;
		virtual entier64 LireEntier64()=0;
		virtual int GetNombreParametres()=0;
		
		virtual void RetournerChaine(carac * c)=0;
		virtual void RetournerChaineA(char * c)=0;
		virtual void RetournerEntier(int i)=0;
		virtual void RetournerFlottant(float f)=0;
		virtual void RetournerIndefini()=0;
		virtual void RetournerChaine(const IChaine * chaine)=0;
		virtual void RetournerCarac(carac c)=0;
		virtual void RetournerTableauValeurs(ITableauValeurs * tableau)=0;
		virtual void RetournerEntier64(entier64 i)=0;
		virtual void RetournerObjet(IObjet * o)=0;
		
		virtual void Modifier(void * p, IObjet * o)=0;
		virtual void Modifier(void * p, int i)=0;
		virtual void Modifier(void * p, carac * c)=0;
		virtual void Modifier(void * p, carac * c, int l)=0;
#ifdef _UNICODE
		virtual void Modifier(void * p, char * c, int l)=0;
#endif

//		virtual void Ajouter(void *p, const IChaine * clef, const IChaine * val, size_t lval)=0;
		
		virtual void Exception(carac *Texte)=0;
		virtual void DebutParametres()=0;
		virtual void AppelerFonction(code* f)=0;
	};

	template<>
	inline int IInterpreteur::Lire<int>(){return LireEntier();}
	template<>
	inline float IInterpreteur::Lire<float>(){return LireFlottant();}
	template<>
	inline const IChaine * IInterpreteur::Lire<const IChaine *>(){return LireChaine();}
	template<>
	inline const chaineval * IInterpreteur::Lire<const chaineval *>(){return (const chaineval *)LireChaine();}
	template<>
	inline IObjet * IInterpreteur::Lire<IObjet *>(){return LireObjet();}
	template<>
	inline ITableauValeurs * IInterpreteur::Lire<ITableauValeurs*>(){return LireTableauValeurs();}
	template<>
	inline regexp * IInterpreteur::Lire<regexp*>(){return LireRegExp();}
	template<>
	inline IValeur* IInterpreteur::Lire<IValeur*>(){return LireValeur();}
	template<>
	inline entier64 IInterpreteur::Lire<entier64>(){return LireEntier64();}
	template<>
	inline const carac * IInterpreteur::Lire<const carac*>(){return LireChaine()->c;}
	template<>
	inline const IChaineA *IInterpreteur::Lire<const IChaineA*>(){return LireChaineA();}

	template<>
	inline void IInterpreteur::Retourner<int>(int param){RetournerEntier(param);}
	template<>
	inline void IInterpreteur::Retourner<float>(float param){RetournerFlottant(param);}
	template<>
	inline void IInterpreteur::Retourner<const IChaine *>(const IChaine* param){RetournerChaine(param);}
	template<>
	inline void IInterpreteur::Retourner<const chaineval *>(const chaineval* param){RetournerChaine((const IChaine*)param);}
	template<>
	inline void IInterpreteur::Retourner<carac>(carac param){RetournerCarac(param);}
	template<>
	inline void IInterpreteur::Retourner<ITableauValeurs*>(ITableauValeurs* param){RetournerTableauValeurs(param);}
	template<>
	inline void IInterpreteur::Retourner<entier64>(entier64 param){RetournerEntier64(param);}
	template<>
	inline void IInterpreteur::Retourner<IObjet *>(IObjet * param){RetournerObjet(param);}

	class ISymbole: public IObjet
	{
	public:
		virtual ISymbole * NouvelleMethodeC(voidf Fonc, const carac * nom)=0;
		virtual ISymbole * NouvelleFonctionC(voidf Fonc, const carac * nom)=0;
	};

	class IProgramme
	{
	public:
		virtual float GetVersion()=0;
		virtual ISymbole * NouvelleClasseC(const carac *Nom, voidf allocateur, ISymbole * parent=0)=0;
		virtual ISymbole * NouvelleMethodeC(voidf Fonc, const carac *Nom, ISymbole * parent=0)=0;
		/*virtual void NouvellePropriete(void *c, carac *Nom, int Prive, 
			voidf FonctionLire, voidf FonctionEcrire)=0;*/
		virtual ISymbole * NouvelleFonctionC(voidf Fonc, const carac *Nom, ISymbole * parent=0)=0;
		//virtual int NouvelleVariable(const carac *Nom)=0;
	};

	template<class T>
	class CreateurClasseC
	{
	public:
		static void Allocateur(IInterpreteur * itp)
		{
			itp->FinParametres();
			T * ret=(T*)iGotoPP->Allouer(sizeof(T));
			new (ret) T();
			itp->RetournerObjet(ret);
		}
		static ISymbole * Creer(IProgramme * programme, ISymbole * parent, const carac *nom)
		{
			ISymbole * classe=programme->NouvelleClasseC(nom,Allocateur,parent);
			return classe;
		}
	};
}

#include "appelfonction.h"

#endif
