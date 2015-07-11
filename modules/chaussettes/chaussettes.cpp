/*

Module chaussettes pour le GOTO++ (accès au réseau)
Copyright 2002-2005 Sidoine de Wispelaere

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
					 
Ce module permet d'utiliser BSD sockets pour accéder au réseau
depuis le GOTO++, ce qui en fait un langage de choix pour
prototyper rapidement des protocoles client/serv... ahah..
client/serveur.
*/
#ifdef _WIN32
#	include <gotopp/gppmodule.h>
#	include <winsock.h>
#	define MSG_NOSIGNAL 0
#else
#	include <gotopp/gppmodule.h>
#	include <unistd.h>
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <netdb.h>
#	include <memory.h>
#	include <stdlib.h>
#	include <stdio.h>
#	define INVALID_SOCKET -1
#	define closesocket close
	typedef int SOCKET;
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif
#endif

using namespace GotoPP;

namespace GotoPP
{
	IGotoPP * iGotoPP;
}

namespace Chaussettes
{
	class BaseChaussette:public IObjet
	{
	public:
		SOCKET socket;
		char * Tampon;
		int lTampon,aTampon;
		BaseChaussette():Tampon(0),lTampon(0),aTampon(0),socket(INVALID_SOCKET)
		{
		}
		~BaseChaussette();
	};

	class Chaussette:public BaseChaussette
	{
	public:
		void Tricoter(const IChaineA * ip, int port);
		void Remplir(const IChaineA * texte);
		const IChaine * Vider();
		const IChaine * LireLigne();
		void Fermer();
	};

	class Chaussure:public BaseChaussette
	{
	public:
		void Cordonnier(int port);
		Chaussette * Chausser();
	};

	const IChaine * MonNom();
	int ResoudreIPvQ(const IChaineA * ip);

	const int SOCKET_INVALIDE=-1;
	const size_t MAXHOSTNAME=512;

	inline bool VALIDE(SOCKET s)
	{
		return int(s)>=0;
	}

	inline bool INVALIDE(SOCKET s)
	{
		return int(s)<0;
	}
}

//Interface avec le GOTO++
using namespace Chaussettes;

EXPORT bool Initialiser(IGotoPP * _iGotoPP)
{
	iGotoPP=_iGotoPP;
#ifdef _WIN32
	WSADATA Donnees;
	if (WSAStartup(MAKEWORD( 2, 2 ),&Donnees))
		return false;
#endif
	return iGotoPP->TesterVersion();
}

EXPORT void EnregistrerSymboles(IProgramme * programme, ISymbole * parent)
{
	ISymbole * classeChaussure=CreateurClasseC<Chaussure>::Creer(programme,parent,_T("Chaussure"));
	CreateurAppelMethodeC_1<Chaussure,int>::AjouterFonction<&Chaussure::Cordonnier>(classeChaussure,_T("Cordonnier"));
	CreateurAppelMethodeC_R0<Chaussure,Chaussette*>::AjouterFonction<&Chaussure::Chausser>(classeChaussure,_T("Chausser"));
	
	ISymbole * classeChaussette=CreateurClasseC<Chaussette>::Creer(programme,parent,_T("Chaussette"));
	CreateurAppelMethodeC_2<Chaussette,const IChaineA*,int>::AjouterFonction<&Chaussette::Tricoter>(classeChaussette,_T("Tricoter"));
	CreateurAppelMethodeC_1<Chaussette,const IChaineA*>::AjouterFonction<&Chaussette::Remplir>(classeChaussette,_T("Remplir"));
	CreateurAppelMethodeC_R0<Chaussette,const IChaine*>::AjouterFonction<&Chaussette::Vider>(classeChaussette,_T("Vider"));
	CreateurAppelMethodeC_R0<Chaussette,const IChaine*>::AjouterFonction<&Chaussette::LireLigne>(classeChaussette,_T("LireLigne"));
	CreateurAppelMethodeC_0<Chaussette>::AjouterFonction<&Chaussette::Fermer>(classeChaussette,_T("Fermer"));

	CreateurAppelFonctionC_R0<const IChaine*>::AjouterFonction<&MonNom>(parent,_T("MonNom"));
	CreateurAppelFonctionC_R1<int,const IChaineA*>::AjouterFonction<&ResoudreIPvQ>(parent,_T("ResoudreIPvQ"));
}

EXPORT void Fermer()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

//Implémentation

