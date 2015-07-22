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
					 
Des bidules que j'ai eu la flemme de classer. Faut dire que c'est
l'horreur ce programme, avec ces deux classes tellement énorme
qu'on peut pas décemment tout faire rentrer dans deux fichiers.
Il y a essentiellement des machins pour sauver et charger des
programmes.

*/

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <unistd.h>
#endif

#include <gotopp/base.h>
#include <gotopp/icompilateur.h>
#include <gotopp/erreur.h>
#include <gotopp/instruction.h>
#include <gotopp/programme.h>
#include <gotopp/gppmodule.h>

#include "../compilateur/compriv.h"
#include "sauveur.h"

using namespace GotoPP;
using namespace std;

typedef  bool (*FonctionInitialiser)(IGotoPP*);
typedef void (*FonctionEnregistrerSymboles)(IProgramme *, ISymbole *);
typedef  void (*FonctionFermer)();

bool Programme::ChargerModule(carac * Fichier, uint l)
{
	//TODO: ne charger la DLL qu'une fois
#ifdef _WIN32
	carac NomFichier[512];
	memcpy(NomFichier,Fichier,l*sizeof(carac));
	czcpy(&NomFichier[l],_T(".dll"));
	void * dll=LoadLibrary(NomFichier);
	if (!dll)
	{
		cznprintf(NomFichier,511,_T("%s/lib/%.*s.dll"),
			CheminModules,l,Fichier);
		dll=LoadLibrary(NomFichier);
	}
#else
	char NomFichier[512];
	char currentwd[512];
#ifdef _UNICODE
	char *fichier=new char[l+1];
	cztostr(fichier,Fichier,l);
#else
	const char *fichier=Fichier;
#endif
	getcwd(currentwd,511);

	sprintf(NomFichier,"%s/"DLL_PREFIX"%.*s"DLL_SUFFIX,
		currentwd,l,fichier);
	//czout<<NomFichier<<endl;
	void * dll=dlopen(NomFichier,RTLD_NOW);
	if (!dll)
	{
		sprintf(NomFichier,"%s/lib/"DLL_PREFIX"%.*s"DLL_SUFFIX,CheminModules,l,fichier);
		//czout<<NomFichier<<endl;
		dll=dlopen(NomFichier,RTLD_NOW);

		if (!dll)
		{
			sprintf(NomFichier,"%s/lib/gpp/"DLL_PREFIX"%.*s"DLL_SUFFIX,CheminModules,l,fichier);
			//czout<<NomFichier<<endl;
			dll=dlopen(NomFichier,RTLD_NOW);
		}
		if (!dll)
		{
			sprintf(NomFichier,"/usr/lib/gpp/"DLL_PREFIX"%.*s"DLL_SUFFIX,l,fichier);
			//czout<<NomFichier<<endl;
			dll=dlopen(NomFichier,RTLD_NOW);
		}
		if (!dll)
		{
			sprintf(NomFichier,"/usr/local/lib/gpp/"DLL_PREFIX"%.*s"DLL_SUFFIX,l,fichier);
			//czout<<NomFichier<<endl;
			dll=dlopen(NomFichier,RTLD_NOW);
		}
	}
#ifdef _UNICODE
	delete[]fichier;
#endif
#endif
	if (!dll)		
	{
#ifndef _WIN32
	#ifdef _UNICODE
		throw CErreur(L"%S",dlerror());
	#else
		throw CErreur("%s (%.*s)",dlerror(),l,fichier);
	#endif
#else
		throw TXT("module introuvable");
#endif
	}
	FonctionInitialiser Initialiser;
	FonctionEnregistrerSymboles EnregistrerSymboles;
#ifdef _WIN32
	Initialiser=(FonctionInitialiser)GetProcAddress(HMODULE(dll),"Initialiser");
	EnregistrerSymboles=(FonctionEnregistrerSymboles)GetProcAddress(HMODULE(dll),"EnregistrerSymboles");
#else
	Initialiser=(FonctionInitialiser)dlsym(dll,"Initialiser");
	EnregistrerSymboles=(FonctionEnregistrerSymboles)dlsym(dll,"EnregistrerSymboles");
#endif
	if (Initialiser==0)
		throw TXT("symbole Initialiser introuvable dans le module");
	if (EnregistrerSymboles==0)
		throw TXT("symbole EnregistrerSymboles introuvable dans le module");
	if (!Initialiser(iGotoPP))
		throw TXT("module incompatible");
	EnregistrerSymboles(this,symboleGlobal);
	CModule * module=new CModule();
	module->Handle=dll;
	Module.Etendre(module);
	return true;
}

