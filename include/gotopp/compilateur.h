#ifndef _COMPILATEUR_H
#define _COMPILATEUR_H

namespace GotoPP
{
	class BoutCode;
	class CErreur;

	enum AccesPropriete
	{
		PROP_PUBLIQUE,//La propriété est toujours accessible
		PROP_PROTEGEE,//La propriété n'est accessible que depuis les méthodes
		PROP_PRIVEE, //La propriété devient cachée quand on dérive la classe
		PROP_CACHEE //La propriété est inaccessible
	};

	class Compilateur;
	class Programme;

	class ICompilateur
	{
	public:
		virtual BoutCode* Charger(const carac * fichier)=0;//Charge et compile ce fichier
		virtual BoutCode* Compiler(const carac *CodeSource)=0;//Compile ce code source
		
		virtual void AfficherErreur(CErreur & e)=0;
		virtual void AfficherErreur(const carac *c)=0;

		static ICompilateur * Factory(Programme * programme);
	};
}

#endif
