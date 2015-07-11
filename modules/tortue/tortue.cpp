/*

Interpréteur GOTO++
Copyright 2002-2006 Sidoine de Wispelaere, Sylvain Glaize

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

Module permettant l'affichage d'une tortue à la LOGO					 

1er janvier 2006 : nouvelle interface
25 juin 2003 : version Linux -- Sylvain Glaize
*/


#ifdef _WIN32
#	include <windows.h>
#	include <math.h>
#else
#	include <X11/Xlib.h>
#	include <math.h>

GC      X11_GraphicContext;
Display *X11_Display;
int     X11_Screen;
Window  X11_Window, X11_Root;
unsigned long X11_White_pixel, X11_Black_pixel;

struct POINT {
    int x;
    int y;
};

#endif
#include <vector>
#include <gotopp/gppmodule.h>

using namespace GotoPP;
using namespace std;

namespace GotoPP
{
	IGotoPP * iGotoPP;
}

namespace Tortue
{
	class CTortue;

/*CLASSE Tortue
Une fenêtre avec un fond blanc et une ligne noire, laquelle est le tracé de la tortue.
Les tortues doivent être placées dans des $Fenetre$.
\module tortue
*/
	class Fenetre:public IObjet
	{
	public:
	#ifdef _WIN32
		HWND hWnd;
	#else
		void * hWnd;
	#endif
	public:
		~Fenetre();
		Fenetre():hWnd(0){}
	public:
		void Creer();
		CTortue * CreerTortue();
		void Constructeur();
	};

	void GererFenetres();

/*CLASSE Fenetre
Une fenêtre avec un cadre et tout. On peut y mettre des $Tortue$ dedans.
\module tortue
*/		
	class CTortue:public IObjet
	{
	public:
		std::vector<POINT> Points;
		std::vector<unsigned long> Poly;
		int Angle;
		double x,y;
	#ifdef _WIN32
		HWND hWnd;
	#else
		Window * hWnd;
	#endif
	public:
		CTortue():hWnd(0){}
		~CTortue();
		void Avancer(float Longueur);
		void Sauter(float longueur);
		void Tourner(int angle);
		void Nettoyer();
	};

}

using namespace Tortue;

#ifdef _WIN32
	LRESULT CALLBACK GestionTortues(
		HWND hwnd,        // handle to window
		UINT uMsg,        // message identifier
		WPARAM wParam,    // first message parameter
		LPARAM lParam)
	{
		CTortue * t=(CTortue *)(size_t)GetWindowLongPtr(hwnd,GWLP_USERDATA);
		switch (uMsg)
		{
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc=BeginPaint(hwnd,&ps);
				DWORD l=(DWORD)t->Poly.size();
				if (l && t->Poly[l-1]==0)
					l--;
				if (l)
					PolyPolyline(hdc,&t->Points[0],&t->Poly[0],l);

				EndPaint(hwnd,&ps);
				break;
			}

		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam); 
		}
		return 0;
	}

	LRESULT CALLBACK GestionFenetres(
		HWND hwnd,        // handle to window
		UINT uMsg,        // message identifier
		WPARAM wParam,    // first message parameter
		LPARAM lParam)
	{
		Fenetre * Fen=(Fenetre* )(size_t)GetWindowLongPtr(hwnd,GWLP_USERDATA);
		if (uMsg==WM_DESTROY)
		{
			PostQuitMessage(0);
			return 0;
		}
		else
			return DefWindowProc(hwnd, uMsg, wParam, lParam); 
	}
#endif

EXPORT bool Initialiser(IGotoPP * _iGotoPP)
{
	iGotoPP=_iGotoPP;
 
#ifdef _WIN32
	//On crée la classe de fenêtre
	WNDCLASSEX wc;
	memset(&wc,0,sizeof(wc));
	wc.cbClsExtra=0;
	wc.cbSize=sizeof(wc);
	wc.cbWndExtra=sizeof(void*);//l'objet
	wc.hbrBackground=(HBRUSH)COLOR_BACKGROUND+1;
	wc.hCursor=LoadCursor (NULL, IDC_ARROW);
	wc.hIcon=NULL;
	wc.hIconSm=NULL;
	wc.hInstance=GetModuleHandle(NULL);
	wc.lpfnWndProc=GestionFenetres;
	wc.lpszClassName=_T("fenetre");
	wc.lpszMenuName=NULL;
	wc.style=0;
	ATOM ret=RegisterClassEx(&wc);
	if (!ret)
		return false;
	
	//On crée la classe de tortue
	memset(&wc,0,sizeof(wc));
	wc.cbClsExtra=0;
	wc.cbSize=sizeof(wc);
	wc.cbWndExtra=sizeof(void*);
	wc.hbrBackground=(HBRUSH)COLOR_BACKGROUND+1;
	wc.hCursor=LoadCursor (NULL, IDC_ARROW);
	wc.hIcon=NULL;
	wc.hIconSm=NULL;
	wc.hInstance=GetModuleHandle(NULL);
	wc.lpfnWndProc=GestionTortues;
	wc.lpszClassName=_T("tortue");
	wc.lpszMenuName=NULL;
	wc.style=0;
	ret=RegisterClassEx(&wc);
	if (!ret)
		return false;
#else
    if(XInitThreads()==0)
        throw _T("Peut pas initialiser les threads");

    X11_Display = XOpenDisplay(NULL);
    if (!X11_Display)
        throw _T("impossible de se connecter à l'afficheur");
    X11_Screen = DefaultScreen (X11_Display);
    X11_GraphicContext = DefaultGC (X11_Display, X11_Screen);
    X11_Root = RootWindow (X11_Display, X11_Screen);
    X11_White_pixel = WhitePixel (X11_Display, X11_Screen);
    X11_Black_pixel = BlackPixel (X11_Display, X11_Screen);
    XSetBackground (X11_Display, X11_GraphicContext, X11_White_pixel);
    XSetForeground (X11_Display, X11_GraphicContext, X11_Black_pixel);
#endif
    return iGotoPP->TesterVersion();
}