CModule::~CModule()
{
	FonctionFermer Fermer;
#ifdef _WIN32
	Fermer=(FonctionFermer)GetProcAddress(HMODULE(Handle),"Fermer");
	if (Fermer)
		Fermer();
	FreeLibrary(HMODULE(Handle));
#else
	Fermer=(FonctionFermer)dlsym(Handle,"Fermer");
	if (Fermer)
		Fermer();
	dlclose(Handle);
#endif
}

int GotoPP::CmpLabels(const void* _a, const void* _b)
{
	const SLabel *a=(const SLabel*)_a;
	const SLabel *b=(const SLabel*)_b;
	if (a->Numero>b->Numero)
		return 1;
	else if (a->Numero<b->Numero)
		return -1;
	else
		return 0;
}

void Programme::SauverBinaire(const carac * Fichier, bool Etat)
{
/*	FILE * F=czfopen(Fichier,_T("wb"));
	fwrite("RIFF",4,1,F);
	size_t TailleTot=0;
	fwrite(&TailleTot,4,1,F);
	fwrite("gpp ",4,1,F);
	
	//Enregistrement des variables globales
	fwrite("vars",4,1,F);
	size_t TailleVar=0;
	size_t Debut=ftell(F);
	fwrite(&TailleVar,4,1,F);
	fwrite(&varGlob.l,sizeof(varGlob.l),1,F);
	for (index_t v=0; v<varGlob.l; v++)
	{
/*		BYTE ln=(BYTE)czlen(Variable[v].Nom);
		fwrite(&ln,1,1,F);
		fwrite(Variable[v].Nom,ln,1,F);*/
		//varGlob[v].Sauver(F);
		/*size_t lt=ln+8;
		if (Variable[v].Valeur.Type==TYPE_FONCTION)
			lt+=9+12;
		fwrite("vari",4,1,F);
		fwrite(&lt,4,1,F);
		fwrite("nom ",4,1,F);
		fwrite(&ln,4,1,F);
		fwrite(Variable[v].Nom,ln,1,F);
		if (Variable[v].Valeur.Type==TYPE_FONCTION)
		{
			size_t l;
			fwrite("type",4,1,F);
			l=1;
			fwrite(&l,4,1,F);
			fwrite(&Variable[v].Valeur.Type,1,1,F);
			fwrite("vale",4,1,F);
			l=4;
			fwrite(&l,4,1,F);
			fwrite(&Variable[v].Valeur.v.i,4,1,F);
		}
		TailleVar+=lt+8;*/
/*	}
	TailleVar=ftell(F)-Debut-4;
	fseek(F,long(Debut),SEEK_SET);
	fwrite(&TailleVar,4,1,F);
	fseek(F,0,SEEK_END);
	TailleTot+=TailleVar+8;

	//Enregistrement des noms de fichier
	fwrite("fsrs",4,1,F);
	size_t TailleFSrc=0;
	Debut=ftell(F);
	fwrite(&TailleFSrc,4,1,F);
	for (index_t f=0; f<FichierSource.l; f++)
	{
		size_t ln=czlen(FichierSource[f].Nom);
		size_t lt=ln*sizeof(carac)+8;
		fwrite("fsrc",4,1,F);
		fwrite(&lt,4,1,F);
		fwrite("nom ",4,1,F);
		fwrite(&ln,4,1,F);
		fwrite(FichierSource[f].Nom,ln*sizeof(carac),1,F);
		TailleFSrc+=lt+8;
	}
	fseek(F,long(Debut),SEEK_SET);
	fwrite(&TailleFSrc,4,1,F);
	fseek(F,0,SEEK_END);
	TailleTot+=TailleFSrc+8;

	//Enregistrement des étiquettes
	fwrite("etiq",4,1,F);
	size_t TailleEtiq=8*Label.l;
	fwrite(&TailleEtiq,4,1,F);
	for (index_t e=0; e<Label.l; e++)
	{
		fwrite(&Label[e].Numero,4,1,F);
		fwrite(&Label[e].Valeur,4,1,F);
	}
	TailleTot+=TailleEtiq+8;

	//Enregistrement du code compilé
	fwrite("code",4,1,F);
	fwrite(&Ins.t,4,1,F);
	fwrite(Ins.c,Ins.t,1,F);
	TailleTot+=Ins.t+8;

	//Enregistrement des switches
	if (Switch.l)
	{
		fwrite("swis",4,1,F);
		int TailleSw=0;
		fwrite(&TailleSw,4,1,F);
		for (index_t s=0; s<Switch.l; s++)
		{
			fwrite("swit",4,1,F);
			int T=(int)Switch[s].Cas.l*2*sizeof(int)+sizeof(int)+1;
			fwrite(&T,4,1,F);
			byte Type=0;
			fwrite(&Type,1,1,F);
			fwrite(&Switch[s].PosDefaut,sizeof(int),1,F);
			for (index_t c=0; c<Switch[s].Cas.l; c++)
			{
				fwrite(&Switch[s].Cas[c].Valeur,sizeof(int),1,F);
				fwrite(&Switch[s].Cas[c].Pos,sizeof(int),1,F);
			}
			TailleSw+=8+T;
		}
		fseek(F,-4-TailleSw,SEEK_CUR);
		fwrite(&TailleSw,4,1,F);
		fseek(F,0,SEEK_END);
	}
	fseek(F,4,SEEK_SET);
	fwrite(&TailleTot,4,1,F);

	fclose(F);*/
}

