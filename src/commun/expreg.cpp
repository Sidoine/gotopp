/*
 * regcomp and regexec -- regsub and regerror are elsewhere
 * @(#)regexp.c	1.3 of 18 April 87
 *
 *	Copyright (c) 1986 by University of Toronto.
 *	Written by Henry Spencer.  Not derived from licensed software.
 *
 *	Permission is granted to anyone to use this software for any
 *	purpose on any computer system, and to redistribute it freely,
 *	subject to the following restrictions:
 *
 *	1. The author is not responsible for the consequences of use of
 *		this software, no matter how awful, even if they arise
 *		from defects in it.
 *
 *	2. The origin of this software must not be misrepresented, either
 *		by explicit claim or by omission.
 *
 *	3. Altered versions must be plainly marked as such, and must not
 *		be misrepresented as being the original software.
 *
 * Beware that some of this code is subtly aware of the way operator
 * precedence is structured in regular expressions.  Serious changes in
 * regular-expression syntax might require a total rethink.
 *
 * Modified by Sidoine de Wispelaere : 
 * - standard C++ compliant 
 * - somme extra things, like \1, \d, \W...
 * TODO: 
 *		- remove macros
		- a lot of things!
 */

#include <gotopp/base.h>
#include <gotopp/erreur.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gotopp/type/expreg.h>
#include <assert.h>
#include "regmagic.h"


using namespace GotoPP;

typedef unsigned char UCHAR;
/*
 * The "internal use only" fields in regexp.h are present to pass info from
 * compile to execute that permits the execute phase to run lots faster on
 * simple cases.  They are:
 *
 * regstart	carac that must begin a match; '\0' if none obvious
 * reganch	is the match anchored (at beginning-of-line only)?
 * regmust	string (pointer into program) that match must include, or NULL
 * regmlen	length of regmust string
 *
 * Regstart and reganch permit very fast decisions on suitable starting points
 * for a match, cutting down the work a lot.  Regmust permits fast rejection
 * of lines that cannot possibly match.  The regmust tests are costly enough
 * that regcomp() supplies a regmust only if the r.e. contains something
 * potentially expensive (at present, the only such thing detected is * or +
 * at the start of the r.e., which can involve a lot of backup).  Regmlen is
 * supplied because the test in regexec() needs it and regcomp() is computing
 * it anyway.
 */

/*
 * Structure for regexp "program".  This is essentially a linear encoding
 * of a nondeterministic finite-state machine (aka syntax caracts or
 * "railroad normal form" in parsing technology).  Each node is an opcode
 * plus a "next" pointer, possibly plus an operand.  "Next" pointers of
 * all nodes except BRANCH implement concatenation; a "next" pointer with
 * a BRANCH on both ends of it is connecting two alternatives.  (Here we
 * have one of the subtle syntax dependencies:  an individual BRANCH (as
 * opposed to a collection of them) is never concatenated with anything
 * because of operator precedence.)  The operand of some types of node is
 * a literal string; for others, it is a node leading into a sub-FSM.  In
 * particular, the operand of a BRANCH node is the first node of the branch.
 * (NB this is *not* a tree structure:  the tail of the branch connects
 * to the thing following the set of BRANCHes.)  The opcodes are:
 */

/* definition		number	opnd?	meaning */
#define	END		0	/* no	End of program. */
#define	BOL		1	/* no	Match "" at beginning of line. */
#define	EOL		2	/* no	Match "" at end of line. */
#define	ANY		3	/* no	Match any one caracacter. */
#define	ANYOF		4	/* str	Match any caracacter in this string. */
#define	ANYBUT		5	/* str	Match any caracacter not in this string. */
#define	BRANCH		6	/* node	Match this alternative, or the next... */
#define	BACK		7	/* no	Match "", "next" ptr points backward. */
#define	EXACTLY		8	/* str	Match this string. */
#define	NOTHING		9	/* no	Match empty string. */
#define	STAR		10	/* node	Match this (simple) thing 0 or more times. */
#define	PLUS		11	/* node	Match this (simple) thing 1 or more times. */
#define ANYOFINT 	12	// str  Paires d'intervalles
#define ANYBUTINT	13	// str  Paires d'intervalles à exclure
#define	OPEN		20	/* no	Mark this point in input as start of #n. */
				/*	OPEN+1 is number 1, etc. */
#define	CLOSE		30	/* no	Analogous to OPEN. */
#define RAPPEL  	40	// non	Rappelle une valeur stockée entre parenthèses
/*
 * Opcode notes:
 *
 * BRANCH	The set of branches constituting a single choice are hooked
 *		together with their "next" pointers, since precedence prevents
 *		anything being concatenated to any individual branch.  The
 *		"next" pointer of the last BRANCH in a choice points to the
 *		thing following the whole choice.  This is also where the
 *		final "next" pointer of each individual branch points; each
 *		branch starts with the operand node of a BRANCH node.
 *
 * BACK		Normal "next" pointers all implicitly point forward; BACK
 *		exists to make loop structures possible.
 *
 * STAR,PLUS	'?', and complex '*' and '+', are implemented as circular
 *		BRANCH structures using BACK.  Simple cases (one caracacter
 *		per match) are implemented with STAR and PLUS for speed
 *		and to minimize recursive plunges.
 *
 * OPEN,CLOSE	...are numbered at compile time.
 */

