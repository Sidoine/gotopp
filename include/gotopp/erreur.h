#ifndef _ERREUR_H
#define _ERREUR_H

namespace GotoPP
{
	class CErreur
	{
		carac* texte;
	public:
		CErreur(carac *Format,...);
		~CErreur();
		carac *Texte()
		{
			if (texte)
				return &texte[1];
			else
				return _T("");
		}
		carac & Compteur()
		{
			return texte[0];
		}
		CErreur(const CErreur& e)
		{
			texte=e.texte;
			Compteur()++;
		}
		void operator=(CErreur &e)
		{
			texte=e.texte;
			Compteur()++;
		}
	};
}

#endif
