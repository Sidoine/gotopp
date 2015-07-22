#ifndef GOTOPPTEST_INTEGRATIONTEST_H
#define GOTOPPTEST_INTEGRATIONTEST_H

#include <gotopp/programme.h>
#include <gotopp/icompilateur.h>
#include <memory>
#include <gotopp/machine.h>

namespace GotoPPTest
{		
	class IntegrationTest
	{
	public:
		void Addition()
		{
			GotoPP::Init();
			GotoPP::Programme * program = new GotoPP::Programme();
			std::unique_ptr<GotoPP::BoutCode> code(program->compilateur->Compiler(L"}*(1) + *(1){ @+"));

			GotoPP::CInterpreteur * interpreteur = new GotoPP::CInterpreteur(program);
			interpreteur->Executer(code->depart);
			GotoPP::valeur val;
			interpreteur->Depiler2(val);
			assert(val.Entier() == 2);
		}

		void Run()
		{
			Addition();
		}
	};
}

#endif