/*
 * A node is one carac of opcode followed by two caracs of "next" pointer.
 * "Next" pointers are stored as two 8-bit pieces, high order first.  The
 * value is a positive offset from the opcode of the node containing it.
 * An operand, if any, simply follows the node.  (Note that much of the
 * code generation knows about this implicit relationship.)
 *
 * Using two bytes for the "next" pointer is vast overkill for most things,
 * but allows patterns to get big without disasters.
 */
#define	OP(p)	(*(p))
#define	NEXT(p)	(((*((p)+1)&0377)<<8) + (*((p)+2)&0377))
#define	OPERAND(p)	((p) + 3)

/*
 * See regmagic.h for one further detail of program structure.
 */


/*
 * Utility definitions.
 */
#ifndef CHARBITS
#define	UCHARAT(p)	((int)*(unsigned char *)(p))
#else
#define	UCHARAT(p)	((int)*(p)&CHARBITS)
#endif

#define	FAIL(m)	{throw m;} //{ regerror(m); return(NULL); }
#define	ISMULT(c)	((c) == '*' || (c) == '+' || (c) == '?')
#define	META	_T("^$.[()|?+*\\")

/*
 * Flags to be passed up and down.
 */
#define	HASWIDTH	01	/* Known never to match null string. */
#define	SIMPLE		02	/* Simple enough to be STAR/PLUS operand. */
#define	SPSTART		04	/* Starts with * or +. */
#define	WORST		0	/* Worst case. */

namespace GotoPP
{
	class RegExpComp
	{
		const carac *regparse;		/* Input-scan pointer. */
		int regnpar;		/* () count. */
		carac regdummy;
		carac *regcode;		/* Code-emit pointer; &regdummy = don't. */
		long regsize;		/* Code size. */

		/*
		 * Forward declarations for regcomp()'s friends.
		 */
		carac *reg(int paren, int*flagp);
		carac *regbranch(int * flagp);
		carac *regpiece(int * flagp);
		carac *regatom(int * flagp);
		carac *regnode(carac op);
		carac *regnext(const carac * p);
		void regc(carac b);
		void regs(carac * b);
		void reginsert(carac op,carac *opnd);
		void regtail(carac *p, carac * val);
		void regoptail(carac *p, carac * val);
		#ifdef STRCSPN
		int strcspn();
		#endif
	public:

		regexp * regcomp(const GotoPP::carac *exp);
	};
	
	class RegExpExec
	{
		const carac *reginput;		/* String-input pointer. */
		const carac *regbol;		/* Beginning of input, for ^ check. */
		const carac **regstartp;	/* Pointer to startp array. */
		const carac **regendp;		/* Ditto for endp. */

		/*
		 * Forwards.
		 */
		int regtry(regexp *prog, const carac *string);
		int regmatch(const carac *prog);
		int regrepeat(const carac *p);
		const carac * strintervalle(const carac * intervalles, carac chercher);

		carac * regnext(const carac *p);


		/*#ifdef _DEBUG
		int regnarrate = 0;
		void regdump(regexp * rgexp);
		carac * regprop(const carac *op);
		#endif*/

	public:
		int regexec(regexp *prog, const GotoPP::carac * string);
	};
}

//Implémentation de regexp
namespace GotoPP
{
	int regexp::Exec(const carac * string)
	{
		RegExpExec exec;
		return exec.regexec(this,string);
	}

	regexp * regexp::Creer(const carac *a, const carac *modif)
	{
		RegExpComp comp;
		regexp * v=comp.regcomp(a);
		if (modif)
		{
			while (*modif)
			{
				switch (*modif)
				{
				/*case 'i':
					v->modifieur|=GotoPP::PasCasse;
					break;*/
				case 'g':
					v->modifieur=Modifieur(v->modifieur|Global);
					break;
				default:
					throw GotoPP::CErreur(TXT("modificateur %c inconnu pour l'expression régulière"),
						*modif);
				}
				modif++;
			}
		}
		return v;
	}

	const chaineval * regexp::GetValeur(int numero)
	{
		if (numero<NSUBEXP)
			return chaineval::Creer(startp[numero],endp[numero]-startp[numero]);
		else
			return chaineval::Creer(_T(""));
	}
}

/*
 - regcomp - compile a regular expression into internal code
 *
 * We can't allocate space until we know how big the compiled form will be,
 * but we can't compile it (and thus know how big it is) until we've got a
 * place to put the code.  So we cheat:  we compile it twice, once with code
 * generation turned off and size counting turned on, and once "for real".
 * This also means that we don't allocate space until we are sure that the
 * thing really will compile successfully, and we never have to move the
 * code and thus invalidate pointers into it.  (Note that it has to be in
 * one piece because free() must be able to free it all.)
 *
 * Beware that the optimization-preparation code in here knows about some
 * of the structure of the compiled regexp.
 */