FILE * OuvrirFichier(const carac * _Nom, const carac * CheminModules)
{
	carac Nom[512];
	czcpy(Nom,_Nom);
	carac * Extension=NULL;
	carac * e=Nom;
	while (*e)
	{
		if (*e=='.')
			Extension=e;
		e++;
	}
	if (!Extension)
		czcpy(e,_T(".gpp"));

	FILE * F=czfopen(Nom,_T("rb"));
	if (!F && !Extension)
	{
		czcpy(e,_T(".gpt"));
		F=czfopen(Nom,_T("rb"));
	}
	//On essaye dans lib
	if (!F && *CheminModules)
	{
		if (!Extension)
			czcpy(e,_T(".gpp"));
		carac NomBis[512];
		cznprintf(NomBis,511,_T("%s/lib/%s"),CheminModules,Nom);
		F=czfopen(NomBis,_T("rb"));
		if (!F && !Extension)
		{
			czcpy(e,_T(".gpt"));
			cznprintf(NomBis,511,_T("%s/lib/%s"),CheminModules,Nom);
			F=czfopen(NomBis,_T("rb"));
		}
	}

#ifndef _WIN32
	//On essaye enfin dans /usr/lib
	if (!F)
	{
		if (!Extension)
			czcpy(e,_T(".gpp"));
		carac NomBis[512];
		cznprintf(NomBis,511,_T("/usr/lib/gpp/%s"),Nom);
		F=czfopen(NomBis,_T("rb"));
		if (!F && !Extension)
		{
			czcpy(e,_T(".gpt"));
			cznprintf(NomBis,511,_T("/usr/lib/gpp/%s"),Nom);
			F=czfopen(NomBis,_T("rb"));
		}
	}
#endif
	return F;
}

