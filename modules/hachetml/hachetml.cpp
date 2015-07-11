/*

Module HTML pour GOTO++
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
					 
Voilà de quoi faire du CGI avec le GOTO++. Et le pire c'est
que ça MARCHE ! Le problème c'est que chez les hébergeurs,
l'interpréteur GOTO++ est rarement présent.
*/
#ifdef _WIN32
#	include <gotopp/gppmodule.h>
#	include <io.h>
#	include <fcntl.h>
#else
#	include <gotopp/gppmodule.h>
#	include <ctype.h>
#define stricmp strcasecmp

inline void strlwr(char * c)
{
	while (*c)
		*(c++)=tolower(*c);
}

#endif

using namespace GotoPP;

//#define DEBUG_PARAM

class CHacheTML:public CIntModule
{
public:
	void ExtraitMeta(char *MetaVal, char * Meta, char * Var);
	void Meta(char ** contenu, char ** metaNom, char **metaVal);
	bool Param_MultiPart(char * contenu, size_t longueur, char * contentType);
	bool Variable_Creer(char * Nom, char * Contenu, size_t Longueur, char * NomFichier,
								 char * ContentType);
	bool Param_URL(char * Input);
	void LitParams();
	void HTML_Message(const char *c)
	{
		puts(c);
	}
	void HTML_Erreur(const char *c)
	{
		puts(c);
	}
};


#define SAUTERL(x) if (*(x)=='\n') (x)++; else (x)+=2;
#define COUPERL(x) if (*(x)=='\n') {*(x)=0;(x)++;} else {*(x)=0; (x)+=2;}
#define COUPEPRL(x) if (*(x-2)=='\r') {*(x-2)=0;} else {*(x-1)=0;}

int hValeur, hNomFichier,hContentType;
char NomVariable[256];
char NomFichier[512];

void CHacheTML::ExtraitMeta(char *MetaVal, char * Meta, char * Var)
{
	char *p=MetaVal;
	*p=0;
	if (Var!=NULL)
	{
		Meta=strstr(Meta,Var);
		if (Meta==NULL)
			return;
		Meta=strstr(Meta,"=");
		if (Meta==NULL)
			return;
		Meta++;
	}

	if (*Meta=='"')
	{
		Meta++;
		while ((*Meta) && (*Meta!='"'))
			*(p++)=*(Meta++);
	}
	else
	{
		while ((*Meta) && (*Meta!=';'))
			*(p++)=*(Meta++);
	}
	*p=0;
}


void CHacheTML::Meta(char ** contenu, char ** metaNom, char **metaVal)
{
	*metaNom=*contenu;
	while ((**contenu) && (**contenu!='\r') && (**contenu!='\n') && (**contenu!=':'))
		(*contenu)++;
	if (**contenu==':')
	{
		**contenu=0;
		(*contenu)++;
		while (**contenu==' ')
			(*contenu)++;
		*metaVal=*contenu;

		while ((**contenu!='\r') && (**contenu!='\n') && (**contenu))
			(*contenu)++;
	}
	else
		*metaVal=NULL;
	COUPERL(*contenu);
}

char * cherche(char *a, char *b, size_t l)
{
	int i;
	while (l)
	{
   		i=0;
		while (((a[i]==b[i]) || (b[i]==0)) && (l-i))
		{
			if (b[i]==0)
				return a;
			i++;
		}
		a++;
		l--;
	}
	return NULL;
}

