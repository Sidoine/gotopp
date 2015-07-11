/*

Interpréteur GOTO++
Copyright 2002 Sidoine de Wispelaere

Ce programme est un logiciel libre ; vous pouvez le redistribuer
et/ou le modifier sous les termes de la GNU General Public Licence
telle que publiée par la Free Software Fundation ; soit dans la
version 2 de la Licence, soit (à votre choix) dans une version
ultérieure.

Ce programme est distribué dans l_T('espoir qu')il sera utile mais
SANS AUCUNE GARANTIE ; sans même la garantie implicite qu'il soit
COMMERCIALISABLE ou ADEQUAT POUR UN USAGE PARTICULIER. Voir la
GNU General Public Licence pour plus de détails.

Vous devriez avoir reçu une copie de la GNU General Public Licence
avec ce programme ; dans le cas contraire, écrivez à :
Free Software Fundation, Inc.
59 Temple Place - Suite 330, Boston, MA  02111-1307, US

                          ****
					 
Point d_T('entrée de l')interpréteur. Affiche des tas de messages marrants.
Bref, des heures d'amusement en perspective.

*/
#include <gotopp/base.h>
#include <gotopp/compilateur.h>
#include <gotopp/erreur.h>
#include <gotopp/machine.h>
#include <gotopp/programme.h>
#include <gotopp/programme.h>
#include <gotopp/global.h>

#include "gpp.h"
#include "../interne/defbib.h"

#include <string>
#include <memory>
#ifdef _WIN32
#include <windows.h>
#else
#include <curses.h>
#endif

using namespace GotoPP;
using namespace std;
using namespace DefBib;

#define ENFONCTION(x) (voidf)&x

void Test2(const chaineval * texte)
{
	czout<<texte->c<<endl;;
}

class TestObjet:public ObjetNonClonable
{
	int valeur;
public:
	void Set(int a)
	{
		valeur=a;
	}
	int Get()
	{
		return valeur;
	}
};

void Gpp::FonctionsDefaut(Programme * programme)
{
	ISymbole * base=programme->symboleGlobal;
	programme->NouvelleVariable(_T("TexteErreur"),symboleValeur);

	//TestObjet * aa=(TestObjet*)GC_MALLOC(sizeof(TestObjet));
	//Construct(aa);
	
	//Symbole * test=programme->NouvelleClasseC(_T("testobjet"),sizeof(TestObjet),(Symbole*)0);
	/*ISymbole *test=CreateurClasseC<TestObjet>::Creer(programme,0,_T("testobjet"));
	CreateurAppelMethodeC_1<TestObjet,int>::AjouterFonction<&TestObjet::Set>(test,_T("Set"));
	CreateurAppelMethodeC_R0<TestObjet,int>::AjouterFonction<&TestObjet::Get>(test,_T("Get"));*/
	//TODO: gestion du constructeur
//	Symbole * testcstr=CreateurAppelMethodeC_R0<TestObjet,TestObjet*>::AjouterFonction<&TestObjet::

	//programme->NouvelleFonctionC(Test,_T("test"));
	//CreateurAppelFonctionC_1<const chaineval *>::AjouterFonction<Test2>(base,_T("test"));
//	programme->NouvelleFonctionC(ENFONCTION(DefBib::Test),_T("test"),_T(":e"));
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biGotoGoto),_T("GOTOGOTO"));
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biGotoChepa),_T("GOTOCHEPA"));
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biInitialisation),_T("INITIALISATION"));
	CreateurAppelFonctionC_R1<int, ITableauValeurs *>::AjouterFonction<DefBib::biTableauPoids>(base,_T("ChoisisUnEntier"));	
	