GotoPP::regexp * RegExpComp::regcomp(const carac *exp)
{
	regexp *r;
	const carac *scan;
	const carac *longest;
	int len;
	int flags;

	ASSERT(exp!=NULL);

	/* First pass: determine size, legality. */
	regparse = exp;
	regnpar = 1;
	regsize = 0L;
	regcode = &regdummy;
	regc(MAGIC);
	if (reg(0, &flags) == NULL)
		return(NULL);

	/* Small enough for pointer-storage convention? */
	if (regsize >= 32767L)		/* Probably could be 65535L. */
		FAIL("regexp too big");

	/* Allocate space. */
	r = (regexp *)GC_MALLOC(sizeof(regexp) + (unsigned)regsize*sizeof(carac));
	if (r == NULL)
		FAIL("out of space");

	/* Second pass: emit code. */
	regparse = exp;
	regnpar = 1;
	regcode = r->program;
	regc(MAGIC);
	if (reg(0, &flags) == NULL)
		return(NULL);

	/* Dig out information for optimizations. */
	r->Longueur=regsize+sizeof(regexp);
	r->regstart = '\0';	/* Worst-case defaults. */
	r->reganch = 0;
	r->regmust = -1;
	r->regmlen = 0;
	r->Chaine=NULL;
	r->modifieur=regexp::Aucun;
	scan = r->program+1;			/* First BRANCH. */
	if (OP(regnext(scan)) == END)
	{		/* Only one top-level choice. */
		scan = OPERAND(scan);

		/* Starting-point info. */
		if (OP(scan) == EXACTLY)
			r->regstart = *OPERAND(scan);
		else if (OP(scan) == BOL)
			r->reganch++;

		/*
		 * If there's something expensive in the r.e., find the
		 * longest literal string that must appear and make it the
		 * regmust.  Resolve ties in favor of later strings, since
		 * the regstart check works with the beginning of the r.e.
		 * and avoiding duplication strengthens checking.  Not a
		 * strong reason, but sufficient in the absence of others.
		 */
		if (flags&SPSTART)
		{
			longest = NULL;
			len = 0;
			for (; scan != NULL; scan = regnext(scan))
			{
				if (OP(scan) == EXACTLY && (int)czlen(OPERAND(scan)) >= len)
				{
					longest = OPERAND(scan);
					len = (int)czlen(OPERAND(scan));
				}
			}
			r->regmust = int(longest-r->program);
			r->regmlen = len;
		}
	}

	return(r);
}

/*
 - reg - regular expression, i.e. main body or parenthesized thing
 *
 * Caller must absorb opening parenthesis.
 *
 * Combining parenthesis handling with the base level of regular expression
 * is a trifle forced, but the need to tie the tails of the branches to what
 * follows makes it hard to avoid.
 */
carac * RegExpComp::reg(int paren, int * flagp)
{
	carac *ret;
	carac *br;
	carac *ender;
	int parno;
	int flags;

	*flagp = HASWIDTH;	/* Tentatively. */

	/* Make an OPEN node, if parenthesized. */
	if (paren)
	{
		if (regnpar >= NSUBEXP)
			FAIL("too many ()");
		parno = regnpar;
		regnpar++;
		ret = regnode(OPEN+parno);
	} else
		ret = NULL;

	/* Pick up the branches, linking them together. */
	br = regbranch(&flags);
	if (br == NULL)
		return(NULL);
	if (ret != NULL)
		regtail(ret, br);	/* OPEN -> first. */
	else
		ret = br;
	if (!(flags&HASWIDTH))
		*flagp &= ~HASWIDTH;
	*flagp |= flags&SPSTART;
	while (*regparse == '|')
	{
		regparse++;
		br = regbranch(&flags);
		if (br == NULL)
			return(NULL);
		regtail(ret, br);	/* BRANCH -> BRANCH. */
		if (!(flags&HASWIDTH))
			*flagp &= ~HASWIDTH;
		*flagp |= flags&SPSTART;
	}

	/* Make a closing node, and hook it on the end. */
	ender = regnode((paren) ? CLOSE+parno : END);	
	regtail(ret, ender);

	/* Hook the tails of the branches to the closing node. */
	for (br = ret; br != NULL; br = (carac*)regnext(br))
		regoptail(br, ender);

	/* Check for proper termination. */
	if (paren && *regparse++ != ')')
	{
		FAIL("unmatched ()");
	}
	else if (!paren && *regparse != '\0') 
	{
		if (*regparse == ')') 
		{
			FAIL("unmatched ()");
		}
		else
			FAIL("junk on end");	/* "Can't happen". */
		/* NOTREACHED */
	}

	return(ret);
}

/*
 - regbranch - one alternative of an | operator
 *
 * Implements the concatenation operator.
 */
