
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
Tout pour l'interpréteur, mis à part la machine
virtuelle qui est dans machine.cpp
*/

#include <gotopp/base.h>
#include <gotopp/erreur.h>
#include <gotopp/machine.h>
#include <gotopp/programme.h>

#include "../commun/sauveur.h"
#include <iostream>

#ifdef _WIN32
#include <process.h>
//typedef TACHE 
#else
#include <pthread.h>
#endif

using namespace GotoPP;
using namespace std;

void CInterpreteur::Sauver(FILE *F)
{
	/*fwrite("inte",4,1,F);
	DWORD DebutInt=ftell(F);
	DWORD TailleInt=0;
	fwrite(&TailleInt,4,1,F);

	//On enregistre des données diverses
	fwrite("etat",4,1,F);
	DWORD Taille=sizeof(void*)+sizeof(Ligne)+
		2*sizeof(DebutVarLoc)+2*sizeof(size_t);
	fwrite(&Taille,4,1,F);
	fwrite(&This.v,sizeof(void*),1,F);
	fwrite(&Ligne,sizeof(Ligne),1,F);
	fwrite(&DebutParam,sizeof(DebutParam),1,F);
	fwrite(&DebutVarLoc,sizeof(DebutVarLoc),1,F);
	size_t PosInstr=Ins.i-Ins.c;
	fwrite(&PosInstr,sizeof(size_t),1,F);
	PosInstr=DebutLigne-Ins.c;
	fwrite(&PosInstr,sizeof(size_t),1,F);

	//On enregistre les variables locales
	if (PileF.l)
	{
		fwrite("vloc",4,1,F);
		DWORD Debut=ftell(F);
		DWORD Taille=0;
		fwrite(&Taille,4,1,F);
		fwrite(&PileF.l,4,1,F);
		for (index_t v=0; v<PileF.l;v++)
			PileF[v].Sauver(F);
		Taille=ftell(F)-Debut-4;
		fseek(F,Debut,SEEK_SET);
		fwrite(&Taille,4,1,F);
		fseek(F,0,SEEK_END);
	}
	
	//On enregistre la pile secondaire
	if (Pile2.Pos)
	{
		fwrite("pil2",4,1,F);
		DWORD Debut=ftell(F);
		DWORD Taille=0;
		fwrite(&Taille,4,1,F);
		fwrite(&PileF.l,4,1,F);
		for (index_t v=0; v<Pile2.Pos;v++)
			Pile2.Pile[v].Sauver(F);
		Taille=ftell(F)-Debut-4;
		fseek(F,Debut,SEEK_SET);
		fwrite(&Taille,4,1,F);
		fseek(F,0,SEEK_END);
	}
	
	//On enregistre les appels
	if (nAppels)
	{
		fwrite("appl",4,1,F);
		DWORD Taille=(5*sizeof(size_t)+sizeof(int))*nAppels;
		fwrite(&Taille,4,1,F);
		for (int i=0; i<nAppels; i++)
		{
			size_t ins=Appel[i].Instruction;
			fwrite(&ins,sizeof(size_t),1,F);
			fwrite(&Appel[i].lPile,sizeof(size_t),1,F);
			fwrite(&Appel[i].PosPileF,sizeof(size_t),1,F);
			fwrite(&Appel[i].This,sizeof(void*),1,F);
			fwrite(&Appel[i].EnErreur,sizeof(int),1,F);			
			fwrite(&Appel[i].nParams,sizeof(size_t),1,F);						
		}
	}
	TailleInt=ftell(F)-DebutInt-4;
	fseek(F,DebutInt,SEEK_SET);
	fwrite(&TailleInt,4,1,F);
	fseek(F,0,SEEK_END);*/
}
	

void CInterpreteur::Charger(FILE * F,int Taille)
{
	/*char Chunk[5];
	Chunk[4]=0;
	int T2;
	while (Taille>0)
	{
		fread(Chunk,4,1,F);
		fread(&T2,4,1,F);
		Taille-=T2+8;
		if (Taille<0)
			throw "fichier erroné";
		if (memcmp(Chunk,"etat",4)==0)
		{
	        	fread(&This.v,sizeof(void*),1,F);
			if (This.v&&Pointeur.Nouveau((void*&)This.v,TYPE_TABLEAU,sizeof(tableauval)))
				throw "hum, this n'avait pas été défini (non supporté)";
			fread(&Ligne,sizeof(Ligne),1,F);
			fread(&DebutParam,sizeof(DebutParam),1,F);
			fread(&DebutVarLoc,sizeof(DebutVarLoc),1,F);
			size_t PosInstr;
			fread(&PosInstr,sizeof(size_t),1,F);
			Ins.i=&Ins.c[PosInstr];
			fread(&PosInstr,sizeof(size_t),1,F);
			DebutLigne=&Ins.c[PosInstr];
		}
		else
		if (memcmp(Chunk,"vloc",4)==0)
		{
			fread(&PileF.l,4,1,F);
			PileF.Redim(PileF.l);
			for (size_t v=0; v<PileF.l;v++)
				PileF[v].Charger(F);
		}
		else
		if (memcmp(Chunk,"pil2",4)==0)
		{
			fread(&Pile2.Pos,4,1,F);
			for (size_t v=0; v<Pile2.Pos;v++)
				Pile2.Pile[v].Charger(F);
		}
		else
		if (memcmp(Chunk,"appl",4)==0)
		{
			nAppels=Taille/(sizeof(size_t)*5+sizeof(int));
            		for (int i=0; i<nAppels; i++)
			{
				size_t ins;
				fread(&ins,sizeof(size_t),1,F);
				Appel[i].Instruction=(int)ins;
				fread(&Appel[i].lPile,sizeof(size_t),1,F);
				fread(&Appel[i].PosPileF,sizeof(size_t),1,F);
				fread(&Appel[i].This,sizeof(void*),1,F);
				fread(&Appel[i].EnErreur,sizeof(int),1,F);			
				fread(&Appel[i].nParams,sizeof(size_t),1,F);						

				if (Appel[i].This && Pointeur.Nouveau((void*&)Appel[i].This,TYPE_TABLEAU,sizeof(tableauval)))
					throw "hum, this n'avait pas été défini pour un appel (non supporté)";
			}
		}
		else
			fseek(F,T2,SEEK_CUR);
	}*/
}