#ifdef _DEBUG
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biGotoPrintDuTexte),_T("p"));
#endif
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biGotoPrint),_T("GOTOPRINT"));
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biGotoPrintDuTexte),_T("GOTOPRINTDUTEXTE"));
	CreateurAppelFonctionC_R0<const chaineval *>::AjouterFonction<&DefBib::biEntreTonTexte>(base,_T("ENTRETONTEXTE"));
	CreateurAppelFonctionC_R0<int>::AjouterFonction<&DefBib::biEntreUnNombre>(base,_T("ENTREUNNOMBRE"));
	CreateurAppelFonctionC_R0<const chaineval *>::AjouterFonction<DefBib::biEntreTonTexteFurtif>(base,_T("ENTRETONTEXTEFURTIF"));
	CreateurAppelFonctionC_R2<int, const chaineval *, IValeur *>::AjouterFonction<DefBib::biRechercherDansChaine>(base,_T("OUQUILESTLETEXTE"));
	CreateurAppelFonctionC_R2<carac, const chaineval *, int>::AjouterFonction<DefBib::biCaractereDansChaine>(base,_T("LeCaracNumero"));
	CreateurAppelFonctionC_R3<const chaineval *, const chaineval *, int, int>::AjouterFonction<DefBib::biExtraireChaine>(base,_T("PrendsUnMorceau"));
	CreateurAppelFonctionC_R1<int, const chaineval *>::AjouterFonction<DefBib::biLongueurChaine>(base,_T("NombreDeLettres"));
	CreateurAppelFonctionC_R1<const chaineval*, const chaineval*>::AjouterFonction<DefBib::biChomp>(base,_T("chomp"));
	CreateurAppelFonctionC_R2<const chaineval*,const chaineval*, int>::AjouterFonction<DefBib::biBegue>(base,_T("BEGUEBEGUE"));
		
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biPremierPingouin),_T("PremierPingouin"));
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biTab),_T("BOITEAPINGOUINS"));	
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biPousse),_T("Pousse"));
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biPlop),_T("Plop"));
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biMetsDessous),_T("MetsDessous"));		
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biEnleveDessous),_T("EnleveDessous"));		
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biTab),_T("tab"));		
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biEnleveMilieu),_T("EnleveMilieu"));		
	
	CreateurAppelFonctionC_2<int, int>::AjouterFonction<DefBib::biGotoXY>(base,_T("gotoXY"));
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biCouleurTexte),_T("CouleurDuTexte"));
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biInfosConsole),_T("InfosConsole"));
	CreateurAppelFonctionC_0::AjouterFonction<DefBib::biNettoyerEcran>(base,_T("NettoyerEcran"));
	CreateurAppelFonctionC_R0<int>::AjouterFonction<DefBib::biTapeUneTouche>(base,_T("TapeUneTouche"));
	CreateurAppelFonctionC_1<int>::AjouterFonction<DefBib::biMontrerCurseur>(base,_T("MontrerCurseur"));
#ifdef _WIN32
	programme->NouvelleConstante(_T("TOUCHEBAS"),-80);
	programme->NouvelleConstante(_T("TOUCHEHAUT"),-72);
	programme->NouvelleConstante(_T("TOUCHEGAUCHE"),-75);
	programme->NouvelleConstante(_T("TOUCHEDROITE"),-77);
	programme->NouvelleConstante(_T("TOUCHEECHAP"),27);
#else
	programme->NouvelleConstante(_T("TOUCHEBAS"),KEY_DOWN);
	programme->NouvelleConstante(_T("TOUCHEHAUT"),KEY_UP);
	programme->NouvelleConstante(_T("TOUCHEGAUCHE"),KEY_LEFT);
	programme->NouvelleConstante(_T("TOUCHEDROITE"),KEY_RIGHT);
	programme->NouvelleConstante(_T("TOUCHEECHAP"),27);	