bool CHacheTML::Param_MultiPart(char * contenu, size_t longueur, char * contentType)
{
	char * suite;
	char * metaNom,*metaVal;
	char * Depart;
	char Temp[256];

	if (contentType != NULL)
	{
		ExtraitMeta(Temp,contentType,NULL);
		if ((strcmp(Temp,"multipart/form-data")==0)
			||(strcmp(Temp,"multipart/mixed")==0))
		{
			char boundary[256];
			ExtraitMeta(boundary,contentType,"boundary");
#ifdef DEBUG_PARAM
			HTML_Message("NOT Multipart:");
#endif
			if (*boundary==0)
				return false;//HTML_Erreur("Le délimiteur des données du formulaire n'est pas défini.");

#ifdef DEBUG_PARAM
			HTML_Message("NOT Boudary :");
			HTML_Message(boundary);
#endif
			
			do
			{
				contentType=NULL;
				Depart=contenu;
				//On saute le boundary précédé de deux -
				contenu+=2+strlen(boundary);
				//Si le boundary est suivi d'un tiret on met fin à la boucle
				if (*contenu=='-')
					break;
				SAUTERL(contenu);
				suite=cherche(contenu,boundary,longueur-(contenu-Depart));
				if (suite==NULL)
					break;
				suite-=2; //On positionne la suite au premier tiret (--boundary)
				do
				{
					Meta(&contenu, &metaNom,&metaVal);
					//Si la ligne n'est pas vide
					if (*metaNom)
					{
						strlwr(metaNom);
						
#ifdef DEBUG_PARAM
						HTML_Message("NOT -- Meta :");
						HTML_Message(metaNom);
						if (metaVal)
							HTML_Message(metaVal);
#endif
						if (strcmp(metaNom,"content-type")==0)
						{
							contentType=metaVal;
#ifdef DEBUG_PARAM
							HTML_Message("NOT Content-type défini");
#endif
						}
						if (strcmp(metaNom,"content-disposition")==0)
						{
#ifdef DEBUG_PARAM
							HTML_Message("NOT Content-disposition défini");
#endif
							ExtraitMeta(Temp, metaVal,"name");
							if (*Temp)
							{
#ifdef DEBUG_PARAM
								HTML_Message(Temp);
#endif
								strncpy(NomVariable,Temp,256);
							}
							ExtraitMeta(NomFichier, metaVal,"filename");
#ifdef DEBUG_PARAM
							if (*NomFichier)
								HTML_Message(NomFichier);
#endif
						}
#ifdef DEBUG_PARAM
						HTML_Message("NOT -- Analyse du méta terminée");
#endif
					}
				}
				while (*metaNom);
				COUPEPRL(suite);
				Param_MultiPart(contenu, suite-contenu-2, contentType);
				contenu=suite;
				longueur-=int(suite-Depart);
			}
			while (longueur>0);
			return true;
		}
	}
#ifdef DEBUG_PARAM
	HTML_Erreur("NOT Nom de variable :");
    HTML_Erreur(NomVariable);
	HTML_Erreur("NOT Valeur de la variable :");
	HTML_Erreur(contenu);
#endif
	Variable_Creer(NomVariable,contenu,longueur,NomFichier,contentType);
#ifdef DEBUG_PARAM
	HTML_Erreur("NOT Variable créée");
#endif
	return true;
}

bool CHacheTML::Variable_Creer(char * Nom, char * Contenu, size_t Longueur, char * _NomFichier,
								 char * _ContentType)
{
	if (Nom==NULL)
		Nom="";

	void *v;

	v=LireVariable(hValeur);
	Ajouter(v,Nom,Contenu,Longueur);
	if (_NomFichier)
	{
		v=LireVariable(hNomFichier);
		Ajouter(v,Nom,_NomFichier,strlen(_NomFichier));
	}
	if (_ContentType)
	{
		v=LireVariable(hContentType);
		Ajouter(v,Nom,_ContentType,strlen(_ContentType));
	}
	return true;
}

inline void PlusesToSpaces(char *Str)
// Convertit les + en espaces
{
    if (Str != NULL)
	{
		while (*Str != '\0')
        {
			if (*Str == '+')
				*Str = ' ';
			Str++;
		}
	}
}

inline int HexVal(char c)
// Renvoie la valeur d'un caractère correspondant à un chiffre hexadécimal
{
	if ((c>='0') && (c<='9')) return (c-'0');
	if ((c>='a') && (c<='f')) return (c-'a'+10);
	if ((c>='A') && (c<='F')) return (c-'A'+10);
	return 0;
}

