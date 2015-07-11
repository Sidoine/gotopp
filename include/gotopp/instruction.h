/*


	Instruction : un bout de code du programme créé par
	un appel à Compilateur::Compiler
*/

#ifndef _INSTRUCTION_H
#define _INSTRUCTION_H

namespace GotoPP
{
	//Utilisé à l'exécution
	class CInstruction
	{
	public:
		//Y'a que cette variable qui est utilisée à l'exécution :
		code * i;//Position actuelle
		
		CInstruction()
		{
			i=NULL;
		}
		void Goto(code * j)
		{
			i=j;
		}
		void GotoRel(int j)
		{
			i+=j;
		}
		void Lire(valeur & ret)
		{
			ret.Type=LireType();
			switch(ret.Type)
			{
			case TYPE_CHANCES:
				ret.v.f=LireChances();
				break;
			case TYPE_CODE:
				ret.v.i=LireCode();
				break;
			case TYPE_TYPE:
				ret.v.i=LireType();
				break;
			case TYPE_CARAC:
				ret.v.i=LireCarac();
				break;
			case TYPE_CHAINE:
				ret.v.c=LireChaine();
				break;
			case TYPE_OBJETC:
				ret.v.o=LireObjet();
				break;
			default:
				ret.v.i=LireEntier();
			}
		}
		IObjet * LireObjet()
		{
			IObjet * ret=*(IObjet**)i;
			i+=sizeof(IObjet*);
			return ret;
		}

		const chaineval * LireChaine()
		{
			const chaineval * ret=(const chaineval*)i;
			i+=sizeof(size_t)+(ret->l+1)*sizeof(carac);
			return ret;
		}

		carac LireCarac()
		{
			carac ret=*((carac*)i);
			i+=sizeof(carac);
			return ret;
		}
		int LireEntier()
		{
			int ret=*((int*)i);
			i+=sizeof(int);
			return ret;
		}
		type LireType()
		{
			type ret=*((type*)i);
			i+=sizeof(type);
			return ret;
		}
		float LireChances()
		{
			float ret=*((float*)i);
			i+=sizeof(float);
			return ret;
		}
		code * LireEtiquette()
		{
			code * ret=*((code**)i);
			i+=sizeof(code*);
			return ret;
		}
		int LireVariable()
		{
			return LireEntier();
		}
		int LireFonction()
		{
			return LireEntier();
		}
		code LireCode()
		{
			return *(i++);
		}
		Symbole * LireSymbole()
		{
			Symbole * ret=*((Symbole**)i);
			i+=sizeof(Symbole*);
			return ret;
		}
		voidf LireFonctionC()
		{
			voidf ret=*((voidf*)i);
			i+=sizeof(voidf);
			return ret;
		}
	};

	//En sortie d'un appel au compilateur
	class BoutCode: public CInstruction
	{
	public:
		code * depart;
		code * fin;
	
		BoutCode():CInstruction()
		{
			depart=fin=NULL;
		}
		bool Fin()
		{
			return (i==fin);
		}
	};
}

#endif