#endif
	
	CreateurAppelFonctionC_1<int>::AjouterFonction<DefBib::biDodo>(base,_T("DODO"));
	//programme->NouvelleFonctionC(ENFONCTION(DefBib::biSauver),_T("Sauver"));
	//programme->NouvelleFonctionC(ENFONCTION(DefBib::biCharger),_T("Charger"));
	CreateurAppelFonctionC_1<CTache*>::AjouterFonction<DefBib::biBigCrunch>(base,_T("BigCrunch"));
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biGotoDansTexte),_T("GOTODANSTEXTE"));

	CreateurAppelFonctionC_R1<int, const chaineval*>::AjouterFonction<DefBib::biGotoDOS>(base,_T("GOTODOS"));
	CreateurAppelFonctionC_R1<const chaineval *, const chaineval*>::AjouterFonction<DefBib::biLireEnv>(base,_T("ProtegeonsLaNature"));
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biGotoErreur),_T("GOTOerreur"));
	CreateurAppelFonctionC_0::AjouterFonction<DefBib::biBoum>(base,_T("boum"));
	CreateurAppelFonctionC_R2<ITableauValeurs*, const chaineval *, IValeur*>::AjouterFonction<DefBib::biBananaSplit>(base,_T("bananasplit"));
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biNombreChepa),_T("nombreCHEPA"));

	//programme->NouvelleFonctionC(ENFONCTION(DefBib::biRacine),_T("Carotte"),_T("e:e"));
	CreateurAppelFonctionC_R1<float,float>::AjouterFonction<sqrtf>(base,_T("Carotte"));
	CreateurAppelFonctionC_R2<float,float,float>::AjouterFonction<powf>(base,_T("CentMegaWatts"));
	programme->NouvelleFonctionC(&DefBib::tOUTeNpLUS,_T("tOUTeNpLUS"));
	
	CreateurAppelFonctionC_R2<int, const chaineval*, const chaineval*>::AjouterFonction<DefBib::biCompChaines>(base,_T("ORDRALPHABETIX"));
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biMarkov),_T("Markov"));
	CreateurAppelFonctionC_R2<const chaineval *, ITableauValeurs*, const chaineval*>::AjouterFonction<DefBib::biJoindre>(base,_T("Marijuana"));	
	CreateurAppelFonctionC_R3<const chaineval *, const chaineval *, regexp*, const chaineval*>::AjouterFonction<DefBib::biRemplacer>(base,_T("Rxmplacxr"));	
	
	CreateurAppelFonctionC_R1<const chaineval*, entier64>::AjouterFonction<DefBib::biTexteDate>(base,_T("TexteDate"));
	CreateurAppelFonctionC_R0<entier64>::AjouterFonction<DefBib::biHeure>(base,_T("Quelleheureestil"));
	CreateurAppelFonctionC_R1<entier64,entier64>::AjouterFonction<DefBib::biDateFichierEnDate>(base,_T("DateFichierEnDate"));
	programme->NouvelleFonctionC(ENFONCTION(DefBib::biDateIncomprehensible),_T("DateIncomprehensible"));

	CreateurAppelFonctionC_R0<const chaineval*>::AjouterFonction<DefBib::biRepCourant>(base,_T("RepCourant"));
	CreateurAppelFonctionC_R0<const chaineval*>::AjouterFonction<DefBib::biRepParent>(base,_T("RepParent"));
	CreateurAppelFonctionC_R2<const chaineval*, const chaineval* , const chaineval*>::AjouterFonction<DefBib::biConcatRep>(base,_T("ConcatRep"));
	CreateurAppelFonctionC_R1<const chaineval*, const chaineval*>::AjouterFonction<DefBib::biETTelephone>(base,_T("ETTelephone"));
		
	CreateurClasseC<CTache>::Creer(programme,base,_T("universparallele"));
	
	/*CreateurAppelMethodeC_R0<Fichier,int>::AjouterFonction<&Fichier::Taille>(programme,classeFichier,_T("Taille"));
	CreateurAppelMethodeC_R0<Fichier,int>::AjouterFonction<&Fichier::LireMode>(programme,classeFichier,_T("Mode"));
	NouvellePropriete(c,_T("Taille"),PROP_PUBLIQUE,ENFONCTION(CFoncFichier::Taille),(voidf)(NULL));
	NouvellePropriete(c,_T("Mode"),PROP_PUBLIQUE,ENFONCTION(CFoncFichier::LireMode),ENFONCTION(CFoncFichier::ChgMode));*/

	CreerIO(programme);
	CreerSysteme(programme);
}

