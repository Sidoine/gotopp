#include <gotopp/base.h>
#include <gotopp/global.h>

namespace GotoPP
{
	bool CType::EstFonction()
	{
		return (l>=1 && t[l-1]==symboleFonctionGPP);
	}
}