void Programme::ChargerBinaire(const carac * Fichier)
{
#if (0)
	FILE * F=OuvrirFichier(Fichier,CheminModules);
	if (!F)
		throw CErreur(TXT("impossible d'ouvrir le fichier %s"),Fichier);
	try
	{

	//DefProchainFichier(_T(""));

	bool Prepare=false;
	//On détruit tout
//	Int.Reinit();

	varGlob.Vider();
/*	if (Ins.c)
		free(Ins.c);
	Ins.c=NULL;
	Ins.a=0;
	Ins.t=0;
	Ins.i=NULL;*/
	Module.Vider();
	Switch.Vider();
	Label.Vider();
	fichierSource.Vider();

	InitSauveur();

	char Chunk[5];
	Chunk[4]=0;
	uint Taille;
	fread(Chunk,4,1,F);
	if (strcmp(Chunk,"RIFF"))
	{
		fclose(F);
		throw TXT("ce n'est pas un fichier RIFF");
	}
	fread(&Taille,4,1,F);
	fread(Chunk,4,1,F);
	if (strcmp(Chunk,"gpp "))
	{
		fclose(F);
		throw TXT("ce fichier RIFF ne contient pas du GOTO++");
	}
	int Taille2;
	while (Taille>0)
	{
		fread(Chunk,4,1,F);
		fread(&Taille2,4,1,F);
		Taille-=Taille2+8;
		if ((Taille2<0)||(Taille<0))
			throw TXT("taille erronée d'un champ RIFF");
		if (strcmp(Chunk,"vars")==0)
		{
			fread(&varGlob.l,sizeof(varGlob.l),1,F);
			varGlob.Redim(varGlob.l);
			for (index_t v=0; v<varGlob.l; v++)
			{
	/*			BYTE l;
				fread(&l,1,1,F);
				fread(Variable[v].Nom,l,1,F);
				Variable[v].Nom[l]=0;*/
				varGlob[v].Charger(F);
			}
			/*while (Taille2>0)
			{
				int Taille3;
				fread(Chunk,4,1,F);
                fread(&Taille3,4,1,F);
				Taille2-=Taille3+8;
				if ((Taille2<0)||(Taille3<0))
					goto erreur;
				if (strcmp(Chunk,"vari")==0)
				{
					Variable.Etendre();
					Variable.Dernier().Options=0;
					Variable.Dernier().Valeur.Type=TYPE_CHANCES;
					while (Taille3>0)
					{
						int Taille4;
						fread(Chunk,4,1,F);
						fread(&Taille4,4,1,F);
						Taille3-=Taille4+8;
						if ((Taille3<0)||(Taille4<0))
							goto erreur;

						if (strcmp(Chunk,"nom ")==0)
						{
                            fread(Variable.Dernier().Nom,Taille4,1,F);
							if (Taille4>TMAXIDENTIFIEUR)
								goto erreur;
							Variable.Dernier().Nom[Taille4]=0;
						}
						else if (strcmp(Chunk,"type")==0)
						{
                            if (Taille4!=1)
								goto erreur;
							fread(&Variable.Dernier().Valeur.Type,1,1,F);
						}
						else if (strcmp(Chunk,"vale")==0)
						{
							if (Taille4>sizeof(valeurunion))
								goto erreur;
							fread(&Variable.Dernier().Valeur.v,Taille4,1,F);
						}
						else
							fseek(F,Taille4,SEEK_CUR);
					}
					if (Variable.Dernier().Valeur.Type==TYPE_CHANCES)
						Variable.Dernier().Valeur.v.f=float(rand())/(RAND_MAX+1.0f)*100;
				}
				else
					fseek(F,Taille3,SEEK_CUR);
			}*/
		}
		else
		if (strcmp(Chunk,"fsrs")==0)
		{
			while(Taille2>0)
			{
				int Taille3;
				fread(Chunk,4,1,F);
				fread(&Taille3,4,1,F);
				Taille2-=Taille3+8;
				if ((Taille2<0)||(Taille3<0))
					throw TXT("taille erronée d'un champ RIFF");

				if (strcmp(Chunk,"fsrc")==0)
				{
					while (Taille3>0)
					{
						fichierSource.Etendre(new FichierSource());
						fichierSource.Dernier()->Source=NULL;
						int Taille4;
						fread(Chunk,4,1,F);
						fread(&Taille4,4,1,F);
						Taille3-=Taille4+8;
						if ((Taille4<0)||(Taille3<0))
							throw TXT("taille erronée d'un champ RIFF");

						if (strcmp(Chunk,"nom ")==0)
						{
							if (Taille4>511)
								throw TXT("nom d'un module trop long");
							fread(&fichierSource.Dernier()->Nom,Taille4,1,F);
							fichierSource.Dernier()->Nom[Taille4]=0;
						}
						else
							fseek(F,Taille4,SEEK_CUR);
					}
				}
				else
					fseek(F,Taille3,SEEK_CUR);
			}
		}
		else
		if (strcmp(Chunk,"etiq")==0)
		{
			Label.Reserve(Taille2/8);
			for (int a=0; a<Taille2/8; a++)
			{
				Label.Etendre(new SLabel());
				fread(&Label.Dernier()->Numero,4,1,F);
				fread(&Label.Dernier()->Valeur,4,1,F);
			}
		}
		else
		if (strcmp(Chunk,"code")==0)
		{
/*			Ins.c=(char*)malloc(Taille2);
			Ins.a=Taille2;
			Ins.t=Taille2;
			fread(Ins.c,Taille2,1,F);*/
//			DebutLigne=Ins.c;
		}
		else
		if (strcmp(Chunk,"swis")==0)
		{
			while(Taille2>0)
			{
				int Taille3;
				fread(Chunk,4,1,F);
				fread(&Taille3,4,1,F);
				Taille2-=Taille3+8;
				if ((Taille2<0)||(Taille3<0))
					throw TXT("taille erronée d'un champ RIFF");

				if (strcmp(Chunk,"swit")==0)
				{
					Switch.Etendre(new CSwitch());
					byte Type;
					fread(&Type,1,1,F);
					fread(&Switch.Dernier()->PosDefaut,sizeof(int),1,F);
                    Taille3-=1+sizeof(int);
					int nCas=Taille3/(2*sizeof(int));
					Switch.Dernier()->Cas.Redim(nCas);
					CCasSwitch * cas=Switch.Dernier()->Cas.t;
					Switch.Dernier()->Cas.l=nCas;
					for (int c=0; c<nCas; c++)
					{
						fread(&cas->Valeur,sizeof(int),1,F);
						fread(&cas->Pos,sizeof(int),1,F);
						cas++;
					}
				}
				else
					fseek(F,Taille3,SEEK_CUR);
			}
		}
		else
		if (strcmp(Chunk,"inte")==0)
		{
/*			PreparerInt(&Int);
			Prepare=true;
//			Int.Charger(F,Taille2);*/
			throw "chargement de l'interpréteur non supporté";
		}
		else
			fseek(F,Taille2,SEEK_CUR);
	}
/*	if (!Prepare)
		PreparerInt(&Int);*/
	}
	catch(...)
	{
		fclose(F);
		throw;
	}
	fclose(F);
#endif
}