EXPORT void EnregistrerSymboles(IProgramme * programme, ISymbole * parent)
{
	CreateurAppelFonctionC_0::AjouterFonction<&GererFenetres>(parent,_T("GererFenetres"));

	//Classe Fenetre
	ISymbole * classeFenetre=CreateurClasseC<Fenetre>::Creer(programme,parent,_T("Fenetre"));
	CreateurAppelMethodeC_0<Fenetre>::AjouterFonction<&Fenetre::Creer>(classeFenetre,_T("Creer"));
	CreateurAppelMethodeC_R0<Fenetre,CTortue*>::AjouterFonction<&Fenetre::CreerTortue>(classeFenetre,_T("CreerTortue"));
	
	ISymbole * classeTortue=CreateurClasseC<CTortue>::Creer(programme,parent,_T("Tortue"));
	CreateurAppelMethodeC_1<CTortue,float>::AjouterFonction<&CTortue::Avancer>(classeTortue,_T("Avancer"));
	CreateurAppelMethodeC_1<CTortue,float>::AjouterFonction<&CTortue::Sauter>(classeTortue,_T("Sauter"));
	CreateurAppelMethodeC_1<CTortue,int>::AjouterFonction<&CTortue::Tourner>(classeTortue,_T("Tourner"));
	CreateurAppelMethodeC_0<CTortue>::AjouterFonction<&CTortue::Nettoyer>(classeTortue,_T("Nettoyer"));
}

EXPORT void Fermer()
{

}

// Implémentation

namespace Tortue
{
	CTortue::~CTortue()
	{
	#ifdef _WIN32
		DestroyWindow(hWnd);
	#endif
	}

	#ifndef _WIN32
	CTortue * TheTortue = NULL;
	#endif

	Fenetre::~Fenetre()
	{
	#ifdef _WIN32
		DestroyWindow(hWnd);
	#else
		XDestroyWindow(X11_Display,X11_Window);
	#endif
	}

/*AIDE Fenetre%Creer
Crée une fenêtre vide.
\module tortue
*/
	void Fenetre::Creer()
	{
	#ifdef _WIN32
		hWnd=CreateWindow(_T("fenetre"),_T("GOTO++"),WS_OVERLAPPEDWINDOW|WS_VISIBLE,
			CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
			NULL,NULL,GetModuleHandle(NULL),NULL);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG)(LONG_PTR)(size_t)this);
		if (!hWnd)
			throw _T("Impossible de créer la fenêtre");
	#else
		X11_Window = XCreateSimpleWindow (X11_Display, X11_Root,
						0, 0, 640, 480, 5, X11_Black_pixel, X11_White_pixel);
		XSelectInput (X11_Display, X11_Window, ExposureMask|DestroyNotify);
		XStoreName (X11_Display, X11_Window, "Goto++"); /* titre de la fenetre */
		XMapWindow (X11_Display, X11_Window);
	#endif
	}

/*AIDE Fenetre%CreerTortue
Crée une tortue à l'intérieur de la fenêtre.
\module tortue
\retour Tortue Renvoie la tortue qui se trouve dans la fenêtre.
*/
	CTortue * Fenetre::CreerTortue()
	{
		CTortue * t=(CTortue*)iGotoPP->Allouer(sizeof(CTortue));
		new (t)CTortue();

	#ifdef _WIN32
		RECT Taille;
		GetClientRect(hWnd,&Taille);
		t->hWnd=CreateWindow(_T("tortue"),_T("Tortue"),WS_CHILD|WS_VISIBLE,
			0,0,Taille.right,Taille.bottom,
			hWnd,NULL,GetModuleHandle(NULL),NULL);

		if (!t->hWnd)
			throw (_T("Impossible de créer la fenêtre pour la tortue"));
	#else
		if(TheTortue)
			throw _T("une seule tortue !");
		t->hWnd=&X11_Window;
	#endif
		t->Poly.resize(t->Poly.size()+1);
		t->Poly.back()=0;
		t->Angle=0;
		
	#ifdef _WIN32
		t->x=Taille.right/2;
		t->y=Taille.bottom/2;
		SetWindowLongPtr(t->hWnd,GWLP_USERDATA,(LONG)(LONG_PTR)t);
	#else
		t->x=320;
		t->y=240;
		TheTortue = t;
	#endif
		return t;
	}