carac * RegExpComp::regbranch(int * flagp)
{
	carac *ret;
	carac *chain;
	carac *latest;
	int flags;

	*flagp = WORST;		/* Tentatively. */

	ret = regnode(BRANCH);
	chain = NULL;
	while (*regparse != '\0' && *regparse != '|' && *regparse != ')')
	{
		latest = regpiece(&flags);
		if (latest == NULL)
			return(NULL);
		*flagp |= flags&HASWIDTH;
		if (chain == NULL)	/* First piece. */
			*flagp |= flags&SPSTART;
		else
			regtail(chain, latest);
		chain = latest;
	}
	if (chain == NULL)	/* Loop ran zero times. */
		(void) regnode(NOTHING);

	return(ret);
}

/*
 - regpiece - something followed by possible [*+?]
 *
 * Note that the branching code sequences used for ? and the general cases
 * of * and + are somewhat optimized:  they use the same NOTHING node as
 * both the endmarker for their branch list and the body of the last branch.
 * It might seem that this node could be dispensed with entirely, but the
 * endmarker role is not redundant.
 */
carac * RegExpComp::regpiece(int *flagp)
{
	carac *ret;
	carac op;
	carac *next;
	int flags;

	ret = regatom(&flags);
	if (ret == NULL)
		return(NULL);

	op = *regparse;
	if (!ISMULT(op)) {
		*flagp = flags;
		return(ret);
	}

	if (!(flags&HASWIDTH) && op != '?')
		FAIL("*+ operand could be empty");
	*flagp = (op != '+') ? (WORST|SPSTART) : (WORST|HASWIDTH);

	if (op == '*' && (flags&SIMPLE))
		reginsert(STAR, ret);
	else if (op == '*')
	{
		/* Emit x* as (x&|), where & means "self". */
		reginsert(BRANCH, ret);			/* Either x */
		regoptail(ret, regnode(BACK));		/* and loop */
		regoptail(ret, ret);			/* back */
		regtail(ret, regnode(BRANCH));		/* or */
		regtail(ret, regnode(NOTHING));		/* null. */
	}
	else if (op == '+' && (flags&SIMPLE))
		reginsert(PLUS, ret);
	else if (op == '+')
	{
		/* Emit x+ as x(&|), where & means "self". */
		next = regnode(BRANCH);			/* Either */
		regtail(ret, next);
		regtail(regnode(BACK), ret);		/* loop back */
		regtail(next, regnode(BRANCH));		/* or */
		regtail(ret, regnode(NOTHING));		/* null. */
	}
	else if (op == '?')
	{
		/* Emit x? as (x|) */
		reginsert(BRANCH, ret);			/* Either x */
		regtail(ret, regnode(BRANCH));		/* or */
		next = regnode(NOTHING);		/* null. */
		regtail(ret, next);
		regoptail(ret, next);
	}
	regparse++;
	if (ISMULT(*regparse))
		FAIL("nested *?+");

	return(ret);
}

/*
 - regatom - the lowest level
 *
 * Optimization:  gobbles an entire sequence of ordinary caracacters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Backslashed caracacters are exceptions, each becoming a
 * separate node; the code is simpler that way and it's not worth fixing.
 */