bool FichierSource::Charger(const carac * Fichier, size_t l, const carac * CheminModules)
{
	Source=NULL;
	memcpy(Nom,Fichier,(l+1)*sizeof(carac));
	Nom[l]=0;
	FILE * F=OuvrirFichier(Nom,CheminModules);
	if (!F)
		throw CErreur(TXT("fichier %.*s introuvable"),l,Fichier);

	char RIFF[5];
	fread(RIFF,4,1,F);
	RIFF[4]=0;
	if (strcmp(RIFF,"RIFF")==0)
	{
		fclose(F);
		return false;
	}
	fseek(F,0,SEEK_END);
	Taille=ftell(F);
	rewind(F);
#ifdef _UNICODE
	uchar *src=new uchar[Taille+1];
	fread(src,Taille,1,F);
	fclose(F);

	Source=new carac[Taille+1];
	for (size_t i=0; i<Taille; i++)
		Source[i]=src[i];
	delete[]src;
#else
	Source=new carac[Taille+1];
	fread(Source,Taille,1,F);
	fclose(F);
#endif
	Source[Taille]=0;
	return true;
}

int GotoPP::CompCasSwitch(const void * a, const void *b)
{
	if (((CCasSwitch*)a)->Valeur>((CCasSwitch*)b)->Valeur)
		return 1;
	else
	if (((CCasSwitch*)a)->Valeur<((CCasSwitch*)b)->Valeur)
		return -1;
	else
		return 0;
}

