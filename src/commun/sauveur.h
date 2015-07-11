#ifndef _SAUVEUR_H
#define _SAUVEUR_H

namespace GotoPP
{
	class regexp;
}

namespace GotoPP
{
	struct bebe;
	class SPointeur
	{
	public:
		type Type;
		union
		{
			chaineval * c;
			tableauval * t;
			hachageval * h;
			GotoPP::regexp * er;
			valeur * b;
			void * p;
		};
		union
		{
			chaineval * c;
			tableauval * t;
			hachageval * h;
			GotoPP::regexp * er;
			valeur * b;
			void * p;
		} ici;
		SPointeur()
		{
			ici.b=0;
			Type=0;
		}
		void Sauver(FILE *F);
		void Charger(FILE *F);
	};

	class CPointeur:public Tableau<SPointeur*>
	{
	public:
		bool Nouveau(void * Pointeur, type Type)
		{
			for (index_t p=0; p<l; p++)
			{
				if (t[p]->p==Pointeur)
					return false;
			}
			Etendre(new SPointeur());
			Dernier()->Type=Type;
			Dernier()->p=Pointeur;
			return true;
		}
		bool Nouveau(void * &Pointeur, type Type, size_t Taille)
		{
			for (index_t p=0; p<l; p++)
			{
				if (t[p]->p==Pointeur)
				{
					Pointeur=t[p]->ici.p;
					return false;
				}
			}
			Etendre(new SPointeur());
			Dernier()->Type=Type;
			Dernier()->p=Pointeur;
			Dernier()->ici.p=Pointeur=malloc(Taille);
			return true;
		}
	};
	GPP_GLOBAL CPointeur Pointeur;
	void InitSauveur();
}
#endif