void CInterpreteur::Reinit()
{
	This=new tableauval();
	for (int a=0; a<nAppels; a++)
	{
		Appel[a].This=0;
	}
	nAppels=0;
	Pile.Vider();
	Pile2.Vider();
}

namespace DefBib
{
	void RestaurerConsole();
}

void CInterpreteur::ExecuterCatch()
{
	try
	{
		Executer();
	}
	catch(const carac * c)
	{
		AfficherErreur(c);
	}
	catch(CErreur & e)
	{
		AfficherErreur(e);
	}
	Ins.i=DebutLigne;
}

carac* CInterpreteur::AcFichierSource()
{
	size_t MeilleureVal=0;
	index_t Meilleur=~0;
	code* i=Ins.i;
	for (index_t s=0; s<programme->fichierSource.l; s++)
	{
		if (i>=programme->fichierSource[s]->DebutCode && i<=programme->fichierSource[s]->FinCode)
		{
			size_t Distance=i-programme->fichierSource[s]->DebutCode;
			if (Distance>=MeilleureVal)
			{
				MeilleureVal=Distance;
				Meilleur=s;
			}
		}
	}
	if (Meilleur!=~0)
		return programme->fichierSource[Meilleur]->Nom;
	else
		return TXT("<code dynamique>");
}

#ifdef _WIN32
void
#else
void *
#endif
 ExecuterIntTache(void * _T)
{
	CTache * T=(CTache*)_T;
	T->Int->ExecuterCatch();
	T->Int->programme->Tache.Supprimer(T);
#ifndef _WIN32
	return NULL;
#endif
}

CTache * CInterpreteur::NouvelleTache(code* Depart)
{
	CInterpreteur * Int=new CInterpreteur(programme);
	//((CCompilateur*)Compilateur)->PreparerInt(Int);
	Int->Ins.i=Depart;
	index_t nl=PileF.l-DebutParam;
	Int->PileF.Redim(nl);
	Int->DebutVarLoc=DebutVarLoc-DebutParam;
	Int->DebutParam=0;
	Int->BasenParams=Int->DebutVarLoc;
	for (index_t i=0; i<nl; i++)
	{
		Int->PileF[i]=PileF[DebutParam+i];
	}

	//Initialiser quelques valeurs
	Int->Ligne=Ligne;
	Int->Pile2.Pos=0;

	CTache * tache=new CTache();
	programme->Tache.Etendre(tache);
	tache->Int=Int;
#ifdef _WIN32
	tache->t=_beginthread(ExecuterIntTache,0,
		(void*)tache);
#else
	pthread_create(&tache->t,NULL,&ExecuterIntTache,
		(void*)tache);
#endif
	return tache;
	//return ((CCompilateur*)Compilateur)->NouvelleTache(Int);
}

/*int CCompilateur::NouvelleTache(CInterpreteur * Int)
{
	int t=(int)Tache.Nouveau();
	Tache[t].Int=Int;
	Tache[t].i=t;
#ifdef _WIN32
	Tache[t].t=_beginthread(ExecuterIntTache,0,(void*)&Tache[t]);
#else
	pthread_create(&Tache[t].t,NULL,&ExecuterIntTache,(void*)&Tache[t]);
#endif
	return t;
}*/

CInterpreteur::CInterpreteur(Programme * _programme):ThisC(0),nAppels(0),Ligne(0),DebutLigne(0),
				DebutParam(0),DebutVarLoc(0),BasenParams(0)
{
	UtiliserProgramme(_programme);

	Appel[0].EnErreur=0;
	
	//Copier les variables globales
	/*varGlob.t=GotoPP::varGlob.t;
	Variable.l=GotoPP::varGlob.l;
	varGlob.a=~0;//On ne redimensionne pas
*/
	//Copier le code VM
//	Int->Ins.ExpReg=Ins.ExpReg;

	/*Copier les étiquettes
	Label.l=GotoPP::Label.l;
	Label.t=GotoPP::Label.t;
	Label.a=~0;

	Copier les fonctions
	nFonctions=(int)GotoPP::FonctionInt.l;
	FonctionInt=GotoPP::FonctionInt.t;
	FonctionMod=GotoPP::FonctionMod.t;
	*/
	//Initialiser quelques valeurs
	//Int->Mutex=GotoPP::Mutex::m;
//	nAppels=0;
//	memset(&Int->Appel[0],0,sizeof(CAppel));
//	Appel[0].EnErreur=NULL;
	
/*	Int->This.Init();
	Int->DerVar.Type=TYPE_INCONNU;
	Int->Pile.Pos=0;
	Int->Pile2.Pos=0;
	Int->PileF.Init();
	Int->ThisC=NULL;*/
}

void CInterpreteur::Executer(code *i)
{
	Ins.i=i;
	Executer();
}

void CInterpreteur::ExecuterCatch(code *i)
{
	Ins.i=i;
	ExecuterCatch();
}

void CInterpreteur::UtiliserProgramme(Programme * _programme)
{
	programme=_programme;
	Switch=_programme->Switch.t;
	Label=_programme->Label.t;
	varGlob=programme->varGlob.t;
}

IObjet * CInterpreteur::GetThis()
{
	return ThisC;
}