inline void TranslateEscapes(char *Str)
// Convertis les %XX en caractères dont le XX est la valeur ASCII
{
	char *NextEscape;
	int AsciiValue;

	NextEscape = strchr(Str, '%');
	while (NextEscape != NULL)
	{
		AsciiValue = (16 * HexVal(NextEscape[1])) + HexVal(NextEscape[2]);
		*NextEscape = (char) AsciiValue;
		memmove(&NextEscape[1], &NextEscape[3], strlen(&NextEscape[3]) + 1);
		NextEscape = strchr(&NextEscape[1], '%');
	}
}

bool CHacheTML::Param_URL(char * Input)
//Décodage des paramètres codés URL
{
	// les variables sont séparées par le caractére "&" 

	char *pToken;
	char *NomVar,*ValVar;

	pToken = Input;
	while (*pToken)
	{
		NomVar=pToken;
		while ((*pToken) && (*pToken!='=') && (*pToken!='&'))
			pToken++;
		if (*pToken=='=')
		{
			if (*pToken) *(pToken++)=0;
			ValVar=pToken;
			while ((*pToken) && (*pToken!='&'))
				pToken++;
			if (*pToken) *(pToken++)=0;
			PlusesToSpaces(ValVar);
			TranslateEscapes(ValVar);
#ifdef DEBUG_PARAM
			HTML_Erreur("NOT Nouvelle variable :");
			HTML_Erreur(NomVar);
			HTML_Erreur(ValVar);
#endif
			Variable_Creer(NomVar,ValVar,strlen(ValVar),"","text/plain");
		}
		else
		{
			if (*pToken) *(pToken++)=0;
			Variable_Creer(NomVar,"",0,"","text/plain");
		}
	}

	return true;
}

void CHacheTML::LitParams()
//Lecture des paramètres
{
#ifdef DEBUG_PARAM
	HTML_Erreur("NOT Lecture des paramètres :");
#endif
	char *contentType=NULL;
	size_t argLength;
	char *Input;

	char *requestMethod =getenv("REQUEST_METHOD");

	if (requestMethod && stricmp(requestMethod,"POST")==0)
	{
		// Méthode POST
		//On lit les données envoyées dans l'ent tête
		argLength=atoi(getenv("CONTENT_LENGTH"));

	#ifdef DEBUG_PARAM
		HTML_Erreur("NOT Longueur des données");
	#endif
		
		if (argLength == 0)
			return;
		
		Input=(char*)malloc(argLength+1);
#ifdef _WIN32
		_setmode( _fileno( stdin ), _O_BINARY );
#endif
		fread(Input,argLength,1,stdin);
		Input[argLength]=0;
	}
	else
	{
		// Méthode GET
		char * tp=getenv("QUERY_STRING");
		if (tp==NULL)
		{
			Input=(char*)malloc(sizeof("a=12"));
			strcpy(Input,"a=12");
		}
		else
		{
			argLength=strlen(tp);
			Input=(char*)malloc(argLength+1);
			memcpy(Input,tp,argLength+1);
		}
	}
    
#ifdef DEBUG_PARAM
	HTML_Erreur("NOT Données lues");
#endif

	contentType=getenv("CONTENT_TYPE");

	if (!contentType ||*contentType==0)
	{
#ifdef DEBUG_PARAM
		HTML_Erreur("NOT Content-type indéfini");
#endif
	}
	else
	{
#ifdef DEBUG_PARAM
		HTML_Erreur("NOT Content-type:");
		HTML_Erreur(contentType);
#endif
	}	

	if ((contentType==NULL) || (memcmp(contentType,"multipart/form-data",19)))
		Param_URL(Input);
	else
		Param_MultiPart(Input,argLength,contentType);

#ifdef DEBUG_PARAM
	HTML_Erreur("NOT Opération terminée");
#endif
	free(Input);
	return;
}

extern "C" EXPORT bool Init(CCompModule * Int)
{
	if (Int->Version<VersionMin)
		return false;

	hValeur=Int->NouvelleVariable(_T("Morf"));
	hNomFichier=Int->NouvelleVariable(_T("Reihcif"));
	hContentType=Int->NouvelleVariable(_T("Tnetnoc"));
	Int->NouvelleFonction(_T("SmarapTil"),voidf(&CHacheTML::LitParams));
	return true;
}
