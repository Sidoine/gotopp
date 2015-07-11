#include <gotopp/base.h>
#include <gotopp/pile.h>

int GotoPP::CPile::nParams()
{
	int p=int(Pos)-1;
	while (p>=0 && Pile[p].Type!=TYPE_DEBUTPARAM)
		p--;
	if (p==-1)
		throw TXT("il manque la marque de début de paramètres");
	return Pos-p-1;
}