/*AIDE GererFenetres
\module tortue
\desc
Gère les fenêtres. La fonction bloque le programme jusqu'à ce que toutes
les fenêtres soient fermées.
*/
	void GererFenetres()
	{
	#ifdef _WIN32
		MSG msg;
		BOOL bRet;
		while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
		{ 
			if (bRet == -1)
			{
				return;//Erreur
			}
			else
			{
				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
			}
		} 
	#else
		int i;
		CTortue *t = TheTortue;
		for(;;) {
			XEvent ev;
			XNextEvent (X11_Display, &ev);
			switch (ev.type) {
			case Expose :
				if(t->Points.size()>1)
				{
					XLockDisplay  (X11_Display);
					for(i=0;i<t->Points.size()-1;i++)
					{
						XDrawLine(X11_Display,
								  *(t->hWnd),
								  X11_GraphicContext,
								  t->Points[i  ].x,t->Points[i  ].y,
								  t->Points[i+1].x,t->Points[i+1].y);
					}
					XFlush        (X11_Display);
					XUnlockDisplay(X11_Display);
				}
				break;
			case DestroyNotify :
				//XDestroyWindow(X11_Display, X11_Window);
				//XCloseDisplay(X11_Display);
				return;
				break;
			}
		}
	#endif
	}

	
/*AIDE Tortue%Tourner
Fait tourner la tortue.
\module tortue
\param angle entier L'angle en degrés. Positif pour tourner dans le sens inverse des
aiguilles d'une montre.
*/

	void CTortue::Tourner(int angle)
	{
		Angle+=angle;
	}


	const double PI=3.14159;

/*AIDE Tortue%Sauter
Fait faire un bond d'une certaine longueur à la tortue.
\module tortue
\param distance chances La longueur du bond.
\desc
En gros, c'est comme $Tortue%Avancer$, sauf que la tortue ne laisse pas
de trace sur son chemin.
*/
	void CTortue::Sauter(float longueur)
	{
		//Si on avait tracé quelquechose, on passe au polygone suivant
		if (Poly.back())
		{
			Poly.resize(Poly.size()+1);
			Poly.back()=0;
		}
		x+=(longueur*cos(double(Angle)/PI));
		y+=(longueur*sin(double(Angle)/PI));
	}

	
/*AIDE Tortue%Nettoyer
Efface toutes les traces de la tortue.
\module tortue
*/
	void CTortue::Nettoyer()
	{
		Poly.resize(0);
		Points.resize(0);
	#ifdef _WIN32
		RECT rect;
		GetClientRect(hWnd,&rect);
		x=rect.right/2;
		y=rect.bottom/2;
		InvalidateRect(hWnd,NULL,TRUE);
	#else
		XClearWindow (X11_Display, X11_Window);
		XFlush (X11_Display);
	#endif
	}

/*AIDE Tortue%Avancer
Fait avancer la tortue.
\module tortue
\param distance chances La distance sur laquelle la tortue doit se déplacer, en pixels.
*/
	void CTortue::Avancer(float Longueur)
	{
		//Si on n'avait encore rien tracé, on indique le point de départ
		if (Poly.back()==0)
		{
			Poly.back()=1;
			POINT pt;
			pt.x=int(x);
			pt.y=int(y);
			Points.push_back(pt);
		}
		int depx=(int)x;
		int depy=(int)y;

		//On se déplace
		x+=(Longueur*cos(double(Angle)/180*PI));
		y+=(Longueur*sin(double(Angle)/180*PI));

		//On rajoute le point
		POINT pt;
		pt.x=int(x);
		pt.y=int(y);
		Points.push_back(pt);
		Poly.back()++;

		//On dessine le trait
	#ifdef _WIN32
		HDC hdc=GetDC(hWnd);
		MoveToEx(hdc,depx,depy,NULL);
		LineTo(hdc,(int)x,(int)y);
		ReleaseDC(hWnd,hdc);
	#else
		XLockDisplay  (X11_Display);
		XDrawLine     (X11_Display,*(hWnd),X11_GraphicContext,depx,depy,(int)x,(int)y);
		XFlush        (X11_Display);
		XUnlockDisplay(X11_Display);
	#endif
	}

}