carac * RegExpComp::regatom(int *flagp)
{
	carac *ret;
	int flags;

	*flagp = WORST;		/* Tentatively. */

	switch (*regparse++)
	{
	case '^':
		ret = regnode(BOL);
		break;
	case '$':
		ret = regnode(EOL);
		break;
	case '.':
		ret = regnode(ANY);
		*flagp |= HASWIDTH|SIMPLE;
		break;
	case '[': {
			int classe;
			int classend;

			if (*regparse == '^')
			{	/* Complement of range. */
				ret = regnode(ANYBUT);
				regparse++;
			}
			else
				ret = regnode(ANYOF);
			if (*regparse == ']' || *regparse == '-')
				regc(*regparse++);
			while (*regparse != '\0' && *regparse != ']')
			{
				if (*regparse == '-')
				{
					regparse++;
					if (*regparse == ']' || *regparse == '\0')
						regc('-');
					else
					{
						classe = UCHARAT(regparse-2)+1;
						classend = UCHARAT(regparse);
						if (classe > classend+1)
							FAIL("invalid [] range");
						for (; classe <= classend; classe++)
							regc(classe);
						regparse++;
					}
				}
				else
					regc(*regparse++);
			}
			regc('\0');
			if (*regparse != ']')
				FAIL("unmatched []");
			regparse++;
			*flagp |= HASWIDTH|SIMPLE;
		}
		break;
	case '(':
		ret = reg(1, &flags);
		if (ret == NULL)
			return(NULL);
		*flagp |= flags&(HASWIDTH|SPSTART);
		break;
	case '\0':
	case '|':
	case ')':
		FAIL("internal urp");	/* Supposed to be caught earlier. */
		break;
	case '?':
	case '+':
	case '*':
		FAIL("?+* follows nothing");
		break;
	case '\\':
		switch (*regparse)
		{
		case 0:
			FAIL("un \\ termine l'expression régulière");
			break;
			
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			ret=regnode(RAPPEL+(*regparse-'0'));
			regparse++;
			*flagp |= HASWIDTH|SIMPLE;
			break;
			
		case 'd':
			regparse++;
			ret = regnode(ANYOFINT);
			regs(_T("09"));
			regc(0);
			*flagp |= HASWIDTH|SIMPLE;
			break;
		case 'D':
			regparse++;
			ret = regnode(ANYBUTINT);
			regs(_T("09"));
			regc(0);
			*flagp |= HASWIDTH|SIMPLE;
			break;
		case 's':
			regparse++;
			ret=regnode(ANYOF);
			regs(_T("\t "));
			regc(0);
			*flagp |= HASWIDTH|SIMPLE;
			break;
		case 'S':
			regparse++;
			ret=regnode(ANYBUT);
			regs(_T("\t "));
			regc(0);
			*flagp |= HASWIDTH|SIMPLE;
			break;
		case 'w':
			regparse++;
			ret = regnode(ANYOFINT);
			regs(_T("09azAZ__"));
			regc(0);
			*flagp |= HASWIDTH|SIMPLE;
			break;	
		case 'W':
			regparse++;
			ret = regnode(ANYBUTINT);
			regs(_T("09azAZ__"));
			regc(0);
			*flagp |= HASWIDTH|SIMPLE;
			break;		
		
		case 'r':
			regparse++;
			ret = regnode(EXACTLY);
			regc('\r');
			regc('\0');
			*flagp |= HASWIDTH|SIMPLE;
			break;	

		case 'n':
			regparse++;
			ret = regnode(EXACTLY);
			regc('\n');
			regc('\0');
			*flagp |= HASWIDTH|SIMPLE;
			break;	
			
		case 't':
			regparse++;
			ret = regnode(EXACTLY);
			regc('\t');
			regc('\0');
			*flagp |= HASWIDTH|SIMPLE;
			break;	
									
		default:
			ret = regnode(EXACTLY);
			regc(*regparse++);
			regc('\0');
			*flagp |= HASWIDTH|SIMPLE;
			break;
		}
		break;
	default:
		{
			int len;
			carac ender;

			regparse--;
			len = (int)czscspn(regparse, META);
			if (len <= 0)
				FAIL("internal disaster");
			ender = *(regparse+len);
			if (len > 1 && ISMULT(ender))
				len--;		/* Back off clear of ?+* operand. */
			*flagp |= HASWIDTH;
			if (len == 1)
				*flagp |= SIMPLE;
			ret = regnode(EXACTLY);
			while (len > 0)
			{
				regc(*regparse++);
				len--;
			}
			regc('\0');
		}
		break;
	}

	return(ret);
}

/*
 - regnode - emit a node
 renvoie Location. */
carac * RegExpComp::regnode(carac op)
{
	carac *ret;
	carac *ptr;

	ret = regcode;
	if (ret == &regdummy)
	{
		regsize += 3;
		return(ret);
	}

	ptr = ret;
	*ptr++ = op;
	*ptr++ = '\0';		/* Null "next" pointer. */
	*ptr++ = '\0';
	regcode = ptr;

	return(ret);
}

/*
 - regc - emit (if appropriate) a byte of code
 */
void RegExpComp::regc(carac b)
{
	if (regcode != &regdummy)
		*regcode++ = b;
	else
		regsize++;
}

void RegExpComp::regs(carac * b)
{
	while (*b)
	{
		if (regcode != &regdummy)
			*regcode++ = *b;
		else
			regsize++;
		b++;
	}
}

/*
 - reginsert - insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
void RegExpComp::reginsert(carac op, carac *opnd)
{
	carac *src;
	carac *dst;
	carac *place;

	if (regcode == &regdummy)
	{
		regsize += 3;
		return;
	}

	src = regcode;
	regcode += 3;
	dst = regcode;
	while (src > opnd)
		*--dst = *--src;

	place = opnd;		/* Op node, where operand used to be. */
	*place++ = op;
	*place++ = '\0';
	*place++ = '\0';
}

/*
 - regtail - set the next-pointer at the end of a node chain
 */
void RegExpComp::regtail(carac *p, carac *val)
{
	carac *scan;
	carac *temp;
	int offset;

	if (p == &regdummy)
		return;

	/* Find last node. */
	scan = p;
	while (1)
	{
		temp = regnext(scan);
		if (temp == NULL)
			break;
		scan = temp;
	}

	if (OP(scan) == BACK)
		offset = (int)(scan - val);
	else
		offset = (int)(val - scan);
	*(scan+1) = (offset>>8)&0377;
	*(scan+2) = offset&0377;
}

/*
 - regoptail - regtail on operand of first argument; nop if operandless
 */
void RegExpComp::regoptail(carac *p, carac *val)
{
	/* "Operandless" and "op != BRANCH" are synonymous in practice. */
	if (p == NULL || p == &regdummy || OP(p) != BRANCH)
		return;
	regtail(OPERAND(p), val);
}

/*
 * regexec and friends
 */

/*
 - regexec - match a regexp against a string
 */