namespace Chaussettes
{

	BaseChaussette::~BaseChaussette()
	{
		if (VALIDE(socket))
			closesocket(socket);
	}

	void Chaussette::Fermer()
	{
		if (VALIDE(socket))
		{
			closesocket(socket);
			socket=INVALID_SOCKET;
		}
	}

/*AIDE Chaussure%Cordonnier
Crée une chaussure attendant une connexion.
\module chaussettes
\param port entier À quel port attacher la chaussette.
\exemple
#! Cet exemple crée un serveur qui envoie
un message au client qui se connecte, puis qui quitte. !#
GOTOMODULE chaussettes

Chaussure test
test@Cordonnier(*(1256))
GOTOPRINT()
Chaussette sauvage connexion
connexion=test@Chausser()
GOTOPRINT()
connexion@Remplir(«Bonjour mon gars»)
*/
	void Chaussure::Cordonnier(int port)
	{
		char   NomHote[MAXHOSTNAME+1];
  		SOCKET    s;
		sockaddr_in sa;
		hostent *hp;

		memset(&sa, 0, sizeof(struct sockaddr_in));
		gethostname(NomHote, MAXHOSTNAME);
		hp= gethostbyname(NomHote);
		if (hp == NULL)
			throw _T("Impossible de trouver le nom d'hôte");

		sa.sin_family= hp->h_addrtype;
		sa.sin_port= htons(short(port));
		if ((s= ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
			throw _T("Impossible de créer un socket");

	#ifndef _WIN32
		int  aut=1;
		setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&aut,sizeof(int));
	#endif
		if (bind(s,(struct sockaddr *)&sa,sizeof(struct sockaddr_in)) < 0)
		{
			closesocket(s);
			throw _T("Impossible de lier le socket au port (déjà utilisé ?)");
		}
		listen(s, 3);
	//	puts("Tricoter");
		if (VALIDE(socket))
			closesocket(socket);
		socket=(int)s;
	}

/*AIDE Chaussure%Chausser
Attend qu'une chaussette se connecte à la chaussure.
\module chaussettes
\retour chaussette Chaussette La chaussette.
\desc Si la fonction réussit, la fonction renvoie la chaussette du client 
(créée de l'autre côté d'Internet avec $Chaussette%Tricoter$).
Quand vous remplissez cette chaussette, le client
pourra la vider et inversement.
*/
	Chaussette * Chaussure::Chausser()
	{
		SOCKET t;
		if (INVALIDE(socket))
			throw _T("la chaussette n'a pas été tricotée");
		
		if ((t = accept(socket,NULL,NULL)) < 0)
			throw _T("Erreur lors du chaussage");

		Chaussette * chaussette=(Chaussette*)iGotoPP->Allouer(sizeof(Chaussette));
		new (chaussette) Chaussette();
		chaussette->socket=t;
		return chaussette;
	}

/*AIDE Chaussette%Tricoter
Connecte une chaussette à une chaussure se trouvant sur un autre serveur.
\module chaussettes
\param adresse chaine L'adresse du serveur auquel se connecter.
\param port entier Le port du serveur auquel se connecter.
\desc La machine $Fermer$ doit être appelée quand on en a fini avec.
\exemple
#! Cet exemple se connecte à un serveur et affiche le contenu
du premier paquet reçu. !#
GOTOMODULE chaussettes

Chaussette Test
ret=Test@Tricoter(«localhost» *(1256))
GOTOPRINT()
ret=Test@Vider(texte)
GOTOPRINT()
GOTOPRINTDUTEXTE() ; &texte
*/
	void Chaussette::Tricoter(const IChaineA * ip, int port)
	{
		sockaddr_in sa;
		hostent *hp;
		SOCKET s;

		if ((hp=gethostbyname(ip->c)) == NULL)
			throw _T("Impossible de trouver le serveur");

		memset(&sa,0,sizeof(sa));
		memcpy((char *)&sa.sin_addr,hp->h_addr,hp->h_length);
		sa.sin_family= hp->h_addrtype;
		sa.sin_port= htons(short(port));


		if ((s= ::socket(hp->h_addrtype,SOCK_STREAM,0)) < 0)
			throw _T("Impossible de créer un socket");

		if (connect(s,(struct sockaddr *)&sa,sizeof sa) < 0)
		{
			closesocket(s);
			throw _T("Impossible de se connecter au serveur");
		}

		if (VALIDE(socket))
		{
			closesocket(socket);
		}

		if (Tampon)
			free(Tampon);
		Tampon=0;
		lTampon=0;
		socket=s;
	}

/*AIDE Chaussette%Remplir
Remplit une chaussette avec des données, dans l'espoir que quelque part
dans le vaste Internet quelqu'un la videra. Avec le GOTO++, c'est un peu
tous les jours Noël.
\module chaussettes
\param cadeau chaine Le cadeau à mettre dans la chaussette.
*/
	void Chaussette::Remplir(const IChaineA * texte)
	{
		if (INVALIDE(socket))
			throw _T("la chaussette n'a pas été ouverte");
		
		size_t resteAEnvoyer=texte->l;
		const char * c=texte->c;
		while (resteAEnvoyer)
		{
			const int tailleMaxPaquet=65536;
			size_t envoyer=(resteAEnvoyer<tailleMaxPaquet)?resteAEnvoyer:tailleMaxPaquet;
			size_t envoye;
			if ((envoye=send(socket, c, (int)envoyer,MSG_NOSIGNAL))<=0)
				throw _T("Erreur en envoyant des données");

			resteAEnvoyer-=envoye;
			char *d=(char*)c;
			d+=envoye;
			c=(char*)d;    
		}
	}



/*AIDE Chaussette%Vider
Vide une chaussette (au besoin, attend que la chaussette soit pleine).
\module chaussettes
\retour chaine Le contenu de la chaussette, qui est vidée.
*/
	const IChaine * Chaussette::Vider()
	{
		char Buffer[65536];//Hum... Bof bof je dois dire
		//Etant paresseux, c'est le seul moyen que j'ai trouvé
		//pour que ça marche en MT.
		//Faudrait sans doute utiliser un mutex

		if (lTampon)
		{
			IChaine * ret=iGotoPP->AllouerChaine(Tampon,lTampon);
			lTampon=0;
			return ret;
		}
		if (INVALIDE(socket))
			throw _T("la chaussette est fermée");
		
		int l=recv(socket, Buffer,sizeof(Buffer),0);
		if (l<=0)
			throw _T("Connexion interrompue");
		return iGotoPP->AllouerChaine(Buffer,l);
	}

	const IChaine * Chaussette::LireLigne()
	{
		char * Fin=NULL;
		int p;
		do
		{
			if (Tampon && lTampon)
			{
				//On vérifie si il y a une ligne complète
				Fin=Tampon;
				p=0;
				while (p<lTampon && *Fin!='\n')
				{
					Fin++;
					p++;
				}
				if (*Fin=='\n')
					p++;
			}

			if (!Fin || *Fin!='\n')
			{
				//On lit la suite
				if (INVALIDE(socket))
					throw _T("la chaussette est fermée");

				if (aTampon-lTampon<65536)
				{
					aTampon+=65536;
					Tampon=(char*)iGotoPP->Reallouer(Tampon,aTampon);
				}
				int l=recv(socket, &Tampon[lTampon],65536,0);

				if (l<=0)
					throw _T("Connexion interrompue");

				int al=lTampon;
				lTampon+=l;
			}
			else
				break;
		}
		while (1);
			
		IChaine * ret=iGotoPP->AllouerChaine(Tampon,p);
		lTampon-=p;
		if (lTampon)
			memmove(Tampon,&Tampon[p],lTampon);
		return ret;
	}

	
/*AIDE MonNom
Renvoie le nom de l'ordinateur sur lequel tourne le programme.
\module chaussettes
\retour chaine Le nom de l'ordinateur.
*/
	const IChaine * MonNom()
	{
		char NomHote[MAXHOSTNAME];
		
		gethostname(NomHote, MAXHOSTNAME);
		return iGotoPP->AllouerChaine(NomHote);
	}

	
/*AIDE ResoudreIPvQ
Récupère un nom d'ordinateur ou une IP sous la forme 192.168.0.1 et la transforme
en un nombre entier.
\module chaussettes
\param nom chaine Le nom de l'ordinateur ou une IP sous forme de texte.
\retour entier L'IP sous forme d'entier.
*/
	int ResoudreIPvQ(const IChaineA * ip)
	{
		hostent *hp;
		
		hp= gethostbyname(ip->c);
			
		if (hp == NULL)
			throw _T("Impossible de trouver le nom d'hôte");
		return *(int*)hp->h_addr;
	}

}
