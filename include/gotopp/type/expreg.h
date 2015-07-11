/*
 * Definitions etc. for regexp(3) routines.
 *
 * Caveat:  this is V8 regexp(3) [actually, a reimplementation thereof],
 * not the System V one.
 * 
 * See expreg.cpp for legal information.
 */
 
#ifndef _EXPREG_H
#define _EXPREG_H
 
namespace GotoPP
{
	class chaineval;

#	define NSUBEXP  10
		
	class regexp:public IObjet
	{
		friend class RegExpComp;
		friend class RegExpExec;

	public:
		int Exec(const GotoPP::carac * string);
		static regexp * Creer(const carac *a, const carac *modif);
		regexp * Copier(){return this;}

		const chaineval * GetValeur(int i);
		enum Modifieur
		{
			Aucun=0,
			Global=1,
			PasCasse=2
		};
		size_t Longueur; //Taille du programme
		const GotoPP::chaineval * Chaine; //A quelle chaîne appartiennent les machins suivants :
		const GotoPP::carac* startp[NSUBEXP];	//Ce qui a été trouvé
		const GotoPP::carac* endp[NSUBEXP];	
		GotoPP::carac regstart;		/* Internal use only. */
		GotoPP::carac reganch;		/* Internal use only. */
		int regmust;		//Début d'une chaîne qui doit être présente
		int regmlen;		/* Internal use only. */
		Modifieur modifieur;
	private:
		GotoPP::carac program[1];	/* Unwarranted chumminess with compiler. */
		
	};
}
#endif