int RegExpExec::regexec(regexp *prog, const carac *string)
{
	//regdump(prog);
	
	const carac *s;
	
	/* Be paranoid... */
	ASSERT(prog != NULL && string != NULL);

	/* Check validity of program. */
	ASSERT(UCHARAT(prog->program) == MAGIC);

	/* If there is a "must appear" string, look for it. */
	if (prog->regmust != -1)
	{
		carac * regmust=&prog->program[prog->regmust];
		s = string;
		while ((s = czschr(s, regmust[0])) != NULL) {
			if (czncmp(s, regmust, prog->regmlen) == 0)
				break;	/* Found it. */
			s++;
		}
		if (s == NULL)	/* Not present. */
			return(0);
	}

	/* Mark beginning of line for ^ . */
	regbol = string;

	/* Simplest case:  anchored match need be tried only once. */
	if (prog->reganch)
		return(regtry(prog, string));

	/* Messy cases:  unanchored match. */
	s = string;
	if (prog->regstart != '\0')
		/* We know what carac it must start with. */
		while ((s = czschr(s, prog->regstart)) != NULL) {
			if (regtry(prog, s))
				return(1);
			s++;
		}
	else
		/* We don't -- general case. */
		do
		{
			if (regtry(prog, s))
				return(1);
		}
		while (*s++ != '\0');

	/* Failure. */
	return(0);
}

/*
 - regtry - try match at specific point
 			return  0 failure, 1 success */
int RegExpExec::regtry(regexp *prog, const carac *string)
{
	int i;
	const carac **sp;
	const carac **ep;

	reginput = string;
	regstartp = prog->startp;
	regendp = prog->endp;

	sp = prog->startp;
	ep = prog->endp;
	for (i = NSUBEXP; i > 0; i--)
	{
		*sp++ = NULL;
		*ep++ = NULL;
	}
	if (regmatch(prog->program + 1))
	{
		prog->startp[0] = string;
		prog->endp[0] = reginput;
		return(1);
	}
	else
		return(0);
}

const carac * RegExpExec::strintervalle(const carac * intervalles, carac chercher)
{
	while (*intervalles)
	{
		if (UCHAR(chercher)>=UCHAR(*intervalles) && UCHAR(chercher)<=UCHAR(intervalles[1]))
			return intervalles;
		intervalles+=2;
	}
	return NULL;
}

/*
 - regmatch - main matching routine
 *
 * Conceptually the strategy is simple:  check to see whether the current
 * node matches, call self recursively to see whether the rest matches,
 * and then act accordingly.  In practice we make some effort to avoid
 * recursion, in particular by going through "ordinary" nodes (that don't
 * need to know whether the rest of the match failed) by a loop instead of
 * by recursion.
 		return 0 failure, 1 success
 */
 

