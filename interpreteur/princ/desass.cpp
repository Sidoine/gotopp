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
					 
Des bidules pour afficher le code désassemblé et d'autres machins
approchant.
*/
#include <gotopp/base.h>
#include <gotopp/programme.h>
#include <gotopp/instruction.h>
#include <gotopp/icompilateur.h>
#include "gpp.h"


using namespace GotoPP;
using namespace std;

namespace GotoPP
{
	extern DescCode descCode[];
}

void Gpp::Desass(Programme *programme,bool UneLigne,  GotoPP::BoutCode & Ins)
{
	int PosPile=0;
	int DerVar=-1;
	int nLignes=0;

	if (!UneLigne)
		Ins.i=Ins.depart;
	while (!Ins.Fin())
	{
		code c=Ins.LireCode();
		if (c == code(Code::I_DEBUTLIGNE))
		{
			nLignes++;
			if (UneLigne && nLignes==2)
				break;
		}
		if (!descCode[c].Nom[0])
		{
			puts("Instruction inconnue");
			return;
		}
		czoprintf(_T("%p %s"),Ins.i-sizeof(code), descCode[c].Nom);
		type t=TYPE_INCONNU;
		for (int a=0; a<descCode[c].nArgs;a++)
		{
			int z=descCode[c].Arg[a];
			if (z==TYPE_INCONNU)
				z=t;
			
			switch (z)
			{	
			case TYPE_CARAC:
				{
					carac caractere=Ins.LireCarac();
					if (byte(caractere)<32)
						czoprintf(_T(" ^%c"),'A'+caractere);
					else
						czoprintf(_T(" %c"),caractere);
					break;	
				}
			case TYPE_ETIQUETTE:
				{
					code * code=Ins.LireEtiquette();
					czoprintf(_T(" :%p [%s]"), code, programme->NomEtiquette(code));
					break;
				}
			case TYPE_ENTIER:
				czoprintf(_T(" %d"),Ins.LireEntier());
				break;
			case TYPE_CHANCES:
				czoprintf(_T(" %f"),Ins.LireChances());
				break;
			case TYPE_TYPE:
				t=Ins.LireType();
				czoprintf(_T(" %s"),NomType(t));
				break;
			case TYPE_ETIQLOC:
				{
					code * i=Ins.i;
					czoprintf(_T(" :%p"),i+Ins.LireEntier()+sizeof(int));
					break;
				}
			case TYPE_VARIABLELOC:
				czoprintf(_T(" %d"),Ins.LireVariable());
				break;
			case TYPE_VARIABLE:
				czoprintf(_T(" %s"),programme->NomVarGlob(Ins.LireEntier()));
				break;
			case TYPE_CHAINE:
				{
					const chaineval * c=Ins.LireChaine();
					czout << " «";
					fputcz(c->c,stdout);
					czout << '»';
					break;
				}
/*			case TYPE_CLASSE:
				{
					czout<<' '<<Ins.LireEntier();//Classe[Ins.LireEntier()].Nom;
					break;
				}*/
			case TYPE_FONCTIONC:
				{
					czout <<' '<< Ins.LireFonctionC();
					break;
				}
			case TYPE_OBJETC:
				{
					czout << "objet";
					Ins.LireObjet();
					break;
				}
			default:
				throw TXT("Code incompréhensible");
				return;
			}
		}
		putchar('\n');
	}	
}	

void Gpp::ListeVars(Programme * programme)
{
	for (index_t v=0; v<programme->varGlob.l;v++)
	{
		if (v)
			putchar(',');
		czoprintf(_T("%s"),programme->NomVarGlob(v));
	}
	putchar('\n');
}

void Gpp::ContenuPile(GotoPP::Programme * programme)
{
	/*fprintf(stderr,"Pile : ");
	for (unsigned int p=0; p<Pile.Pos+1; p++)
	{
		cerr << NomType(Pile.Pile[p].Type) << ' ' << Pile.Pile[p];
		if (p<Pile.Pos)
			cerr << ';';
	}
	fputc('\n',stderr);*/
}
