#include <gotopp/base.h>
#include "instrconstr.h"
#include <gotopp/instruction.h>

void GotoPP::InstrConstr::Reinit()
{
	if (c)
		free(c);
	c=NULL;
	t=a=0;
	i=NULL;
}

GotoPP::BoutCode * GotoPP::InstrConstr::CreerBoutCode()
{
	BoutCode *ret=new BoutCode;
	ret->i=c;
	ret->depart=c;
	ret->fin=c+t;
	c=NULL;
	return ret;
}