int RegExpExec::regmatch(const carac *prog)
{
	const carac *scan;	/* Current node. */
	const carac *next;		/* Next node. */

	scan = prog;
#ifdef DEBUG
	if (scan != NULL && regnarrate)
		fprintf(stderr, "%s(\n", regprop(scan));
#endif
	while (scan != NULL)
	{
#ifdef DEBUG
		if (regnarrate)
			fprintf(stderr, "%s...\n", regprop(scan));
#endif
		next = regnext(scan);

		switch (OP(scan))
		{
		case BOL:
			if (reginput != regbol)
				return(0);
			break;
		case EOL:
			if (*reginput != '\0')
				return(0);
			break;
		case ANY:
			if (*reginput == '\0')
				return(0);
			reginput++;
			break;
		case EXACTLY: {
				register int len;
				const carac *opnd;

				opnd = OPERAND(scan);
				/* Inline the first caracacter, for speed. */
				if (*opnd != *reginput)
					return(0);
				len = (int) czlen(opnd);
				if (len > 1 && czncmp(opnd, reginput, len) != 0)
					return(0);
				reginput += len;
			}
			break;
		case ANYOF:
			if (*reginput == '\0' || czschr(OPERAND(scan), *reginput) == NULL)
				return(0);
			reginput++;
			break;
		case ANYBUT:
			if (*reginput == '\0' || czschr(OPERAND(scan), *reginput) != NULL)
				return(0);
			reginput++;
			break;
		case ANYOFINT:
			if (*reginput == '\0' || strintervalle(OPERAND(scan), *reginput) == NULL)
				return(0);
			reginput++;
			break;
		case ANYBUTINT:
			if (*reginput == '\0' || strintervalle(OPERAND(scan), *reginput) != NULL)
				return(0);
			reginput++;
			break;
			
		case NOTHING:
			break;
		case BACK:
			break;
		case OPEN+1:
		case OPEN+2:
		case OPEN+3:
		case OPEN+4:
		case OPEN+5:
		case OPEN+6:
		case OPEN+7:
		case OPEN+8:
		case OPEN+9:
			{
				register int no;
				const carac *save;

				no = OP(scan) - OPEN;
				save = reginput;
				
				if (regstartp[no] == NULL)
					regstartp[no] = save;
					
				if (regmatch(next)) {
					/*
					 * Don't set startp if some later
					 * invocation of the same parentheses
					 * already has.
					 
					if (regstartp[no] == NULL)
						regstartp[no] = save;
					printf("%d ouverte\n",no);*/
					return(1);
				}
				else
				{
					if (regstartp[no]==save)
						regstartp[no]=NULL;
					return(0);
				}
			}
			break;
		case CLOSE+1:
		case CLOSE+2:
		case CLOSE+3:
		case CLOSE+4:
		case CLOSE+5:
		case CLOSE+6:
		case CLOSE+7:
		case CLOSE+8:
		case CLOSE+9:
			{
				register int no;
				const carac *save;

				no = OP(scan) - CLOSE;
				save = reginput;

				if (regendp[no] == NULL)
					regendp[no] = save;
					
				if (regmatch(next))
				{
					/*
					 * Don't set endp if some later
					 * invocation of the same parentheses
					 * already has.
					 */
					/*if (regendp[no] == NULL)
						regendp[no] = save;
					printf("%d fermée\n",no);*/
					return(1);
				}
				else
				{
					if (regendp[no]==save)
						regendp[no] =NULL;
					return(0);
				}
			}
			break;
		case RAPPEL+1:
		case RAPPEL+2:
		case RAPPEL+3:
		case RAPPEL+4:
		case RAPPEL+5:
		case RAPPEL+6:
		case RAPPEL+7:
		case RAPPEL+8:
		case RAPPEL+9:
			{
				int r=OP(scan)-RAPPEL;
				if (regendp[r]==NULL || regstartp[r]==NULL)
				{
					//printf("%d pas enregistrée\n",r);
					return 0;
				}
				size_t len=regendp[r]-regstartp[r];
//				printf("%s (%d) contre %s\n",regstartp[r],len,reginput);
				if (czncmp(regstartp[r], reginput, len) != 0)
					return 0;
				reginput+=len;
				break;
			}
		case BRANCH:
			{
				const carac *save;

				if (OP(next) != BRANCH)		/* No choice. */
					next = OPERAND(scan);	/* Avoid recursion. */
				else {
					do {
						save = reginput;
						if (regmatch(OPERAND(scan)))
							return(1);
						reginput = save;
						scan = regnext(scan);
					} while (scan != NULL && OP(scan) == BRANCH);
					return(0);
					/* NOTREACHED */
				}
			}
			break;
		case STAR:
		case PLUS:
			{
				register carac nextch;
				register int no;
				const carac *save;
				register int min;

				/*
				 * Lookahead to avoid useless match attempts
				 * when we know what caracacter comes next.
				 */
				nextch = '\0';
				if (OP(next) == EXACTLY)
					nextch = *OPERAND(next);
				min = (OP(scan) == STAR) ? 0 : 1;
				save = reginput;
				no = regrepeat(OPERAND(scan));
				while (no >= min) {
					/* If it could work, try it. */
					if (nextch == '\0' || *reginput == nextch)
						if (regmatch(next))
							return(1);
					/* Couldn't or didn't -- back up. */
					no--;
					reginput = save + no;
				}
				return(0);
			}
			break;
		case END:
			return(1);	/* Success! */
			break;
		default:
			throw("mémoire corrompue");
			return(0);
			break;
		}

		scan = next;
	}

	/*
	 * We get here only if there's trouble -- normally "case END" is
	 * the terminating point.
	 */
	throw("pointeurs corrompus dans une expression régulière");
	return(0);
}

/*
 - regrepeat - repeatedly match something simple, report how many
 */
int RegExpExec::regrepeat(const carac *p)
{
	int count = 0;
	const carac *scan;
	const carac *opnd;

	scan = reginput;
	opnd = OPERAND(p);
	switch (OP(p))
	{
	case ANY:
		count = (int) czlen(scan);
		scan += count;
		break;
	case EXACTLY:
		while (*opnd == *scan) {
			count++;
			scan++;
		}
		break;
	case ANYOF:
		while (*scan != '\0' && czschr(opnd, *scan) != NULL) {
			count++;
			scan++;
		}
		break;
	case ANYBUT:
		while (*scan != '\0' && czschr(opnd, *scan) == NULL) {
			count++;
			scan++;
		}
		break;

	case ANYOFINT:
		while (*scan != '\0' && strintervalle(opnd, *scan) != NULL) {
			count++;
			scan++;
		}
		break;
	case ANYBUTINT:
		while (*scan != '\0' && strintervalle(opnd, *scan) == NULL) {
			count++;
			scan++;
		}
		break;
		
	case RAPPEL+1:
	case RAPPEL+2:
	case RAPPEL+3:
	case RAPPEL+4:
	case RAPPEL+5:
	case RAPPEL+6:
	case RAPPEL+7:
	case RAPPEL+8:
	case RAPPEL+9:
		{
			int r=OP(scan)-RAPPEL;
			if (regendp[r]==NULL || regstartp[r]==NULL)
				return 0;
			size_t len=regendp[r]-regstartp[r];
			if (czncmp(opnd, reginput, len) != 0)
				return 0;
			reginput+=len;
			break;
		}
		
	default:		/* Oh dear.  Called inappropriately. */
		throw ("internal foulup");
		count = 0;	/* Best compromise. */
		break;
	}
	reginput = scan;

	return(count);
}

/*
 - regnext - dig the "next" pointer out of a node
 */