void Gpp::AjouterArgs(Programme * programme, int argc,carac**argv, int DebutArg)
{
	CType type(symboleChaine);
	type.Empiler(symboleTableau);
	programme->NouvelleVariable(_T("parametre"),type);
	tableauval * t =programme->varGlob.Dernier().v.t=new tableauval();
	t->l=argc-DebutArg;
	if (t->l)
	{
		t->v=(valeur*)GC_MALLOC(sizeof(valeur)*t->l);
		valeur * z=(valeur*)t->v;
		for (int a=DebutArg; a<argc; a++)
		{
			z->Type=TYPE_CHAINE;
			z->v.c=chaineval::Creer(argv[a]);
			z++;
		}
	}
	else
		t->v=NULL;
}


void AfficherAide()
{
	czerr <<
		TXT("Interpréteur GOTO++ version ")<<Version<<TXT(" ")<<NomVersion<<TXT("\n")
		TXT("L'humanité bénéficie du GOTO++ grâce à Sidoine De Wispelaere\n")
		TXT("et Alexandre Béraud.\n\n")
		TXT("Syntaxe :\n")
		TXT("  gotopp [-ahotvX] source [...]\n")
		TXT("\n")
		TXT("Interprète le fichier 'source'\n")
		TXT("\n")
		TXT("Options\n")
		TXT("  a : Affiche le code désassemblé\n")
		TXT("  h : Affiche l'aide\n")
		TXT("  i : Interactif\n")
		TXT("  o : Enregistre le code compilé dans a.out (*)\n")
		TXT("  s : Demande l'appui d'une touche avant de quitter\n")
		TXT("  t : Trace les GOTO (*)\n")
		TXT("  v : Affiche la liste des variables globales (*)\n")
		TXT("  X : N'exécute pas le programme\n")
		TXT("\n")
		TXT("Les paramètres qui suivent le nom du fichier source sont passés au programme.\n")
		TXT("Les expressions régulières utilisent une version modifiée du code de Henry\n")
		TXT("Spencer.\n")
		TXT("(*) Temporairement désactivé\n");
}

carac Insultes[]=
	TXT("T'es vraiment pas doué.\0")
	TXT("C'est pas la première fois que tu fais cette erreur !\0")
	TXT("« Le GOTO++ pour les nuls » est disponible en librairie, penses-y.\0")
	TXT("Je sens que tu vas venir me dire « Ca marche paaas ! »\0")
	TXT("Quelle faute honteuse !\0")
	TXT("La faute du débutant.\0")
	TXT("Bon courage !\0")
	TXT("La bêtise du NIAC est bête, hinhinhin.\0")
	TXT("Mais c'est pas vrai d'être *#@{§¤ à ce point !\0")
	TXT("Ahahahahah !\0")
	TXT("Laisse tomber.\0")
	TXT("Je te conseille de programmer en BASIC.\0")
	TXT("On est un gourou ou on n'en est pas un.\0")
	TXT("Ecrire à Josiane Delpont pour plus d'informations sur cette erreur.\0")
	TXT("Voir page 7830 du manuel du programmeur.\0")
	TXT("Moi aussi j'ai fait cette erreur quand j'étais jeune.\0")
	TXT("Le GOTO++ est pourtant simple, je ne comprends pas.\0")
	TXT("Essaye de rajouter @%Ne en fin de ligne.\0")
	TXT("Pour plus d'informations appelez le support technique.\0")
	TXT("Errore : kaput da machini von kas ën virtuelmac.gpp:78945\0")
	TXT("Suis la voie de Nostradamus et tu ne feras plus d'erreurs.\0")
	TXT("Nostradamus est grand, Agonar et Sidoine sont ses prophètes !\0")
	TXT("Espèce d'âne !\0");

void Insulte()
{
	int nInsultes=1;
	carac * i=Insultes;
	while (*i || i[1])
	{	
		if (*i==0)
			nInsultes++;
		i++;
	}
	int j=rand()%nInsultes;
	i=Insultes;
	while (j)
	{
		while (*i)
			i++;
		i++;
		j--;
	}
	czerr << i << _T("\n");
}

