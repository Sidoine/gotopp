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
					 
Des fonctions intégrées pour gérer les chaînes.
*/
#include <gotopp/base.h>
#include "defbib.h"
#include <string>

using namespace GotoPP;
using namespace std;

namespace DefBib
{

/*AIDE Chomp
Supprime les retours à la ligne en bout de chaîne, si il y en a.
\param chaine chaine La chaîne se terminant éventuellement par un ou plusieurs retours à la ligne.
\retour chaine La même chaîne, mais sans les retours à la ligne à la fin.
\desc
Certaines fonctions malpolies (du module chaussette par exemple) laissent le retour
à la ligne. Cette fonction vorace permet de le supprimer si nécessaire.

Ce qui la différencie de celle des autres langages, c'est qu'elle est
très vorace et supprime autant de retours à la ligne que possible.
*/
const chaineval * biChomp(const chaineval * chaine)
{
	const carac *c=chaine->c;
	c+=chaine->l;
	if (chaine->l && (c[-1]=='\n' ||  c[-1]=='\r'))
	{
		chaineval * n=chaineval::Creer(chaine->c,chaine->l);
		carac *c=&n->c[n->l];
		carac *dep=n->c;
		while (c>dep && (c[-1]=='\n' ||  c[-1]=='\r'))
		{
			*(--c)=0;
			n->l--;
		}
		return n;
	}
	else
		return chaine;
}

const chaineval * biBegue(const chaineval * chaine, int repetitions)
{
	chaineval * ret=chaineval::Creer(repetitions*chaine->l);
	carac * p=ret->c;
	size_t t=chaine->l;
	for (int i=0; i<repetitions; i++)
	{
		memcpy(p,chaine->c,t*sizeof(carac));
		p+=t;
	}
	*p=0;
	return ret;
}

/*AIDE OUQUILESTLETEXTE
Recherche une chaîne de caractère dans une autre chaîne de caractère.
\param ou chaine Chaîne où l'on cherche quelquechose.
\param quoi pingouin Chaîne que l'on cherche ou expression régulière.
\retour entier Numéro du caractère où £quoi£ a été trouvée dans £ou£, ou 0 si elle n'a pas été trouvée.
\exemple
Trouve=OUQUILESTLETEXTE(«Bonjour Josiane Delpont» «Josiane»)
GOTOPRINT()
GOTO Affiche 9.
*/
int biRechercherDansChaine(const chaineval * texte, IValeur * _atrouver)
{
	valeur * atrouver=static_cast<valeur*>(_atrouver);

	const carac * c=texte->c;
	int ret;
	if (atrouver->Type==TYPE_OBJETC)
	{
		//TODO: RTTI
		regexp* ver=atrouver->v.er;
		ret=ver->Exec(c);
		//DerExpReg=atrouver->v.er;
		if (ver->Chaine)
		{
			chaineval *z=(chaineval*)ver->Chaine;
		}
		ver->Chaine=texte;
		if (ret)
			ret=int(1+size_t(ver->startp[0]-c));
	}
	else
	{
		const carac * r=atrouver->Chaine()->c;
		unsigned int i=1;
		const carac *res=czstr(c,r);
		if (res)
			ret=int(1+size_t(res-c));
		else
			ret=0;
	}
	return ret;
}

/*AIDE NombreDeLettres
Donne la longueur de la chaîne.
\param chaine chaine La chaîne dont on veut savoir la longueur.
\retour entier La longueur de la chaîne.
\desc
Renvoie la longueur de la chaîne.

Songez à utiliser tailleenlongueur si cette chaîne est dans une variable.
\exemple
GOTOPRINTDUTEXTE() }NombreDeLettre(«Nostradamus»){
GOTO Affiche 11
*/
int biLongueurChaine(const chaineval * chaine)
{
	return (int)chaine->l;
}

/*AIDE LeCaracNumero
Renvoie un caractère de la chaîne.
\param chaine chaine Chaîne de caractères dont on veut un caractère.
\param numero entier Le numéro du caractère voulu, à partir de 1.
\retour carac Le caractère.
\desc
Cette fonction a sans doute une utilité.
\exemple
Carac=LeCaracNumero(«abcde» *(4))
GOTOPRINT()
GOTO Affiche d
*/
carac biCaractereDansChaine(const chaineval * chaine, int position)
{
	if (position>0 && position<=int(chaine->l))
		return chaine->c[position-1];
	else
		return ' ';
}


/*AIDE bananasplit
Découpe une chaîne en morceaux et crée un tableau.
\param chaine chaine La chaîne à découper.
\param separateur pingouin Une chaîne ou une expression régulière servant de séparateur.
\retour tableau Un tableau comprenant les différents bouts de la chaîne.
\desc
Accessoirement, c'est une banane coupée en deux, avec de la glace
et du chocolat fondu (à quelque chose prêt).
\exemple
z=bananasplit(«1,2,34» «,»)
GOTOPRINTDUTEXTE(&z<*(2)>) GOTO Affiche 34
*/
//À améliorer !
ITableauValeurs * biBananaSplit(const chaineval * chaine, IValeur * _motif)
{
	const carac * c=chaine->c;
	valeur * motif=static_cast<valeur*>(_motif);
	
	tableauval * t=new tableauval();
	
	size_t Extension=chaine->l/80;
	if (Extension<=2)
		Extension=10;
	else if (Extension>1024)
		Extension=1024;
	index_t Alloue=(index_t)Extension;
	t->Dim(Alloue);
		
	int p=0;
	const carac * sol=c;
		
	if (motif->Type!=TYPE_OBJETC) 
	{
		const chaineval * mot=motif->Chaine();
		const carac * m=mot->c;
		int lm=(int)mot->l;
		const carac * suite;
	
		do
		{
			suite=czstr(sol,m);
			if (p==Alloue)
			{
				Alloue+=(index_t)Extension;
				t->Dim(Alloue);
			}

			t->Get(p).Type=TYPE_CHAINE;
			if (suite)
			{
				t->Get(p++).v.c=chaineval::Creer(sol,suite-sol);
			}
			else
			{
				t->Get(p++).v.c=chaineval::Creer(sol);
				break;
			}
			sol=suite+lm;
		}
		while (1);
	}
	else
	{
		//TODO: RTTI
		regexp * re=motif->v.er;
		do
		{
			if (p==Alloue)
			{
				Alloue+=(index_t)Extension;
				t->Dim(Alloue);
			}

			t->Get(p).Type=TYPE_CHAINE;
			if (re->Exec(sol))
			{
				t->Get(p++).v.c=chaineval::Creer(sol,re->startp[0]-sol);
			}
			else
			{
				t->Get(p++).v.c=chaineval::Creer(sol);
				break;
			}
			sol=re->endp[0];
		}
		while (1);
	}
	t->Dim(p);
	
	return t;
}

/*AIDE PrendsUnMorceau
Extrait une partie d'une chaîne.
\param chaine chaine Chaîne dont on veut extraire une partie.
\param depart entier Position du premier caractère.
\param fin entier Position du premier caractère à ne pas extraire.
\retour chaine La chaîne qui a été extraite.
\desc
La partie de la chaîne à partir de £depart£ jusqu'à £fin£ exclu
est extraite et renvoyée.

Si £depart£ vaut 0, on extrait à partir du début de la chaîne
(en fait c'est équivalent à avoir £depart£ à 1).

Si £fin£ vaut 0, on extrait jusqu'à la fin de la chaîne.
\exemple
Extrait=PrendsUnMorceau(«Vive Nostradamus !» *(6) *(10))
GOTOPRINT()
GOTO Affiche Nost
*/
const chaineval * biExtraireChaine(const chaineval * chaine, int depart, int fin)
{
	if ((fin<1)||(fin>int(chaine->l+1)))
		fin=int(chaine->l+1);
	if (depart<1)
		depart=1;
	chaineval * c;
	if (depart>=fin)
	{
		c=chaineval::Creer(_T(""),0);
	}
	else
	{
		c=chaineval::Creer(&chaine->c[depart-1],
			fin-depart);
	}
	return c;
}

/*AIDE ORDRALPHABETIX
Compare deux chaînes de caractère.
\param a chaine Une chaîne.
\param b chaine Une autre.
\retour entier -1 si a est avant b dans l'ordre alphabétique, 1 à l'inverse et 0 si
les deux chaînes sont identiques.
\desc
Bon, je dis l'ordre alphabétique, mais en fait ça dépend. C'est en tout cas
vrai tant qu'il n'y a ni majuscules ni accents. Ce qui fait pas mal de si.
*/
int biCompChaines(const chaineval * b, const chaineval *a)
{
	return czcmp(b->c,a->c);
}

/*AIDE Marijuana
Joint les différents pingouins d'un troupeau en une unique chaîne de caractères.
\param tableau tableau Le tableau.
\param separateur chaine Le séparateur utilisé pour joindre les éléments du tableau.
\retour chaine La chaîne produite.
\exemple
a==BOITEAPINGOUINS(«Nostradamus» «est» *(154) «fois plus génial que les autres.» ;)
GOTOPRINTDUTEXTE(Marijuana(&a « »))
*/
const chaineval * biJoindre(ITableauValeurs * _tableau, const chaineval * separateur)
{
	tableauval * tableau=static_cast<tableauval*>(_tableau);
	tableauval * tab=tableau->Copier();
	valeur * z=tab->v;
	
	size_t taille=0;
	
	//Calcul de la taille et transformation
	//des membres de la copie du tableau en des chaînes
	//(super efficace et optimisé, y'a pas à dire)
	for (index_t e=0; e<tab->l; e++)
	{
		if (e)
			taille+=separateur->l;
		z[e].EnChaine();
		taille+=z[e].v.c->l;
	}
	
	//Création de la chaîne
	chaineval * ret=chaineval::Creer(taille);
	carac *pos=ret->c;
	for (index_t e=0; e<tab->l; e++)
	{
		if (e && separateur->l)
		{
			memcpy(pos,separateur->c,separateur->l*sizeof(carac));
			pos+=separateur->l;
		}
		memcpy(pos,z[e].v.c->c,z[e].v.c->l*sizeof(carac));
		pos+=z[e].v.c->l;
	}
	return ret;
}

/*AIDE Rxmplacxr
Remplace un motif par une chaîne.
\param source chaine La chaîne dans laquelle faire un remplacement
\param motif TuerOgre Une expression régulière
\param subsitut chaine La chaîne qu'on met à la place du motif
\retour chaine La chaîne après le remplacement
\desc
Il arrive parfois que dans un moment d'égarement on 
écrive un texte où l'on dit du bien de Java avant
de se rendre compte de sa grave erreur. Cette fonction
permet de remplacer toute occurence de ce mot ignoble
par GOTO++.

En mettant l'attribut g à l'expression régulière, le remplacement
se fait sur toutes les occurences du motif.
\exemple
GOTOPRINTDUTEXTE(Rxmplacxr(«Le Java c'est bien» ^/[Jj]ava/g «GOTO++»))
GOTO Affiche Le GOTO++ c'est bien
*/
const chaineval * biRemplacer(const chaineval * dansquoi, regexp * motif, const chaineval * parquoi)
{
	const carac * src=dansquoi->c;
	chaineval * ret=0;

	size_t retal=0;
	while (*src && motif->Exec(src))
	{
		if (motif->startp[0]==motif->endp[0])
			break;
		chaineval::Construire(ret, src,motif->startp[0]-src,retal);
		chaineval::Construire(ret, parquoi,retal);
		src=motif->endp[0];
		if ((motif->modifieur & regexp::Global)==0)
			break;
	}
	chaineval::Construire(ret, src,czlen(src),retal);
	chaineval::Finaliser(ret);
	return ret;
}


/*AIDE Markov
Remplace une partie du texte par une autre et fait éventuellement un GOTO.
\param cible etiquette Où continuer l'exécution si le motif est trouvé.
\param entree reference Une référence sur la chaîne à modifier.
\param borneinf carac La borne inférieure de l'intervalle des caractères interchangeables.
\param bornesup carac La borne supérieure de l'intervalle des caractères interchangeables.
\param motif chaine Le motif à rechercher.
\param remplacerpar chaine La chaîne par laquelle le motif est remplacé.
\desc
Cette fonction est d'une grande importance dans le traitement des chaînes
en GOTO++ et elle remplace avantageusement les expressions régulières (qui
sont tout de même présentes, pour les conservateurs).

Elle travaille sur £entree£ et recherche dedans une sous-chaîne qui correspond
au motif décrit par £motif£. Cette sous-chaîne est alors remplacée par 
£remplacerpar£ en tenant compte des informations données par £motif£.

La signification de £motif£ est gouvernée par £borneinf£ et £bornesup£. Tout
caractère du motif qui est compris entre £borneinf£ et £bornesup£ est considéré
comme un caractère interchangeable. C'est à dire qu'il signifie en réalité
« n'importe quel caractère entre £borneinf£ et £bornesup£. » Par ailleurs
si un de ces caractères apparaît plusieurs fois dans le motif, il désignera
une répétition du même caractère (par exemple si a et z sont les bornes, le motif
«ccc» désignera la répétition trois fois de suite d'un même caractère situé
dans ces bornes). Les caractères du motif en dehors des bornes se désignent
eux-même.

Si le motif est trouvé, il est remplacé par £remplacerpar£, en tenant compte
de la signification qui a été donnée aux caractères interchangeables (par
exemple si «c» de £motif£ correspondait à un «e» de £entree£, alors si on
trouve «c» dans £remplacerpar£, le caractère «e» sera inséré).
Suite à quoi, l'exécution
continue à £cible£.

Sinon l'exécution continue à la ligne suivante et la chaîne d'entrée
n'est pas modifiée.
\exemple
GOTO Cet exemple inverse les lettres d'une chaîne
GOTO On utilise le caractère * comme guide pour inverser
GOTO la chaîne et le caractère - pour supprimer ensuite
GOTO ces étoiles.
c=«bonjour»
§1
GOTOPRINTDUTEXTE() ;&c
Markov() %1 c *`a' *`z' «**» «-»
Markov() %1 c *`a' *`z' «-a» «a-»
Markov() %1 c *`a' *`z' «-*» «-»
Markov() %2 c *`a' *`z' «-» «»
Markov() %1 c *`a' *`z' «*ab» «b*a»
Markov() %1 c *`a' *`z' «» «*»
§2
GOTOPRINTDUTEXTE() ;&c
GOTO affiche «roujnob»
*/
void biMarkov(IInterpreteur * _interpreteur)
{
	CInterpreteur * itp=static_cast<CInterpreteur*>(_interpreteur);
	//label chaine* borneinf bornesup gauche droite	
	valeur label,chaine,borneinf,bornesup,gauche,droite;
	itp->Pile.Depiler(droite);
	itp->Pile.Depiler(gauche);
	itp->Pile.Depiler(bornesup);
	itp->Pile.Depiler(borneinf);
	itp->Pile.Depiler(chaine);
	itp->Pile.Depiler(label);
	
	if (label.Type!=TYPE_ETIQUETTE)
		throw "le premier paramètre doit être une étiquette";
	if (borneinf.Type!=TYPE_CARAC)
		throw "la borne inférieure doit être un caractère";
	if (bornesup.Type!=TYPE_CARAC)
		throw "la borne supérieure doit être un caractère";
	if (chaine.Type!=TYPE_REFERENCE)
		throw "le deuxième paramètre doit être une référence";
	
	valeur * w=itp->Deref(chaine.v.p);
	w->EnChaine();
	const chaineval * v=w->v.c;
	
	carac binf=(carac)borneinf.v.i;
	carac bsup=(carac)bornesup.v.i;
	
	const carac * motif;
	gauche.EnChaine();
	motif=gauche.v.c->c;
	
#define NMAXRAPPELS 32
	carac RappelId[NMAXRAPPELS];
	carac RappelVal[NMAXRAPPELS];
	
	const carac * q=v->c;
	while (*q)
	{
		const carac * m=motif;
		const carac * p=q;
		int nRappels=0;
		
		while (*m)
		{
			if (*m>=binf && *m<=bsup)
			{
				bool Rappel=false;
				//On regarde si c'est un rappel
				int z;
				for (z=0; z<nRappels; z++)
				{
					if (RappelId[z]==*m)
						Rappel=true;
				}
				
				if (Rappel)
				{
					if (*p==RappelVal[z])
					{
					}
					else
						break;
				}
				else
				{
					if (*p>=binf && *p<=bsup)
					{

						if (nRappels==NMAXRAPPELS)
							throw TXT("nombre maximal de rappels atteint");
						RappelId[nRappels]=*m;
						RappelVal[nRappels++]=*p;
					}
					else
						break;
				}
			}
			else
			if (*p==*m)
			{
			
			}
			else
				break;
			m++;
			p++;
		}
		
		if (!*m)
		{
			//Motif trouvé
			size_t nlong=v->l+droite.v.c->l-gauche.v.c->l;
			chaineval * nv=chaineval::Creer(nlong);
			carac*nouveau=nv->c;
			size_t dep=q-v->c;
			if (dep)
				memcpy(nouveau,v->c,dep*sizeof(carac));
			//On insère le nouveau motif
			const carac *src=droite.v.c->c;
			carac *dest=&nouveau[dep];
			while (*src)
			{
				if (*src>=binf && *src<=bsup)
				{
					int z;
					for (z=0; z<nRappels; z++)
					{
						if (RappelId[z]==*src)
							break;
					}
					if (z==nRappels)
						throw TXT("un caractère de rappel dans la chaîne de droite n'est pas présent dans la chaîne de gauche");
					*(dest++)=RappelVal[z];
				}
				else
					*(dest++)=*src;
				src++;
			}
			
			size_t fin=v->l-dep-gauche.v.c->l;
			if (fin)
				memcpy(dest,&v->c[dep+gauche.v.c->l],fin*sizeof(carac));
			nouveau[nlong]=0;
			*w=nv;
			itp->Goto(label.v.et);
			return;
		}
		q++;
	}	
	itp->Pile.AjouterIndef();
}


}