carac * RegExpExec::regnext(const carac *p)
{
	int offset;

	/*if (p == &regdummy)
		return(NULL);*/

	offset = NEXT(p);
	if (offset == 0)
		return(NULL);

	if (OP(p) == BACK)
		return (carac*)(p-offset);
	else
		return (carac*)(p+offset);
}

carac * RegExpComp::regnext(const carac *p)
{
	int offset;

	if (p == &regdummy)
		return(NULL);

	offset = NEXT(p);
	if (offset == 0)
		return(NULL);

	if (OP(p) == BACK)
		return (carac*)(p-offset);
	else
		return (carac*)(p+offset);
}

#if (0)

namespace GotoPP
{
	carac *regprop();
}
/*
 - regdump - dump a regexp onto stdout in vaguely comprehensible form
 */
void GotoPP::regdump(regexp * r)
{
	carac *s;
	carac op = EXACTLY;	/* Arbitrary non-END op. */
	carac *next;

	s = r->program + 1;
	while (op != END)
	{	/* While that wasn't END last time... */
		op = OP(s);
		printf("%2d%s", s-r->program, regprop(s));	/* Where, what. */
		next = regnext(s);
		if (next == NULL)		/* Next ptr. */
			printf("(0)");
		else 
			printf("(%d)", (s-r->program)+(next-s));
		s += 3;
		if (op == ANYOF || op == ANYBUT || op == EXACTLY) {
			/* Literal string, where present. */
			while (*s != '\0') {
				putcarac(*s);
				s++;
			}
			s++;
		}
		putcarac('\n');
	}

	/* Header fields of interest. */
	if (r->regstart != '\0')
		printf("start `%c' ", r->regstart);
	if (r->reganch)
		printf("anchored ");
	if (r->regmust != -1)
		printf("must have \"%s\"", &r->program[r->regmust]);
	printf("\n");
}

/*
 - regprop - printable representation of opcode
 */
carac * GotoPP::regprop(const carac *op)
{
	carac *p;
	static carac buf[50];

	czncpy(buf, _T(":"),49);

	switch (OP(op))
	{
	case BOL:
		p = _T("BOL");
		break;
	case EOL:
		p = _T("EOL");
		break;
	case ANY:
		p = _T("ANY");
		break;
	case ANYOF:
		p = _T("ANYOF");
		break;
	case ANYBUT:
		p = _T("ANYBUT");
		break;
	case ANYOFINT:
		p = _T("ANYOFINT");
		break;
	case ANYBUTINT:
		p = _T("ANYBUTINT");
		break;
	case BRANCH:
		p = _T("BRANCH");
		break;
	case EXACTLY:
		p = _T("EXACTLY");
		break;
	case NOTHING:
		p = _T("NOTHING");
		break;
	case BACK:
		p = _T("BACK");
		break;
	case END:
		p = _T("END");
		break;
	case OPEN+1:
	case OPEN+2:
	case OPEN+3:
	case OPEN+4:
	case OPEN+5:
	case OPEN+6:
	case OPEN+7:
	case OPEN+8:
	case OPEN+9:
		cznprintf(buf+czlen(buf),49, _T("OPEN%d"), OP(op)-OPEN);
		p = NULL;
		break;
	case CLOSE+1:
	case CLOSE+2:
	case CLOSE+3:
	case CLOSE+4:
	case CLOSE+5:
	case CLOSE+6:
	case CLOSE+7:
	case CLOSE+8:
	case CLOSE+9:
		cznprintf(buf+czlen(buf),49, _T("CLOSE%d"), OP(op)-CLOSE);
		p = NULL;
		break;
	case STAR:
		p = _T("STAR");
		break;
	case PLUS:
		p = _T("PLUS");
		break;
		
	case RAPPEL+1:
	case RAPPEL+2:
	case RAPPEL+3:
	case RAPPEL+4:
	case RAPPEL+5:
	case RAPPEL+6:
	case RAPPEL+7:
	case RAPPEL+8:
	case RAPPEL+9:
		cznprintf(buf+czlen(buf),49, _T("RAPPEL%d"), OP(op)-RAPPEL);
		p = NULL;
		break;
	default:
		throw TXT("corrupted opcode");
		break;
	}
	if (p != NULL)
		(void) czcat(buf, p);
	return(buf);
}
#endif

/*
 * The following is provided for those people who do not have strcspn() in
 * their C libraries.  They should get off their butts and do something
 * about it; at least one public-domain implementation of those (highly
 * useful) string routines has been published on Usenet.
 */
#ifdef STRCSPN
/*
 * strcspn - find length of initial segment of s1 consisting entirely
 * of caracacters not from s2
 */

int GotoPP::strcspn(carac *s1, carac *s2)
{
	carac *scan1;
	carac *scan2;
	int count;

	count = 0;
	for (scan1 = s1; *scan1 != '\0'; scan1++) {
		for (scan2 = s2; *scan2 != '\0';)	/* ++ moved down. */
			if (*scan1 == *scan2++)
				return(count);
		count++;
	}
	return(count);
}
#endif