void AfficherErreur(const carac * texte, const carac * fichier,
					int ligne, bool execution)
{
	DefBib::RestaurerConsole();
	if (execution)
		czerr<<TXT("Erreur à l'exécution : ");
	else
		czerr<<TXT("Erreur à la compilation : ");
	czerr << texte << TXT(" en ")
		<< fichier << TXT(":") << ligne <<endl;
	if (!execution)
		Insulte();
}
int czmain(int argc, carac ** argv)
{
	carac * Fichier=NULL;
	bool Desass=false;
	bool ListeVars=false;
	bool Trace=false;
	bool Executer=true;
	bool Enregistrer=false;
	bool stop=false;
	bool interactif=false;

	int DebutArg=argc;

	GotoPP::AfficherErreur=&::AfficherErreur;
	InitConsole();

	for (int a=1; a<argc; a++)
	{
		if (*argv[a]=='-'
#ifdef _WIN32
			|| *argv[a]=='/'
#endif
			)
		{
			carac * b=&argv[a][1];
			while (*b)
			{
				switch(*b)
				{
				case 'a':
					Desass=true;
					break;
				case 'h':
					AfficherAide();
					return 0;
				case 'i':
					interactif=true;
					break;
				case 'o':
					/*cerr << _T("Fonction o désactivée\n");
					return 1;*/
					Enregistrer=true;
					break;
				case 's':
					stop=true;
					break;
				case 't':
					Trace=true;
					break;
				case 'v':
					ListeVars=true;
					break;
				case 'X':
					Executer=false;
					break;
				default:
					cerr << TXT("Option inconnue. Utilisez -h pour afficher l'aide.\n");
					return 1;
				}
				b++;
			}
		}
		else if (Fichier==NULL)
		{
			Fichier=argv[a];
			DebutArg=a+1;
			break;
		}
	}

				
	if (Fichier==NULL && !interactif)
#ifdef _DEBUG
		Fichier=_T("exemple.gpp");
#else
	{
		AfficherAide();
		return 0;
	}
#endif
	GotoPP::Init();

	static GotoPP::Programme programme;
	Gpp::FonctionsDefaut(&programme);
	Gpp::AjouterArgs(&programme,argc,argv,DebutArg);

	if (interactif)
	{
		basic_string<carac> commande;
		basic_string<carac> parametres;
		do
		{
#ifdef _WIN32
			//Si on n'est pas en début de ligne, on va à la ligne
			CONSOLE_SCREEN_BUFFER_INFO csbf;
			GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbf);
			if (csbf.dwCursorPosition.X!=0)
				czout<<endl;
#endif

			czout<<_T("(GOTO++) ");
			czin>>commande;
			getline(czin,parametres,_T('\n'));
			if (commande==_T("quit") || commande==_T("exit") || commande==_T("bye"))
				return 0;
			if (commande==_T("goto"))
			{
				try
				{
					auto_ptr<BoutCode> bc(programme.compilateur->Compiler(parametres.c_str()));
					CInterpreteur interpreteur(&programme);
					interpreteur.ExecuterCatch(bc->depart);
				}
				catch(const carac * c)
				{
					programme.compilateur->AfficherErreur(c);
				}
				catch(CErreur e)
				{
					programme.compilateur->AfficherErreur(e);
				}
			}
			else
				czout<<TXT("Commande ")<<commande<<TXT(" inconnue\n");
		}
		while (1);
		/*
		BoutCode * bc=Compilateur::Compiler(Code.v.c.v->c);
	CInterpreteur interpreteur;
	interpreteur.Executer(bc->depart);*/
	}

	try
	{
//		while (*Compilateur::ProchainFichier())
		{
			auto_ptr<BoutCode> bc(programme.compilateur->Charger(Fichier));
			
			/*if (Enregistrer)
				bc->SauverBinaire(_T("a.out"),false);*/

			if (Desass)
				Gpp::Desass(&programme,false,*bc);
			/*if (ListeVars)
				Int.ListeVars();*/
			if (Executer)
			{
				CInterpreteur interpreteur(&programme);
				//interpreteur.Tracer=Trace;
				interpreteur.ExecuterCatch(bc->depart);
			}
		}
	}
	catch(const carac * c)
	{
		programme.compilateur->AfficherErreur(c);
	}
	catch(CErreur e)
	{
		programme.compilateur->AfficherErreur(e);
	}


	RestaurerConsole();

	if (stop)
		getchar();
	return 0;
}

