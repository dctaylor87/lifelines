/* 
   Copyright (c) 1991-1999 Thomas T. Wetmore IV

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use, copy,
   modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/
/*=============================================================
 * builtin.c -- Many interpreter builtin functions
 * Copyright(c) 1992-95 by T.T. Wetmore IV; all rights reserved
 *   2.3.4 - 24 Jun 93    2.3.5 - 07 Sep 93
 *   3.0.0 - 07 May 94    3.0.2 - 03 Jan 95
 *   3.0.3 - 02 Jul 96
 *===========================================================*/
/* modified 05 Jan 2000 by Paul B. McBride (pmcbride@tiac.net) */
/* modified 2000-04-12 J.F.Chandler */

#include "sys_inc.h"
#include "llstdlib.h"
#include "table.h"
#include "translat.h"
#include "gedcom.h"
#include "cache.h"
#include "interp.h"
#include "indiseq.h"
#include "liflines.h"
#include "screen.h"
#include "interpi.h"

STRING nonnum = (STRING) "At least one argument to %s is not numeric";

extern STRING llprograms;

/*========================================+
 * __getint -- Have user provide integer
 *   usage: getint(IDEN [,STRING]) --> VOID
 *=======================================*/
PVALUE
__getint (PNODE node,
          TABLE stab,
          BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	INT val;
	STRING msg = (STRING) "Enter integer for program";
	PVALUE mval = NULL;
	if (!iistype(arg, IIDENT)) {
		prog_error(node, "1st arg to getint must be a variable");
		*eflg = TRUE;
		return NULL;
	}
	if (inext(arg)) {
		mval = evaluate(inext(arg), stab, eflg);
		if (*eflg) return NULL;
		if (ptype(mval) != PSTRING) {
			*eflg = TRUE;
			prog_error(node, "2nd arg to getint must be a string");
			delete_pvalue(mval);
			return NULL;
		}
		msg = (STRING) pvalue(mval);
	}
	val = ask_for_int(msg);
	assign_iden(stab, iident(arg), create_pvalue(PINT, (WORD)val));
	if (mval) delete_pvalue(mval);
	return NULL;
}
/*========================================+
 * __getstr -- Have user provide string
 *   usage: getstr(IDEN [,STRING]) --> VOID
 *=======================================*/
PVALUE
__getstr (PNODE node,
          TABLE stab,
          BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	STRING val;
	STRING msg = (STRING) "Enter string for program";
	PVALUE mval = NULL;
	if (!iistype(arg, IIDENT)) {
		prog_error(node, "1st arg to getstr must be a variable");
		*eflg = TRUE;
		return NULL;
	}
	if (inext(arg)) {
		mval = evaluate(inext(arg), stab, eflg);
		if (*eflg) return NULL;
		if (ptype(mval) != PSTRING) {
			*eflg = TRUE;
			prog_error(node, "2nd arg to getstr must be a string");
			delete_pvalue(mval);
			return NULL;
		}
		msg = (STRING) pvalue(mval);
	}
	val = (STRING) ask_for_string(msg, "enter string: ");
	assign_iden(stab, iident(arg), create_pvalue(PSTRING, (WORD)val));
	if (mval) delete_pvalue(mval);
	return NULL;
}
/*=========================================+
 * __getindi -- Have user identify person
 *   usage: getindi(IDEN [,STRING]) --> VOID
 *========================================*/
PVALUE
__getindi (PNODE node,
           TABLE stab,
           BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	CACHEEL cel;
	STRING key, msg = (STRING) "Identify person for program:";
	PVALUE val = NULL;
	if (!iistype(arg, IIDENT)) {
		prog_error(node, "1st arg to getindi must be a variable");
		*eflg = TRUE;
		return NULL;
	}
	if (inext(arg)) {
		val = eval_and_coerce(PSTRING, inext(arg), stab, eflg);
		if (*eflg) {
			prog_error(node, "2nd arg to getindi must be a string");
			return NULL;
		}
		msg = (STRING) pvalue(val);
	}
	assign_iden(stab, iident(arg), create_pvalue(PINDI, NULL));
	key = ask_for_indi_key(msg, NOCONFIRM, TRUE);
	if (!key) return NULL;
	cel = key_to_indi_cacheel(key);
	assign_iden(stab, iident(arg), create_pvalue(PINDI, (WORD)cel));
	if (val) delete_pvalue(val);
	return NULL;
}
#if 0
/*===================================================
 * __getindidate -- Have user identify person
 *   usage: getindidate(IDEN, INT [,STRING]) --> VOID
 *=================================================*/
WORD
__getindidate (PNODE node,
               TABLE stab,
               BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	INT year;
	CACHEEL cel;
	STRING ttl = "Identify person for report program:";
	*eflg = TRUE;
	if (!iistype(arg, IIDENT)) return NULL;
	*eflg = FALSE;
	year = (INT) evaluate(arg = inext(arg), stab, eflg);
	if (*eflg) return NULL;
	if (inext(arg)) ttl = (STRING) evaluate(inext(arg), stab, eflg);
	if (*eflg) return NULL;
	cel = key_to_indi_cacheel(ask_for_indi_key(ttl, NOCONFIRM, TRUE));
	assign_iden(stab, iident(arg), (WORD) cel);
	return NULL;
}
#endif
/*=====================================+
 * __getfam -- Have user identify family
 *   usage: getfam(IDEN) --> VOID
 *====================================*/
PVALUE
__getfam (PNODE node,
          TABLE stab,
          BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	CACHEEL cel = NULL;
	NODE fam;
	if (!iistype(arg, IIDENT)) {
		prog_error(node, "1st arg to getfam must be a variable");
		*eflg = TRUE;
		return NULL;
	}
	assign_iden(stab, iident(arg), NULL);
	fam = ask_for_fam("Enter a spouse from family.",
	    "Enter a sibling from family.");
	if (fam) cel = fam_to_cacheel(fam);
	assign_iden(stab, iident(arg), create_pvalue(PFAM, (WORD)cel));
	return NULL;
}
/*=================================================+
 * __getindiset -- Have user identify set of persons
 *   usage: getindiset(IDEN [,STRING]) --> VOID
 *================================================*/
PVALUE
__getindiset (PNODE node,
              TABLE stab,
              BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	INDISEQ seq;
	STRING msg = (STRING) "Identify list of persons for program:";
	PVALUE val = NULL;
	if (!iistype(arg, IIDENT)) {
		prog_error(node, "1st arg to getindiset must be a variable");
		*eflg = TRUE;
		return NULL;
	}
	if (inext(arg)) {
		val = eval_and_coerce(PSTRING, inext(arg), stab, eflg);
		if (*eflg) {
			prog_error(node, "2nd arg to getindiset must be a string");
			return NULL;
		}
		msg = (STRING) pvalue(val);
	}
	seq = (INDISEQ) ask_for_indi_list(msg, TRUE);
	if (val) delete_pvalue(val);
	assign_iden(stab, iident(arg), create_pvalue(PSET, (WORD)seq));
	return NULL;
}
/*==================================+
 * __gettoday -- Create today's event
 *   usage: gettoday() --> EVENT
 *=================================*/
PVALUE
__gettoday (PNODE node,
            TABLE stab,
            BOOLEAN *eflg)
{
	NODE prnt = create_node(NULL, "EVEN", NULL, NULL);
	NODE chil = create_node(NULL, "DATE", get_date(), prnt);

#ifdef DEBUG
	llwprintf("__gettoday: called\n");
#endif

	nchild(prnt) = chil;
	return create_pvalue(PGNODE, (WORD)prnt);
}
/*====================================+
 * __name -- Find person's name
 *   usage: name(INDI[,BOOL]) -> STRING
 *===================================*/
PVALUE __name (PNODE node,
               TABLE stab,
               BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	NODE name, indi = eval_indi(arg, stab, eflg, NULL);
	BOOLEAN caps = TRUE;
	PVALUE val;
	TRANTABLE ttr = tran_tables[MINRP];
	if (*eflg) {
		prog_error(node, "1st arg to name must be a person");
		return NULL;
	}
	if (!indi) return create_pvalue(PSTRING, NULL);
	if (inext(arg)) {
		val = eval_and_coerce(PBOOL, inext(arg), stab, eflg);
		if (*eflg) {
			prog_error(node, "2nd arg to name must be boolean");
			return NULL;
		}
		caps = (BOOLEAN) pvalue(val);
		delete_pvalue(val);
	}
	if (!(name = find_tag(nchild(indi), "NAME"))) {
		*eflg = TRUE;
		prog_error(node, "person does not have a name");
		return NULL;
	}
	return create_pvalue(PSTRING,
	    (WORD)manip_name(nval(name), ttr, caps, TRUE, 68));
}
/*==================================================+
 * __fullname -- Process person's name
 *   usage: fullname(INDI, BOOL, BOOL, INT) -> STRING
 *=================================================*/
PVALUE
__fullname (PNODE node,
            TABLE stab,
            BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	NODE name, indi;
	PVALUE val;
	BOOLEAN caps;
	BOOLEAN myreg;
	INT len;
	TRANTABLE ttr;

	ttr = tran_tables[MINRP];
	indi = eval_indi(arg, stab, eflg, NULL);
	if (*eflg || !indi) {
		*eflg = TRUE;
		prog_error(node, "1st arg to fullname must be a person");
		return NULL;
	}
	val = eval_and_coerce(PBOOL, arg = inext(arg), stab, eflg);
	if (*eflg) {
		prog_error(node, "2nd arg to fullname must be boolean");
		return NULL;
	}
	caps = (BOOLEAN) pvalue(val);
	delete_pvalue(val);
	val = eval_and_coerce(PBOOL, arg = inext(arg), stab, eflg);
	if (*eflg) {
		prog_error(node, "3rd arg to fullname must be boolean");
		return NULL;
	}
	myreg = (BOOLEAN) pvalue(val);
	delete_pvalue(val);
	val = eval_and_coerce(PINT, arg = inext(arg), stab, eflg);
	if (*eflg) {
		prog_error(node, "4th arg to fullname must be integer");
		return NULL;
	}
	len = (INT) pvalue(val);
	delete_pvalue(val);
	if (!(name = NAME(indi)) || !nval(name)) {
		*eflg = TRUE;
		prog_error(node, "person does not have a name");
		return NULL;
	}
	return create_pvalue(PSTRING,
	    (WORD)manip_name(nval(name), ttr, caps, myreg, len));
}
/*==================================+
 * __surname -- Find person's surname using new getasurname() routine.
 *   usage: surname(INDI) -> STRING
 *=================================*/
PVALUE
__surname (PNODE node,
           TABLE stab,
           BOOLEAN *eflg)
{
	NODE name, indi = eval_indi(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "the arg to surname must be a person");
		return NULL;
	}
	if (!indi) return create_pvalue(PSTRING, NULL);
	if (!(name = NAME(indi)) || !nval(name)) {
		*eflg = TRUE;
		prog_error(node, "person does not have a name");
		return NULL;
	}
	return create_pvalue(PSTRING, (WORD)getasurname(nval(name)));
}
/*========================================+
 * __soundex -- SOUNDEX function on persons
 *   usage: soundex(INDI) -> STRING
 *=======================================*/
PVALUE
__soundex (PNODE node,
           TABLE stab,
           BOOLEAN *eflg)
{
	NODE name, indi = eval_indi(iargs(node), stab, eflg, NULL);
	if (*eflg || !indi) {
		*eflg = TRUE;
		prog_error(node, "1st arg to soundex must be a person");
		return NULL;
	}
	if (!(name = NAME(indi)) || !nval(name)) {
		*eflg = TRUE;
		prog_error(node, "person does not have a name");
		return NULL;
	}
	return create_pvalue(PSTRING, (WORD)soundex(getsurname(nval(name))));
}
/*===========================================+
 * __strsoundex -- SOUNDEX function on strings
 *   usage: strsoundex(STRING) -> STRING
 *==========================================*/
PVALUE
__strsoundex (PNODE node,
              TABLE stab,
              BOOLEAN *eflg)
{
	PVALUE new, val = evaluate(iargs(node), stab, eflg);
	if (*eflg || !val || ptype(val) != PSTRING) {
		*eflg = TRUE;
		prog_error(node, "1st arg to strsoundex must be a string");
		return NULL;
	}
	new = create_pvalue(PSTRING, (WORD)soundex(pvalue(val)));
	delete_pvalue(val);
	return new;
}
/*===============================+
 * __givens -- Find given names
 *   usage: givens(INDI) -> STRING
 *==============================*/
PVALUE
__givens (PNODE node,
          TABLE stab,
          BOOLEAN *eflg)
{
	NODE name, indi = eval_indi(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "1st arg to givens must be a person");
		return NULL;
	}
	if (!indi) return create_pvalue(PSTRING, NULL);
	if (!(name = NAME(indi)) || !nval(name)) {
		*eflg = TRUE;
		prog_error(node, "person does not have a name");
		return NULL;
	}
	return create_pvalue(PSTRING, (WORD)givens(nval(name)));
}
/*===============================+
 * __set -- Assignment operation
 *   usage: set(IDEN, ANY) -> VOID
 *==============================*/
PVALUE
__set (PNODE node,
       TABLE stab,
       BOOLEAN *eflg)
{
	PNODE var = (PNODE) iargs(node);
	PNODE expr = inext(var);
	PVALUE val;
	if (!iistype(var, IIDENT)) {
		*eflg = TRUE;
		prog_error(node, "1st arg to set must be a variable");
		return NULL;
	}
	val = evaluate(expr, stab, eflg);
	if (*eflg || !val) {
		*eflg = TRUE;
		prog_error(node, "2nd arg to set is in error");
		return NULL;
	}
	assign_iden(stab, iident(var), val);
	return NULL;
}
/*=========================================+
 * __husband -- Find first husband of family
 *   usage: husband(FAM) -> INDI
 *========================================*/
PVALUE
__husband (PNODE node,
           TABLE stab,
           BOOLEAN *eflg)
{
	NODE fam = eval_fam(iargs(node), stab, eflg, NULL);
	if (*eflg || !fam) {
		*eflg = TRUE;
		prog_error(node, "1st arg to husband must be a family");
		return NULL;
	}
	return create_pvalue(PINDI, (WORD)indi_to_cacheel(fam_to_husb(fam)));
}
/*===================================+
 * __wife -- Find first wife of family
 *   usage: wife(FAM) -> INDI
 *==================================*/
PVALUE
__wife (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	NODE fam = eval_fam(iargs(node), stab, eflg, NULL);
	if (*eflg || !fam) {
		*eflg = TRUE;
		prog_error(node, "1st arg to wife must be a family");
		return NULL;
	}
	return create_pvalue(PINDI, (WORD)indi_to_cacheel(fam_to_wife(fam)));
}
/*==========================================+
 * __firstchild -- Find first child of family
 *   usage: firstchild(FAM) -> INDI
 *=========================================*/
PVALUE
__firstchild (PNODE node,
              TABLE stab,
              BOOLEAN *eflg)
{
	NODE fam = eval_fam(iargs(node), stab, eflg, NULL);
	if (*eflg || !fam) {
		*eflg = TRUE;
		prog_error(node, "arg to firstchild must be a family");
		return NULL;
	}
	return create_pvalue(PINDI, (WORD)indi_to_cacheel(fam_to_first_chil(fam)));
}
/*========================================+
 * __lastchild -- Find last child of family
 *   usage: lastchild(FAM) -> INDI
 *=======================================*/
PVALUE
__lastchild (PNODE node,
             TABLE stab,
             BOOLEAN *eflg)
{
	NODE fam = eval_fam(iargs(node), stab, eflg, NULL);
	if (*eflg || !fam) {
		*eflg = TRUE;
		prog_error(node, "arg to firstchild must be a family");
		return NULL;
	}
	return create_pvalue(PINDI, (WORD)indi_to_cacheel(fam_to_last_chil(fam)));
}
/*=================================+
 * __marr -- Find marriage of family
 *   usage: marriage(FAM) -> EVENT
 *================================*/
PVALUE
__marr (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	NODE fam = eval_fam(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "the arg to marriage must be a family");
		return NULL;
	}
	if (!fam) return create_pvalue(PGNODE, NULL);
	return create_pvalue(PGNODE, (WORD)MARR(fam));
}
/*==========================================+
 * __birt -- Find first birth event of person
 *   usage: birth(INDI) -> EVENT
 *=========================================*/
PVALUE
__birt (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	NODE indi = eval_indi(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "the arg to birth must be a person");
		return NULL;
	} 
	if (!indi) return create_pvalue(PGNODE, NULL);
	return create_pvalue(PGNODE, (WORD)BIRT(indi));
}
/*==========================================+
 * __deat -- Find first death event of person
 *   usage: death(INDI) -> EVENT
 *=========================================*/
PVALUE
__deat (PNODE node,
        TABLE stab,
        BOOLEAN  *eflg)
{
	NODE indi = eval_indi(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "the arg to death must be a person");
		return NULL;
	}
	if (!indi) return create_pvalue(PGNODE, NULL);
	return create_pvalue(PGNODE, (WORD)DEAT(indi));
}
/*============================================+
 * __bapt -- Find first baptism event of person
 *   usage: baptism(INDI) -> EVENT
 *===========================================*/
PVALUE
__bapt (PNODE node,
        TABLE stab,
        BOOLEAN  *eflg)
{
	NODE indi = eval_indi(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "the arg to baptism must be a person");
		return NULL;
	}
	if (!indi) return create_pvalue(PGNODE, NULL);
	return create_pvalue(PGNODE, (WORD)BAPT(indi));
}
/*===========================================+
 * __buri -- Find first burial event of person
 *   usage: burial(INDI) -> EVENT
 *==========================================*/
PVALUE
__buri (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	NODE indi = eval_indi(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "the arg to burial must be a person");
		return NULL;
	}
	if (!indi) return create_pvalue(PGNODE, NULL);
	return create_pvalue(PGNODE, (WORD)BURI(indi));
}
/*====================================+
 * __titl -- Find first title of person
 *   usage: title(INDI) -> STRING
 *===================================*/
PVALUE
__titl (PNODE node,
        TABLE stab,
        BOOLEAN  *eflg)
{
	NODE titl, indi = eval_indi(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "the arg to title must be a person");
		return NULL;
	}
	if (!indi) return create_pvalue(PSTRING, NULL);
	titl = find_tag(nchild(indi), "TITL");
	return create_pvalue(PSTRING, (WORD)(titl ? nval(titl) : NULL));
}
/*===================================+
 * __long -- Return long form of event
 *   usage: long(EVENT) -> STRING
 *==================================*/
PVALUE
__long (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	PVALUE val = eval_and_coerce(PGNODE, iargs(node), stab, eflg);
	NODE even;
	TRANTABLE ttr = tran_tables[MINRP];
	if (*eflg) {
		prog_error(node, "the arg to long must be a record line");
		return NULL;
	}
	even = (NODE) pvalue(val);
	delete_pvalue(val);
	return create_pvalue(PSTRING, (WORD)event_to_string(even, ttr, FALSE));
}
/*=====================================+
 * __short -- Return short form of event
 *   usage: short(EVENT) -> STRING
 *====================================*/
PVALUE
__short (PNODE node,
         TABLE stab,
         BOOLEAN *eflg)
{
	PVALUE val = eval_and_coerce(PGNODE, iargs(node), stab, eflg);
	NODE even;
	TRANTABLE ttr = tran_tables[MINRP];
	if (*eflg) {
		prog_error(node, "the arg to short must be a record line");
		return NULL;
	}
	even = (NODE) pvalue(val);
	delete_pvalue(val);
	return create_pvalue(PSTRING, (WORD)event_to_string(even, ttr, TRUE));
}
/*===============================+
 * __fath -- Find father of person
 *   usage: father(INDI) -> INDI
 *==============================*/
PVALUE
__fath (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	NODE indi = eval_indi(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "arg to father must be a person");
		return NULL;
	}
	if (!indi) return create_pvalue(PINDI, NULL);
	return create_pvalue(PINDI, (WORD)indi_to_cacheel(indi_to_fath(indi)));
}
/*===============================+
 * __moth -- Find mother of person
 *   usage: mother(INDI) -> INDI
 *==============================*/
PVALUE
__moth (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	NODE indi = eval_indi(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "arg to mother must be a person");
		return NULL;
	}
	if (!indi) return create_pvalue(PINDI, NULL);
	return create_pvalue(PINDI, (WORD)indi_to_cacheel(indi_to_moth(indi)));
}
/*===========================================+
 * __parents -- Find parents' family of person
 *   usage: parents(INDI) -> FAM
 *==========================================*/
PVALUE
__parents (PNODE node,
           TABLE stab,
           BOOLEAN *eflg)
{
	NODE indi = eval_indi(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "arg to parents must be a person");
		return NULL;
	}
	if (!indi) return create_pvalue(PFAM, NULL);
	return create_pvalue(PFAM, (WORD)fam_to_cacheel(indi_to_famc(indi)));
}
/*==========================================+
 * __nextsib -- Find person's younger sibling
 *   usage: nextsib(INDI) -> INDI
 *=========================================*/
PVALUE
__nextsib (PNODE node,
           TABLE stab,
           BOOLEAN *eflg)
{
	NODE indi = eval_indi(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "arg to nextsib must be a person");
		return NULL;
	}
	if (!indi) return create_pvalue(PINDI, NULL);
	return create_pvalue(PINDI, (WORD)indi_to_cacheel(indi_to_next_sib(indi)));
}
/*========================================+
 * __prevsib -- Find person's older sibling
 *   usage: prevsib(INDI) -> INDI
 *=======================================*/
PVALUE
__prevsib (PNODE node,
           TABLE stab,
           BOOLEAN *eflg)
{
	NODE indi = eval_indi(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "arg to prevsib must be a person");
		return NULL;
	}
	if (!indi) return create_pvalue(PINDI, NULL);
	return create_pvalue(PINDI, (WORD)indi_to_cacheel(indi_to_prev_sib(indi)));
}
/*========================================+
 * __d -- Return cardinal integer as string
 *   usage: d(INT) -> STRING
 *=======================================*/
PVALUE
__d (PNODE node,
     TABLE stab,
     BOOLEAN *eflg)
{
	static char scratch[20];
	PVALUE val;
	val = eval_and_coerce(PINT, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to d is not an integer");
		return NULL;
	}
	sprintf(scratch, "%d", (INT) pvalue(val));
	set_pvalue(val, PSTRING, (WORD)scratch);
	return val;
}
/*=============================================+
 * __f -- Return floating point number as string
 *   usage: f(FLOAT[,INT]) -> STRING
 *============================================*/
PVALUE
__f (PNODE node,
     TABLE stab,
     BOOLEAN *eflg)
{
	char scratch[20];
	char format[10];
	UNION u;
	INT prec = 2;
	PNODE arg = (PNODE) iargs(node);
	PVALUE val = eval_and_coerce(PFLOAT, arg, stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to f is not a float");
		return NULL;
	}
/*HERE*/
	u.w = pvalue(val);
	if (inext(arg)) {
		val = eval_and_coerce(PINT, inext(arg), stab, eflg);
		if (*eflg) {
			prog_error(node, "2nd arg to f must be an integer");
			return NULL;
		}
		prec = (INT) pvalue(val);
		if (prec < 0) prec = 0;
		if (prec > 10) prec = 10;
	}
	sprintf(format, "%%.%df", prec);

#ifdef DEBUG
	llwprintf("format is %s\n", format);
#endif

	sprintf(scratch, format, u.f);
	set_pvalue(val, PSTRING, (WORD)scratch);
	return val;
}
/*==========================================+
 * ___alpha -- Convert small integer to letter
 *   usage: alpha(INT) -> STRING
 *=========================================*/
PVALUE
___alpha (PNODE node,
          TABLE stab,
          BOOLEAN *eflg)
{
	static char scratch[2];
	INT i;
	PVALUE val = evaluate(iargs(node), stab, eflg);
	if (*eflg) return NULL;
	TYPE_CHECK(PINT, val);
	i = (INT) pvalue(val);
	delete_pvalue(val);
	if (i < 1 || i > 26)
		sprintf(scratch, "XX");
	else
		sprintf(scratch, "%c", 'a' + i - 1);
	return create_pvalue(PSTRING, (WORD)scratch);
}
/*================================================+
 * __ord -- Convert small integer to ordinal string
 *   usage: ord(INT) -> STRING
 *===============================================*/
static char *ordinals[] = {
	"first", "second", "third", "fourth", "fifth",
	"sixth", "seventh", "eighth", "ninth", "tenth",
	"eleventh", "twelfth"
};
PVALUE
__ord (PNODE node,
       TABLE stab,
       BOOLEAN *eflg)
{
	static char scratch[12];
	PVALUE val = evaluate(iargs(node), stab, eflg);
	INT i;
	TYPE_CHECK(PINT, val);
	i = (INT) pvalue(val);
	delete_pvalue(val);
	if (*eflg || i < 1) return NULL;
	if (i > 12)
		sprintf(scratch, "%dth", i);
	else
		sprintf(scratch, ordinals[i - 1]);
	return create_pvalue(PSTRING, (WORD)scratch);
}
/*==================================================+
 * __card -- Convert small integer to cardinal string
 *   usage: card(INT) -> STRING
 *=================================================*/
static char *cardinals[] = {
	"zero", "one", "two", "three", "four", "five",
	"six", "seven", "eight", "nine", "ten",
	"eleven", "twelve"
};
PVALUE
__card (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	static char scratch[8];
	PVALUE val = evaluate(iargs(node), stab, eflg);
	INT i;
	if (*eflg) return NULL;
	TYPE_CHECK(PINT, val);
	i = (INT) pvalue(val);
	delete_pvalue(val);
	if (i < 0 || i > 12)
		sprintf(scratch, "%d", i);
	else
		sprintf(scratch, cardinals[i]);
	return create_pvalue(PSTRING, (WORD)scratch);
}
/*==========================================+
 * __roman -- Convert integer to Roman numeral
 *   usage: roman(INT) -> STRING
 *=========================================*/
static char *rodigits[] = {
	"", "i", "ii", "iii", "iv", "v", "vi", "vii", "viii", "ix"
};
static char *rotens[] = {
	"", "x", "xx", "xxx", "xl", "l", "lx", "lxx", "lxxx", "xc"
};
PVALUE
__roman (PNODE node,
         TABLE stab,
         BOOLEAN *eflg)
{
	static char scratch[10];
	PVALUE val = evaluate(iargs(node), stab, eflg);
	INT i;
	if (*eflg) return NULL;
	TYPE_CHECK(PINT, val);
	i = (INT) pvalue(val);
	delete_pvalue(val);
	if (i < 1 || i >= 99)
		sprintf(scratch, "%d", i);
	else
		sprintf(scratch, "%s%s", rotens[i/10], rodigits[i%10]);
	return create_pvalue(PSTRING, (WORD)scratch);
}
/*================================================+
 * __nchildren -- Find number of children in family
 *   usage: nchildren(FAM) -> INT
 *===============================================*/
PVALUE
__nchildren (PNODE node,
             TABLE stab,
             BOOLEAN *eflg)
{
	NODE fam = eval_fam(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "arg to nchildren must be a family");
		return NULL;
	}
	if (!fam) return create_pvalue(PINT, 0);
	return create_pvalue(PINT, (WORD)length_nodes(CHIL(fam)));
}
/*===================================================+
 * __nfamilies -- Find number of families person is in
 *   usage: nfamilies(INDI) -> INT
 *==================================================*/
PVALUE
__nfamilies (PNODE node,
             TABLE stab,
             BOOLEAN *eflg)
{
	NODE indi = eval_indi(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "arg to nfamilies must be a person");
		return NULL;
	}
	if (!indi) return create_pvalue(PINT, 0);
	return create_pvalue(PINT, (WORD)length_nodes(FAMS(indi)));
}
/*===============================================+
 * __nspouses -- Find number of spouses person has
 *   usage: nspouses(INDI) -> INT
 *==============================================*/
PVALUE
__nspouses (PNODE node,
            TABLE stab,
            BOOLEAN *eflg)
{
	INT nspouses;
	NODE indi = eval_indi(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "arg to nspouses must be a person");
		return NULL;
	}
	if (!indi) return create_pvalue(PINT, 0);
	FORSPOUSES(indi,spouse,fam,nspouses) ENDSPOUSES
	return create_pvalue(PINT, (WORD)nspouses);
}
/*=============================+
 * __eq -- Equal operation
 *   usage: eq(ANY, ANY) -> BOOL
 *============================*/
PVALUE
__eq (PNODE node,
      TABLE stab,
      BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	PVALUE val2, val1 = evaluate(arg, stab, eflg);
	if (*eflg) return NULL;
	val2 = evaluate(inext(arg), stab, eflg);
	if (*eflg) return NULL;
	eq_pvalues(val1, val2, eflg);
	if (*eflg) {
		prog_error(node, "incorrect operands for eq");
		return NULL;
	}
	return val1;
}
/*=============================+
 * __ne -- Not equal operation
 *   usage: ne(ANY, ANY) -> BOOL
 *============================*/
PVALUE
__ne (PNODE node,
      TABLE stab,
      BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	PVALUE val2, val1 = evaluate(arg, stab, eflg);
	if (*eflg) {
		prog_error(node, "1st arg to ne is in error");
		return NULL;
	}
	val2 = evaluate(inext(arg), stab, eflg);
	if (*eflg) {
		prog_error(node, "2nd arg to ne is in error");
		return NULL;
	}
	ne_pvalues(val1, val2, eflg);
	if (*eflg) {
		prog_error(node, "incorrect operands for ne");
		return NULL;
	}
	return val1;
}
/*===============================+
 * __le -- Less or equal operation
 *   usage: le(ANY, ANY) -> BOOL
 *==============================*/
PVALUE
__le (PNODE node,
      TABLE stab,
      BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	PVALUE val2, val1 = evaluate(arg, stab, eflg);
	if (*eflg) return NULL;
	val2 = evaluate(inext(arg), stab, eflg);
	if (*eflg) return NULL;
	le_pvalues(val1, val2, eflg);
	if (*eflg) {
		prog_error(node, "incorrect operands for le");
		return NULL;
	}
	return val1;
}
/*==================================+
 * __ge -- Greater or equal operation
 *   usage: ge(ANY, ANY) -> BOOL
 *=================================*/
PVALUE
__ge (PNODE node,
      TABLE stab,
      BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	PVALUE val2, val1 = evaluate(arg, stab, eflg);
	if (*eflg) return NULL;
	val2 = evaluate(inext(arg), stab, eflg);
	if (*eflg) return NULL;
	ge_pvalues(val1, val2, eflg);
	if (*eflg) {
		prog_error(node, "incorrect operands for ge");
		return NULL;
	}
	return val1;
}
/*============================+
 * __lt -- Less than operation
 *   usage: lt(ANY,ANY) -> BOOL
 *===========================*/
PVALUE
__lt (PNODE node,
      TABLE stab,
      BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	PVALUE val2, val1 = evaluate(arg, stab, eflg);

#ifdef DEBUG
	llwprintf("__lt @ %d\n", iline(node));
#endif

	if (*eflg) return NULL;
	val2 = evaluate(inext(arg), stab, eflg);
	if (*eflg) return NULL;
	lt_pvalues(val1, val2, eflg);
	if (*eflg) {
		prog_error(node, "incorrect operands for lt");
		return NULL;
	}
	return val1;
}
/*==============================+
 * __gt -- Greater than operation
 *   usage: gt(ANY, ANY) -> BOOL
 *=============================*/
PVALUE
__gt (PNODE node,
      TABLE stab,
      BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	PVALUE val2, val1 = evaluate(arg, stab, eflg);
	if (*eflg) return NULL;
	val2 = evaluate(inext(arg), stab, eflg);
	if (*eflg) return NULL;
	gt_pvalues(val1, val2, eflg);
	if (*eflg) {

#ifdef DEBUG
	show_pvalue(val1);
	show_pvalue(val2);
#endif
		prog_error(node, "incorrect operands for gt");
		return NULL;
	}
	return val1;
}
/*=================================+
 * __and -- And operation
 *   usage: and(ANY [,ANY]+) -> BOOL
 *================================*/
PVALUE
__and (PNODE node,
       TABLE stab,
       BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	PVALUE val2, val1 = eval_and_coerce(PBOOL, arg, stab, eflg);
	BOOLEAN rc = TRUE;
	if (*eflg) {

#ifdef DEBUG
	show_pvalue(val1);
#endif
		prog_error(node, "an arg to and is not boolean");
		return NULL;
	}
	rc = rc && (BOOLEAN) pvalue(val1);

#ifdef DEBUG
	llwprintf("rc == %d\n", rc);
#endif

	delete_pvalue(val1);
	while ((arg = inext(arg))) {
		if (rc) {
			val2 = eval_and_coerce(PBOOL, arg, stab, eflg);
			if (*eflg) {

#ifdef DEBUG
	show_pvalue(val2);
#endif
				prog_error(node, "an arg to and is not boolean");
				return NULL;
			}
			rc = rc && (BOOLEAN) pvalue(val2);
			delete_pvalue(val2);
		}
	}
	return create_pvalue(PBOOL, (WORD)rc);
}
/*================================+
 * __or -- Or operation
 *   usage: or(ANY [,ANY]+) -> BOOL
 *===============================*/
PVALUE
__or (PNODE node,
      TABLE stab,
      BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	BOOLEAN rc = FALSE;
	PVALUE val2, val1 = eval_and_coerce(PBOOL, arg, stab, eflg);
	if (*eflg) {
		prog_error(node, "an arg to or is not boolean");
		return NULL;
	}
	rc = rc || (BOOLEAN) pvalue(val1);
	delete_pvalue(val1);
	while ((arg = inext(arg))) {
		if (!rc) {
			val2 = eval_and_coerce(PBOOL, arg, stab, eflg);
			if (*eflg) {
				prog_error(node, "an arg to or is not boolean");
				return NULL;
			}
			rc = rc || (BOOLEAN) pvalue(val2);
			delete_pvalue(val2);
		}
	}
	return create_pvalue(PBOOL, (WORD)rc);
}
/*================================+
 * __add -- Add operation
 *   usage: add(INT [,INT]+) -> INT
 *===============================*/
PVALUE
__add (PNODE node,
       TABLE stab,
       BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	PVALUE val2, val1 = evaluate(arg, stab, eflg);
	if (*eflg) return NULL;
	while ((arg = inext(arg))) {
		val2 = evaluate(arg, stab, eflg);
		if (*eflg) return NULL;
		add_pvalues(val1, val2, eflg);
		if (*eflg) {
			prog_error(node, "incorrect operands for add");
			return NULL;
		}
	}
	return val1;
}
/*=============================+
 * __sub -- Subtract operation
 *   usage: sub(INT, INT) -> INT
 *============================*/
PVALUE
__sub (PNODE node,
       TABLE stab,
       BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	PVALUE val2, val1 = evaluate(arg, stab, eflg);
	if (*eflg) return NULL;
	val2 = evaluate(inext(arg), stab, eflg);
	if (*eflg) return NULL;
	sub_pvalues(val1, val2, eflg);
	if (*eflg) {
		prog_error(node, "incorrect operands for sub");
		return NULL;
	}
	return val1;
}
/*================================+
 * __mul -- Multiply operation
 *   usage: mul(INT [,INT]+) -> INT
 *===============================*/
PVALUE
__mul (PNODE node,
       TABLE stab,
       BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	PVALUE val2, val1 = evaluate(arg, stab, eflg);
	if (*eflg) return NULL;
	while ((arg = inext(arg))) {
		val2 = evaluate(arg, stab, eflg);
		if (*eflg) return NULL;
		mul_pvalues(val1, val2, eflg);
		if (*eflg) {
			prog_error(node, "incorrect operands for mul");
			return NULL;
		}
	}
	return val1;
}
/*=============================+
 * __div -- Divide operation
 *   usage: div(INT, INT) -> INT
 *============================*/
PVALUE
__div (PNODE node,
       TABLE stab,
       BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	PVALUE val2, val1 = evaluate(arg, stab, eflg);
	if (*eflg) return NULL;
	val2 = evaluate(inext(arg), stab, eflg);
	if (*eflg) return NULL;
	div_pvalues(val1, val2, eflg);
	if (*eflg) {
		prog_error(node, "incorrect operands for div");
		return NULL;
	}
	return val1;
}
/*=============================+
 * __mod -- Modulus operation
 *   usage: mod(INT, INT) -> INT
 *============================*/
PVALUE
__mod (PNODE node,
       TABLE stab,
       BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	PVALUE val2, val1 = evaluate(arg, stab, eflg);
	if (*eflg) return NULL;
	val2 = evaluate(inext(arg), stab, eflg);
	if (*eflg) return NULL;
	mod_pvalues(val1, val2, eflg);
	if (*eflg) {
		prog_error(node, "incorrect operands for mod");
		return NULL;
	}
	return val1;
}
/*=================================+
 * __exp -- Exponentiation operation
 *   usage: exp(INT, INT) -> INT
 *================================*/
PVALUE
__exp (PNODE node,
       TABLE stab,
       BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	PVALUE val2, val1 = evaluate(arg, stab, eflg);
	if (*eflg) return NULL;
	val2 = evaluate(inext(arg), stab, eflg);
	if (*eflg) return NULL;
	exp_pvalues(val1, val2, eflg);
	if (*eflg) {
		prog_error(node, "incorrect operands for exp");
		return NULL;
	}
	return val1;
}
/*===========================+
 * __neg -- Negation operation
 *   usage: neg(INT) -> INT
 *==========================*/
PVALUE
__neg (PNODE node,
       TABLE stab,
       BOOLEAN *eflg)
{
	PVALUE val = evaluate(iargs(node), stab, eflg);
	if (*eflg) return NULL;
	neg_pvalue(val, eflg);
	return val;
}
/*===========================+
 * __incr -- Increment variable
 *   usage: incr(VARB) -> VOID
 *==========================*/
PVALUE
__incr (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	PNODE var = (PNODE) iargs(node);
	PVALUE val;
	*eflg = TRUE;
	if (!iistype(var, IIDENT)) {
		prog_error(node, "arg to incr must be a variable");
		return NULL;
	}
	*eflg = FALSE;
	val = evaluate(var, stab, eflg);
	if (*eflg || !val) {
		prog_error(node, "arg to incr is in error");
		*eflg = TRUE;
		return NULL;
	}
	incr_pvalue(val, eflg);
	if (*eflg) {
		prog_error(node, "arg to incr is not numeric");
		*eflg = TRUE;
		return NULL;
	}
	assign_iden(stab, iident(var), val);
	return NULL;
}
/*============================+
 * __decr -- Decrement variable
 *   usage: decr(VARB) -> VOID
 *===========================*/
PVALUE
__decr (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	PNODE var = (PNODE) iargs(node);
	PVALUE val;
	*eflg = TRUE;
	if (!iistype(var, IIDENT)) {
		prog_error(node, "arg to decr must be a variable");
		return NULL;
	}
	*eflg = FALSE;
	val = evaluate(var, stab, eflg);
	if (*eflg || !val) {
		prog_error(node, "arg to decr is in error");
		*eflg = TRUE;
		return NULL;
	}
	decr_pvalue(val, eflg);
	if (*eflg) {
		prog_error(node, "arg to decr is not numeric");
		*eflg = TRUE;
		return NULL;
	}
	assign_iden(stab, iident(var), val);
	return NULL;
}
/*======================================+
 * __strcmp -- Compare two strings
 *   usage: strcmp(STRING, STRING) -> INT
 *=====================================*/
PVALUE
__strcmp (PNODE node,
          TABLE stab,
          BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	STRING str1, str2, emp = (STRING) "";
	PVALUE val2, val1 = eval_and_coerce(PSTRING, arg, stab, eflg);
	if (*eflg) {
		prog_error(node, "1st arg to strcmp is not a string");
		return NULL;
	}
	val2 = eval_and_coerce(PSTRING, inext(arg), stab, eflg);
	if (*eflg) {
		prog_error(node, "2nd arg to strcmp is not a string");
		return NULL;
	}
	str1 = (STRING) pvalue(val1);
	str2 = (STRING) pvalue(val2);
	if (!str1) str1 = emp;
	if (!str2) str2 = emp;

#ifdef DEBUG
	llwprintf("__strcmp: ");
	show_pvalue(val1);
	llwprintf(" ");
	show_pvalue(val2);
	llwprintf("\n");
#endif

	set_pvalue(val1, PINT, (WORD)ll_strcmp(str1, str2));
	delete_pvalue(val2);
	return val1;
}
/*=========================================+
 * __nestr -- Compare two strings
 *   usage: nestr(STRING, STRING) -> BOOLEAN
 *========================================*/
PVALUE
__nestr (PNODE node,
         TABLE stab,
         BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	STRING str1, str2, emp = (STRING) "";
	PVALUE val2, val1 = eval_and_coerce(PSTRING, arg, stab, eflg);
	if (*eflg) {
		prog_error(node, "1st arg to nestr is not a string");
		return NULL;
	}
	val2 = eval_and_coerce(PSTRING, inext(arg), stab, eflg);
	if (*eflg) {
		prog_error(node, "2nd arg to nestr is not a string");
		return NULL;
	}
	str1 = (STRING) pvalue(val1);
	str2 = (STRING) pvalue(val2);
	if (!str1) str1 = emp;
	if (!str2) str2 = emp;
	set_pvalue(val1, PBOOL, (WORD)(ll_strcmp(str1, str2) != 0));
	delete_pvalue(val2);
	return val1;
}
/*=========================================+
 * __eqstr -- Compare two strings
 *   usage: eqstr(STRING, STRING) -> BOOLEAN
 *========================================*/
PVALUE
__eqstr (PNODE node,
         TABLE stab,
         BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	STRING str1, str2, emp = (STRING) "";
	PVALUE val2, val1 = eval_and_coerce(PSTRING, arg, stab, eflg);
	if (*eflg) {
		prog_error(node, "1st arg to eqstr is not a string");
		return NULL;
	}
	val2 = eval_and_coerce(PSTRING, inext(arg), stab, eflg);
	if (*eflg) {
		prog_error(node, "2nd arg to eqstr is not a string");
		return NULL;
	}
	str1 = (STRING) pvalue(val1);
	str2 = (STRING) pvalue(val2);
	if (!str1) str1 = emp;
	if (!str2) str2 = emp;
	set_pvalue(val1, PBOOL, (WORD)(ll_strcmp(str1, str2) == 0));
	delete_pvalue(val2);
	return val1;
}
/*=======================================+
 * __strtoint -- Convert string to integer
 *  usage: strtoint(STRING) -> INT
 *======================================*/
PVALUE
__strtoint (PNODE node,
            TABLE  stab,
            BOOLEAN *eflg)
{
	PVALUE val;
	val = eval_and_coerce(PSTRING, iargs(node), stab, eflg);
	if (*eflg || !val || ptype(val) != PSTRING) {
		*eflg = TRUE;
		prog_error(node, "the arg to strtoint is not a string");
		return NULL;
	}
	if (!pvalue(val))
		set_pvalue(val, PINT, 0);
	else
		set_pvalue(val, PINT, (WORD)atoi(pvalue(val)));
	return val;
}
/*============================+
 * __list -- Create list
 *   usage: list(IDENT) -> VOID
 *===========================*/
PVALUE
__list (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	LIST list;
	PNODE var = (PNODE) iargs(node);
	if (!iistype(var, IIDENT)) {
		*eflg = TRUE;
		prog_error(node, "the arg to list is not a variable");
		return NULL;
	}
	*eflg = FALSE;
	list = create_list();
	assign_iden(stab, iident(var), create_pvalue(PLIST, (WORD) list));
	return NULL;
}
/*=======================================+
 * __push -- Push element on front of list
 *   usage: push(LIST, ANY) -> VOID
 *======================================*/
PVALUE
__push (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	LIST list;
	PVALUE el;
	PVALUE val = eval_and_coerce(PLIST, arg, stab, eflg);
	if (*eflg || !val || ptype(val) != PLIST) {
		*eflg = TRUE;
		prog_error(node, "1st arg to push is not a list");
		return NULL;
	}
	el = evaluate(inext(arg), stab, eflg);
	if (*eflg || !el) {
		*eflg = TRUE;
		prog_error(node, "2nd arg to push is in error");
		return NULL;
	}
	list = (LIST) pvalue(val);
	delete_pvalue(val);
	push_list(list, el);
	return NULL;
}
/*======================================+
 * __inlist -- see if element is in list
 *   usage: inlist(LIST, STRING) -> BOOL
 *=====================================*/
PVALUE
__inlist (PNODE node,
          TABLE stab,
          BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	LIST list;
	PVALUE el;
	PVALUE val = eval_and_coerce(PLIST, arg, stab, eflg);
	if (*eflg || !val || ptype(val) != PLIST) {
		*eflg = TRUE;
		prog_error(node, "1st arg to inlist is not a list");
		return NULL;
	}
	el = evaluate(inext(arg), stab, eflg);
	if (*eflg || !el) {
		*eflg = TRUE;
		prog_error(node, "2nd arg to inlist is in error");
		return NULL;
	}
	list = (LIST) pvalue(val);
	set_pvalue(val, PBOOL, (WORD)in_list(list, el, eqv_pvalues));
	delete_pvalue(el);
	return val;
}
/*====================================+
 * __enqueue -- Enqueue element on list
 *   usage: enqueue(LIST, ANY) -> VOID
 *===================================*/
PVALUE
__enqueue (PNODE node,
           TABLE stab,
           BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	LIST list;
	PVALUE el;
	PVALUE val = eval_and_coerce(PLIST, arg, stab, eflg);
	if (*eflg || !val || ptype(val) != PLIST) {
		*eflg = TRUE;
		prog_error(node, "1st arg to enqueue is not a list");
		return NULL;
	}
	el = evaluate(inext(arg), stab, eflg);
	if (*eflg || !el) {
		*eflg = TRUE;
		prog_error(node, "2nd arg to enqueue is in error");
		return NULL;
	}
	list = (LIST) pvalue(val);
	delete_pvalue(val);
	push_list(list, el);
	return NULL;
}
/*========================================+
 * __requeue -- Add element to back of list
 *   usage: requeue(LIST, ANY) -> VOID
 *=======================================*/
PVALUE
__requeue (PNODE node,
           TABLE stab,
           BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	LIST list;
	PVALUE el;
	PVALUE val = eval_and_coerce(PLIST, arg, stab, eflg);
	if (*eflg || !val || ptype(val) != PLIST) {
		*eflg = TRUE;
		prog_error(node, "1st arg to requeue is not a list");
		return NULL;
	}
	el = evaluate(inext(arg), stab, eflg);
	if (*eflg || !el) {
		*eflg = TRUE;
		prog_error(node, "2nd arg to requeue is in error");
		return NULL;
	}
	list = (LIST) pvalue(val);
	delete_pvalue(val);
	back_list(list, el);
	return NULL;
}
/*=======================================+
 * __pop -- Pop element from front of list
 *   usage: pop(LIST) -> ANY
 *======================================*/
PVALUE
__pop (PNODE node,
       TABLE stab,
       BOOLEAN  *eflg)
{
	LIST list;
	PVALUE val = eval_and_coerce(PLIST, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to pop is not a list");
		return NULL;
	}
	list = (LIST) pvalue(val);
	delete_pvalue(val);
	if (empty_list(list)) return create_pvalue(PANY, NULL);
	return (PVALUE) pop_list(list);
}
/*=============================================+
 * __dequeue -- Remove element from back of list
 *   usage dequeue(LIST) -> ANY
 *============================================*/
PVALUE
__dequeue (PNODE node,
           TABLE stab,
           BOOLEAN *eflg)
{
	LIST list;
	PVALUE val = eval_and_coerce(PLIST, iargs(node), stab, eflg);
	if (*eflg || !val || ptype(val) != PLIST) {
		*eflg = TRUE;
		prog_error(node, "the arg to pop is not a list");
		return NULL;
	}
	list = (LIST) pvalue(val);
	delete_pvalue(val);
	val = (PVALUE) dequeue_list(list);
	if (!val) return create_pvalue(PANY, NULL);
	return val;
}
/*=================================+
 * __empty -- Check if list is empty
 *   usage: empty(LIST) -> BOOL
 *================================*/
PVALUE
__empty (PNODE node,
         TABLE stab,
         BOOLEAN *eflg)
{
	LIST list;
	PVALUE val = eval_and_coerce(PLIST, iargs(node), stab, eflg);
	if (*eflg || !val || ptype(val) != PLIST) {
		*eflg = TRUE;
		prog_error(node, "the arg to empty is not a list");
		return NULL;
	}
	list = (LIST) pvalue(val);
	set_pvalue(val, PBOOL, (WORD)empty_list(list));
	return val;
}
/*==================================+
 * __getel -- Get nth value from list
 *   usage: getel(LIST, INT) -> ANY
 *=================================*/
PVALUE
__getel (PNODE node,
         TABLE stab,
         BOOLEAN *eflg)
{
	LIST list;
	INT ind;
	PNODE arg = (PNODE) iargs(node);
	PVALUE val = eval_and_coerce(PLIST, arg, stab, eflg);
	if (*eflg || !val || ptype(val) != PLIST) {
		*eflg = TRUE;
		prog_error(node, "1st arg to getel is not a list");
		return NULL;
	}
	list = (LIST) pvalue(val);
	delete_pvalue(val);
	val = eval_and_coerce(PINT, inext(arg), stab, eflg);
	if (*eflg || !val || ptype(val) != PINT) {
		*eflg = TRUE;
		prog_error(node, "2nd arg to getel is not an integer");
		return NULL;
	}
	ind = (INT) pvalue(val);
	delete_pvalue(val);
	if (!(val = (PVALUE) get_list_element(list, ind)))
		return create_pvalue(PANY, 0);
	return copy_pvalue(val);
}
/*=======================================+
 * __setel -- Set nth value in list
 *   usage: setel(LIST, INT, ANY) -> VOID
 *======================================*/
PVALUE
__setel (PNODE node,
         TABLE stab,
         BOOLEAN *eflg)
{
	LIST list;
	INT ind;
	PNODE arg = (PNODE) iargs(node);
	PVALUE old, val = eval_and_coerce(PLIST, arg, stab, eflg);
	if (*eflg) {
		prog_error(node, "1st arg to setel is not a list");
		return NULL;
	}
	list = (LIST) pvalue(val);
	delete_pvalue(val);
	arg = inext(arg);
	val = eval_and_coerce(PINT, arg, stab, eflg);
	if (*eflg) {
		prog_error(node, "2nd arg to setel is not an integer");
		return NULL;
	}
	ind = (INT) pvalue(val);
	delete_pvalue(val);
	val = evaluate(inext(arg), stab, eflg);
	if (*eflg || !val) {
		*eflg = TRUE;
		prog_error(node, "3rd arg to setel is in error");
		return NULL;
	}
	old = (PVALUE) get_list_element(list, ind);
	if(old) delete_pvalue(old);
	set_list_element(list, ind, val);
	return NULL;
}
/*===============================+
 * __length -- Find length of list
 *   usage: length(LIST) -> INT
 *==============================*/
PVALUE
__length (PNODE node,
          TABLE stab,
          BOOLEAN *eflg)
{
	LIST list;
	PVALUE val = eval_and_coerce(PLIST, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to length is not a list");
		return NULL;
	}
	list = (LIST) pvalue(val);
	set_pvalue(val, PINT, (WORD)length_list(list));
	return val;
}
/*==========================+
 * __not -- Not operation
 *   usage: not(BOOL) -> BOOL
 *=========================*/
PVALUE
__not (PNODE node,
       TABLE stab,
       BOOLEAN *eflg)
{
	PVALUE val = eval_and_coerce(PBOOL, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to not is not boolean");
		return NULL;
	}
	set_pvalue(val, PBOOL, (WORD)!((BOOLEAN) pvalue(val)));
	return val;
}
/*===============================+
 * __save -- Copy string
 *   usage: save(STRING) -> STRING
 *==============================*/
PVALUE
__save (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	PVALUE val = eval_and_coerce(PSTRING, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to save is not a string");
		return NULL;
	}
	return val;
}
/*=================================+
 * __strlen -- Find length of string
 *   usage: strlen(STRING) -> INT
 *================================*/
PVALUE
__strlen (PNODE node,
          TABLE stab,
          BOOLEAN *eflg)
{
	PVALUE val = eval_and_coerce(PSTRING, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to strlen must be a string");
		return NULL;
	}
	if (pvalue(val))
		set_pvalue(val, PINT, (WORD)strlen(pvalue(val)));
	else
		set_pvalue(val, PINT, 0);
	return val;
}
/*=============================================+
 * __concat -- Catenate strings
 *   usage: concat(STRING [, STRING]+) -> STRING
 *============================================*/
PVALUE
__concat (PNODE node,
          TABLE stab,
          BOOLEAN *eflg)
{
        PNODE arg = (PNODE) iargs(node);
        INT len = 0, i, nstrs = 0;
        STRING hold[32];
        STRING p, new, str;
	PVALUE val;

        while (arg) {
                val = eval_and_coerce(PSTRING, arg, stab, eflg);
                if (*eflg) {
			prog_error(node, "an arg to concat is not a string");
			return NULL;
		}
		if ((str = (STRING) pvalue(val))) {
                        len += strlen(str);

#ifdef DEBUG
	llwprintf("concat: str: ``%s'' ", str);
#endif

                        hold[nstrs++] = strsave(str);
                } else
                        hold[nstrs++] = NULL;
                arg = inext(arg);
		delete_pvalue(val);
        }
        p = new = (STRING) stdalloc(len + 1);
        for (i = 0; i < nstrs; i++) {
                str = hold[i];
                if (str) {
                        strcpy(p, str);
                        p += strlen(p);
                        stdfree(str);
                }
        }
	val = create_pvalue(PSTRING, (WORD)new);
	stdfree(new);
        return val;
}
/*=======================================+
 * __lower -- Convert string to lower case
 *   usage: lower(STRING) -> STRING
 *======================================*/
PVALUE
__lower (PNODE node,
         TABLE stab,
         BOOLEAN *eflg)
{
	PVALUE val = eval_and_coerce(PSTRING, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to lower must be a string");
		return NULL;
	}
	set_pvalue(val, PSTRING, (WORD)lower(pvalue(val)));
	return val;
}
/*=======================================+
 * __upper -- Convert string to upper case
 *   usage: upper(STRING) -> STRING
 *======================================*/
PVALUE
__upper (PNODE node,
         TABLE stab,
         BOOLEAN *eflg)
{
	PVALUE val = eval_and_coerce(PSTRING, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to upper must be a string");
		return NULL;
	}
	set_pvalue(val, PSTRING, (WORD)upper(pvalue(val)));
	return val;
}
/*=====================================+
 * __capitalize -- Capitalize string
 *   usage: capitalize(STRING) -> STRING
 *====================================*/
PVALUE
__capitalize (PNODE node,
              TABLE stab,
              BOOLEAN *eflg)
{
	PVALUE val = eval_and_coerce(PSTRING, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to capitalize must be a string");
		return NULL;
	}
	set_pvalue(val, PSTRING, (WORD)capitalize(pvalue(val)));
	return val;
}
/*================================+
 * __pn -- Generate pronoun
 *   usage: pn(INDI, INT) -> STRING
 *===============================*/
static char *mpns[] = {  "He",  "he", "His", "his", "him" };
static char *fpns[] = { "She", "she", "Her", "her", "her" };
PVALUE
__pn (PNODE node,
      TABLE stab,
      BOOLEAN *eflg)
{
	INT typ;
	PVALUE val;
	PNODE arg = (PNODE) iargs(node);
	NODE indi = eval_indi(arg, stab, eflg, NULL);
	if (*eflg || !indi) {
		*eflg = TRUE;
		prog_error(node, "1st arg to pn must be a person");
		return NULL;
	}
	val = eval_and_coerce(PINT, inext(arg), stab, eflg);
	typ = (INT) pvalue(val);
	if (*eflg || typ < 0 || typ > 4) {
		*eflg = TRUE;
		prog_error(node, "2nd arg to pn must be between 0 and 4");
		return NULL;
	}
	if (SEX(indi) == SEX_FEMALE) 
		set_pvalue(val, PSTRING, (WORD)fpns[typ]);
	else
		set_pvalue(val, PSTRING, (WORD)mpns[typ]);
	return val;
}
/*==================================+
 * __print -- Print to stdout window
 *   usage: print([STRING]+,) -> VOID
 *=================================*/
PVALUE
__print (PNODE node,
         TABLE stab,
         BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	PVALUE val;
	while (arg) {
		val = evaluate(arg, stab, eflg);
		if (*eflg || !val || ptype(val) != PSTRING) {
			*eflg = TRUE;
			prog_error(node, "all args to print must be strings");
			return NULL;
		}
		if (pvalue(val)) llwprintf("%s", (STRING) pvalue(val));
		delete_pvalue(val);
		arg = inext(arg);
	}
	return NULL;
}
/*=================================================+
 * __sex -- Find sex, as string M, F or U, of person
 *   usage: sex(INDI) -> STRING
 *================================================*/
PVALUE
__sex (PNODE node,
       TABLE stab,
       BOOLEAN *eflg)
{
	STRING str = (STRING) "U";
	INT sex;
	NODE indi = eval_indi(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "the arg to sex is not a person");
		return NULL;
	}
	if (!indi) return create_pvalue(PSTRING, (WORD)str);
	if ((sex = SEX(indi)) == SEX_MALE) str = (STRING) "M";
	else if (sex == SEX_FEMALE) str = (STRING) "F";
	return create_pvalue(PSTRING, (WORD)str);
}
/*=================================+
 * __male -- Check if person is male
 *   usage: male(INDI) -> BOOL
 *================================*/
PVALUE
__male (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	NODE indi = eval_indi(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "the arg to male is not a person");
		return NULL;
	}
	if (!indi) return create_pvalue(PBOOL, FALSE);
	return create_pvalue(PBOOL, (WORD)(SEX(indi) == SEX_MALE));
}
/*=====================================+
 * __female -- Check if person is female
 *   usage: female(INDI) -> BOOL
 *====================================*/
PVALUE
__female (PNODE node,
          TABLE stab,
          BOOLEAN *eflg)
{
	NODE indi = eval_indi(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "the arg to female is not a person");
		return NULL;
	}
	if (!indi) return create_pvalue(PBOOL, FALSE);
	return create_pvalue(PBOOL, (WORD)(SEX(indi) == SEX_FEMALE));
}
/*========================================+
 * __key -- Return person or family key
 *   usage: key(INDI|FAM [,BOOL]) -> STRING
 *=======================================*/
PVALUE
__key (PNODE node,
       TABLE stab,
       BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	PVALUE val = evaluate(arg, stab, eflg);
	CACHEEL cel;
	BOOLEAN strip = FALSE;
	STRING key;
	if (*eflg || !val || !is_record_pvalue(val)) {
		*eflg = TRUE;
		prog_error(node, "1st arg to key is not a GEDCOM record");
		return NULL;
	}
	cel = (CACHEEL) pvalue(val);
	delete_pvalue(val);
	if (!cel) return create_pvalue(PSTRING, NULL);
	if (inext(arg)) {
		val = eval_and_coerce(PBOOL, inext(arg), stab, eflg);
		if (*eflg) {
			prog_error(node, "2nd arg to key is not boolean");
			return NULL;
		}
		strip = (BOOLEAN) pvalue(val);
		delete_pvalue(val);
	}
	key = (STRING) ckey(cel);
	return create_pvalue(PSTRING, (WORD)(strip ? key + 1 : key));
}
/*==============================================+
 * __root -- Return root of cached record
 *   usage: root(INDI|FAM|EVEN|SOUR|OTHR) -> NODE
 *=============================================*/
PVALUE
__rot (PNODE node,
       TABLE stab,
       BOOLEAN *eflg)
{
	STRING key;
 	PVALUE val = evaluate(iargs(node), stab, eflg);
 	CACHEEL cel;
	NODE gnode=0;
	if (*eflg || !val || !is_record_pvalue(val)) {
		*eflg = TRUE;
		prog_error(node, "the arg to root is not a record");
		return NULL;
	}
	cel = (CACHEEL) pvalue(val);
	if (cnode(cel)) {
		set_pvalue(val, PGNODE, (WORD)cnode(cel));
		return val;
	}
	key = ckey(cel);
	switch (*key) {
	case 'I': gnode = key_to_indi(key); break;
	case 'F': gnode = key_to_fam(key);  break;
	case 'E': gnode = key_to_even(key); break;
	case 'S': gnode = key_to_sour(key); break;
	case 'X': gnode = key_to_othr(key); break;
	default:  FATAL();
	}
	set_pvalue(val, PGNODE, (WORD)gnode); 
	return val;
}
/*================================+
 * __inode -- Return root of person
 *   usage: inode(INDI) -> NODE
 *==============================*/
PVALUE
__inode (PNODE node,
         TABLE stab,
         BOOLEAN *eflg)
{
	NODE indi = eval_indi(iargs(node), stab, eflg, NULL);
	if (*eflg || !indi) {
		*eflg = TRUE;
		prog_error(node, "the arg to inode is not a person");
		return NULL;
	}
	return create_pvalue(PGNODE, (WORD)indi);
}
/*================================+
 * __fnode -- Return root of family
 *   usage: fnode(FAM) -> NODE
 *===============================*/
PVALUE
__fnode (PNODE node,
         TABLE stab,
         BOOLEAN *eflg)
{
	NODE fam = eval_fam(iargs(node), stab, eflg, NULL);
	if (*eflg) {
		prog_error(node, "the arg to fnode is not a family");
		return NULL;
	}
	if (!fam) return create_pvalue(PGNODE, NULL);
	return create_pvalue(PGNODE, (WORD)fam);
}
/*=============================+
 * __table -- Create table
 *   usage: table(IDENT) -> VOID
 *============================*/
PVALUE
__table (PNODE node,
         TABLE stab,
         BOOLEAN *eflg)
{
	TABLE tab;
	PVALUE val;
	PNODE var = (PNODE) iargs(node);
	*eflg = TRUE;
	if (!iistype(var, IIDENT)) {
		prog_error(node, "the arg to table must be a variable");
		return NULL;
	}
	*eflg = FALSE;
	tab = create_table();
	val = create_pvalue(PTABLE, (WORD)tab);

#ifdef DEBUG
	llwprintf("__table: ");show_pvalue(val);wprintf("\n");
#endif

	assign_iden(stab, iident(var), val);
	return NULL;
}
/*=========================================+
 * __insert -- Add element to table
 *   usage: insert(TAB, STRING, ANY) -> VOID
 *========================================*/
PVALUE
__insert (PNODE node,
          TABLE stab,
          BOOLEAN *eflg)
{
    	BOOLEAN there;
	PNODE arg = (PNODE) iargs(node);
	PVALUE old, val = eval_and_coerce(PTABLE, arg, stab, eflg);
	TABLE tab;
	STRING str;

#ifdef DEBUG
	llwprintf("__insert:\n");
#endif

	if (*eflg || (pvalue(val) == NULL)) {
	        *eflg = TRUE;
		prog_error(node, "1st arg to insert is not a table but %s",
		           pvalue_to_string(val));
		return NULL;
	}
	tab = (TABLE) pvalue(val);

#ifdef DEBUG
	show_pvalue(val);
	llwprintf(" ");
#endif

	delete_pvalue(val);
	arg = inext(arg);
	val = eval_and_coerce(PSTRING, arg, stab, eflg);
	if (*eflg || !val || !pvalue(val)) {
		*eflg = TRUE;
		prog_error(node, "2nd arg to insert is not a string but %s",
		           pvalue_to_string(val));
		return NULL;
	}
	str = strsave((STRING) pvalue(val));
	delete_pvalue(val);

#ifdef DEBUG
	show_pvalue(val);
	llwprintf(" ");
#endif

	val = evaluate(inext(arg), stab, eflg);
	if (*eflg || !val) {
		*eflg = TRUE;
		prog_error(node, "3rd arg to insert is in error");
		return NULL;
	}

#ifdef DEBUG
	show_pvalue(val);
	llwprintf("\n");
#endif

	old = (PVALUE) valueofbool(tab, str, &there);
	if(there && old) delete_pvalue(old);
	insert_table(tab, str, val);
	if(there) stdfree(str);	/* key is already in table. free this one */
	return NULL;
}
/*====================================+
 * __lookup -- Look up element in table
 *   usage: lookup(TAB, STRING) -> ANY
 *===================================*/
PVALUE
__lookup (PNODE node,
          TABLE stab,
          BOOLEAN *eflg)
{
	PNODE arg;
	PVALUE new, val;
	TABLE tab;
	STRING str;

#ifdef DEBUG
	llwprintf("lookup called\n");
#endif

	arg = (PNODE) iargs(node);
	val = eval_and_coerce(PTABLE, arg, stab, eflg);
	if (*eflg || (pvalue(val) == NULL)) {
	    	*eflg = TRUE;
		prog_error(node, "1st arg to lookup is not a table");
		return NULL;
	}
	tab = (TABLE) pvalue(val);
	delete_pvalue(val);
	val = eval_and_coerce(PSTRING, inext(arg), stab, eflg);
	if (*eflg) {
		prog_error(node, "2nd arg to lookup is not a string");
		return NULL;
	}
	str = (STRING) pvalue(val);
	new = (PVALUE) valueof(tab, str);
	delete_pvalue(val);
	new = (new ? copy_pvalue(new) : create_pvalue(PANY, NULL));
#if 0
	if (prog_debug) {
		llwprintf("lookup: new =");
		show_pvalue(new);
		llwprintf("\n");
	}
#endif
	return new;
}
/*====================================+
 * __trim -- Trim string if too long
 *   usage: trim(STRING, INT) -> STRING
 *===================================*/
PVALUE
__trim (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	STRING str;
	PVALUE val1, val2;
	INT len;
        val1 = eval_and_coerce(PSTRING, arg, stab, eflg);
	if (*eflg) {
		prog_error(node, "1st arg to trim is not a string");
		return NULL;
	}
	val2 = eval_and_coerce(PINT, inext(arg), stab, eflg);
	if (*eflg) {
		prog_error(node, "2nd arg to trim is not an integer");
		return NULL;
	}
	str = (STRING) pvalue(val1);
	len = (INT) pvalue(val2);
	set_pvalue(val2, PSTRING, (WORD)trim(str, len));
	delete_pvalue(val1);
	return val2;
}
/*======================================+
 * __trimname -- Trim name if too long
 *   usage: trimname(INDI, INT) -> STRING
 *=====================================*/
PVALUE
__trimname (PNODE node,
            TABLE stab,
            BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	INT len;
	PVALUE val;
	NODE indi = eval_indi(arg, stab, eflg, (CACHEEL *) NULL);
	if (*eflg) {
		prog_error(node, "1st arg to trimname is not a person");
		return NULL;
	}
	if (!indi) return create_pvalue(PSTRING, NULL);
	if (!(indi = NAME(indi)) || !nval(indi)) {
		*eflg = TRUE;
		prog_error(node, "1st art to trimname is in error");
		return NULL;
	}
	*eflg = FALSE;
	val = eval_and_coerce(PINT, inext(arg), stab, eflg);
	if (*eflg) {
		prog_error(node, "2nd arg to trimname is not an integer");
		return NULL;
	}
	len = (INT) pvalue(val);
	set_pvalue(val, PSTRING, (WORD)name_string(trim_name(nval(indi), len)));
	return val;
}
/*==============================+
 * __date -- Return date of event
 *   usage: date(EVENT) -> STRING
 *=============================*/
PVALUE
__date (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	NODE line;
	TRANTABLE ttr = tran_tables[MINRP];
	PVALUE val = eval_and_coerce(PGNODE, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to date must be a record line");
		return NULL;
	}
	line = (NODE) pvalue(val);
	return create_pvalue(PSTRING, (WORD)event_to_date(line, ttr, FALSE));
}
/*=====================================================+
 * __extractdate -- Extract date from EVENT or DATE NODE
 *   usage: extractdate(NODE, VARB, VARB, VARB) -> VOID
 *====================================================*/
PVALUE
__extractdate (PNODE node,
               TABLE stab,
               BOOLEAN *eflg)
{
	STRING str, syr;
	NODE line;
	INT mod, da = 0, mo = 0, yr = 0;
	PNODE arg = (PNODE) iargs(node);
	PVALUE val = eval_and_coerce(PGNODE, arg, stab, eflg);
	PNODE dvar = inext(arg);
	PNODE mvar = inext(dvar);
	PNODE yvar = inext(mvar);
	if (*eflg) {
		prog_error(node, "1st arg to extractdate is not a record line");
		return NULL;
	}
	line = (NODE) pvalue(val);
	*eflg = TRUE;
	if (!iistype(dvar, IIDENT)) {
		prog_error(node, "2nd arg to extractdate must be a variable");
		return NULL;
	}
	if (!iistype(mvar, IIDENT)) {
		prog_error(node, "3rd arg to extractdate must be a variable");
		return NULL;
	}
	if (!iistype(yvar, IIDENT)) {
		prog_error(node, "4th arg to extractdate must be a variable");
		return NULL;
	}
	if (nestr("DATE", ntag(line)))
		str = event_to_date(line, NULL, FALSE);
	else
		str = nval(line);
	extract_date(str, &mod, &da, &mo, &yr, &syr);
	assign_iden(stab, iident(dvar), create_pvalue(PINT, (WORD)da));
	assign_iden(stab, iident(mvar), create_pvalue(PINT, (WORD)mo));
	assign_iden(stab, iident(yvar), create_pvalue(PINT, (WORD)yr));
	*eflg = FALSE;
	return NULL;
}
/*==================================================================+
 * __extractdatestr -- Extract date from STRING
 *   usage: extractdatestr(VARB, VARB, VARB, VARB, VARB[, STRING]) -> VOID
 *==================================================================*/
PVALUE
__extractdatestr (PNODE node,
                  TABLE stab,
                  BOOLEAN *eflg)
{
	STRING str = NULL, yrstr;
	INT mod, da, mo, yr;
	PVALUE val;
	PNODE date;
	PNODE modvar = (PNODE) iargs(node);
	PNODE dvar = inext(modvar);
	PNODE mvar = inext(dvar);
	PNODE yvar = inext(mvar);
	PNODE ystvar = inext(yvar);
	*eflg = TRUE;
	if (!iistype(modvar, IIDENT)) {
		prog_error(node, "1st arg to extractdatestr must be a variable");
		return NULL;
	}
	if (!iistype(dvar, IIDENT)) {
		prog_error(node, "2nd arg to extractdatestr must be a variable");
		return NULL;
	}
	if (!iistype(mvar, IIDENT)) {
		prog_error(node, "3rd arg to extractdatestr must be a variable");
		return NULL;
	}
	if (!iistype(yvar, IIDENT)) {
		prog_error(node, "4th arg to extractdatestr must be a variable");
		return NULL;
	}
	if (!iistype(ystvar, IIDENT)) {
		prog_error(node, "5th arg to extractdatestr must be a variable");
		return NULL;
	}
	if ((date = inext(ystvar))) {
		val = evaluate(date, stab, eflg);
		if (*eflg) return NULL;
		if (ptype(val) != PSTRING) {
			*eflg = TRUE;
			prog_error(node, "6th arg to extractdatestr must be a string");
			delete_pvalue(val);
			return NULL;
		}
		str = (STRING) pvalue(val);
	}
	extract_date(str, &mod, &da, &mo, &yr, &yrstr);
	assign_iden(stab, iident(modvar), create_pvalue(PINT, (WORD)mod));
	assign_iden(stab, iident(dvar), create_pvalue(PINT, (WORD)da));
	assign_iden(stab, iident(mvar), create_pvalue(PINT, (WORD)mo));
	assign_iden(stab, iident(yvar), create_pvalue(PINT, (WORD)yr));
	assign_iden(stab, iident(ystvar), create_pvalue(PSTRING, (WORD)yrstr));
	*eflg = FALSE;
	return NULL;
}
/*=================================================+
 * __stddate -- Return standard date format of event
 *   usage: stddate(EVENT) -> STRING
 *================================================*/
static INT daycode = 0;
static INT monthcode = 3;
static INT datecode = 0;
PVALUE
__stddate (PNODE node,
           TABLE stab,
           BOOLEAN *eflg)
{
	NODE evnt;
	PVALUE val = eval_and_coerce(PGNODE, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to stddate is not a record line");
		return NULL;
	}
	evnt = (NODE) pvalue(val);
	set_pvalue(val, PSTRING, (WORD)format_date(event_to_date(evnt, NULL, FALSE),
	    daycode, monthcode, 1, datecode, FALSE));
	return val;
}
/*========================================================================+
 * __complexdate -- Return standard date format of event, including modifiers
 *   usage: complexdate(EVENT) -> STRING
 *=======================================================================*/
PVALUE
__complexdate (PNODE node,
               TABLE stab,
               BOOLEAN *eflg)
{
	NODE evnt;
	PVALUE val = eval_and_coerce(PGNODE, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to complexdate is not a record line");
		return NULL;
	}
	evnt = (NODE) pvalue(val);
	set_pvalue(val, PSTRING, (WORD)format_date(event_to_date(evnt, NULL, FALSE),
	    daycode, monthcode, 1, datecode, TRUE));
	return val;
}
/*===============================================+
 * __dayformat -- Set day format for standard date
 *   usage: dayformat(INT) -> NULL
 *==============================================*/
PVALUE
__dayformat (PNODE node,
             TABLE stab,
             BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	INT value;
	PVALUE val = eval_and_coerce(PINT, arg, stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to dayformat is not an integer");
		return NULL;
	}
	value = (INT) pvalue(val);
	delete_pvalue(val);
	if (value < 0) value = 0;
	if (value > 2) value = 2;
	daycode = value;
	return NULL;
}
/*===============================================+
 * __monthformat -- Set month format standard date
 *   usage: monthformat(INT) -> NULL
 *==============================================*/
PVALUE
__monthformat (PNODE node,
               TABLE stab,
               BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	INT value;
	PVALUE val = eval_and_coerce(PINT, arg, stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to monthformat is not an integer");
		return NULL;
	}
	value = (INT) pvalue(val);
	delete_pvalue(val);
	if (value < 0) value = 0;
	if (value > 6) value = 6;
	monthcode = value;
	return NULL;
}
/*=================================================+
 * __dateformat -- Set date format for standard date
 *   usage: dateformat(INT) -> NULL
 *================================================*/
PVALUE
__dateformat (PNODE node,
              TABLE stab,
              BOOLEAN *eflg)
{
	PNODE arg = (PNODE) iargs(node);
	INT value;
	PVALUE val =  eval_and_coerce(PINT, arg, stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to dateformat is not an integer");
		return NULL;
	}
	value = (INT) pvalue(val);
	delete_pvalue(val);
	if (value <  0) value = 0;
	if (value > 11) value = 11;
	datecode = value;
	return NULL;
}
/*==============================+
 * __year -- Return year of event
 *   usage: year(EVENT) -> STRING
 *=============================*/
PVALUE
__year (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	NODE evnt;
	PVALUE val = eval_and_coerce(PGNODE, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to year is not a record line");
		return NULL;
	}
	evnt = (NODE) pvalue(val);
	set_pvalue(val, PSTRING, (WORD)event_to_date(evnt, NULL, TRUE));
	return val;
}
/*================================+
 * __place -- Return place of event
 *   usage: place(EVENT) -> STRING
 *===============================*/
PVALUE
__place (PNODE node,
         TABLE stab,
         BOOLEAN *eflg)
{
	NODE evnt;
	PVALUE val = eval_and_coerce(PGNODE, iargs(node), stab, eflg);

#ifdef DEBUG
	llwprintf("__place: val = ");
	show_pvalue(val);
	llwprintf("\n");
#endif

	if (*eflg) {
		prog_error(node, "the arg to place is not a record line");
		return NULL;
	}
	evnt = (NODE) pvalue(val);
	set_pvalue(val, PSTRING, (WORD)event_to_plac(evnt, FALSE));
	return val;
}
/*============================+
 * __tag -- Return tag of node
 *   usage: tag(NODE) -> STRING
 *===========================*/
PVALUE
__tag (PNODE node,
       TABLE stab,
       BOOLEAN *eflg)
{
	NODE ged;
	PVALUE val = eval_and_coerce(PGNODE, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to tag is not a record line");
		return NULL;
	}
	ged = (NODE) pvalue(val);
	set_pvalue(val, PSTRING, (WORD)(ged ? ntag(ged) : NULL));
	return val;
}
/*===============================+
 * __value -- Return value of node
 *   usage: value(NODE) -> STRING
 *==============================*/
PVALUE
__value (PNODE node,
         TABLE stab,
         BOOLEAN *eflg)
{
	NODE ged;
	PVALUE val = eval_and_coerce(PGNODE, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to value is not a record line");
		return NULL;
	}
	ged = (NODE) pvalue(val);
	set_pvalue(val, PSTRING, (WORD)nval(ged));
	return val;
}
/*=============================+
 * __xref -- Return xref of node
 *   usage: xref(NODE) -> STRING
 *============================*/
PVALUE
__xref (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	NODE ged;
	PVALUE val = eval_and_coerce(PGNODE, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to xref is not a record line");
		return NULL;
	}
	ged = (NODE) pvalue(val);
	set_pvalue(val, PSTRING, (WORD)nxref(ged));
	return val;
}
/*===============================+
 * __child -- Return child of node
 *   usage: child(NODE) -> NODE
 *==============================*/
PVALUE
__child (PNODE node,
         TABLE stab,
         BOOLEAN *eflg)
{
	NODE ged;
	PVALUE val = eval_and_coerce(PGNODE, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to child is not a record line");
		return NULL;
	}
	ged = (NODE) pvalue(val);
	set_pvalue(val, PGNODE, (WORD)nchild(ged));
	return val;
}
/*=================================+
 * __parent -- Return parent of node
 *   usage: parent(NODE) -> NODE
 *================================*/
PVALUE
__parent (PNODE node,
          TABLE stab,
          BOOLEAN *eflg)
{
	NODE ged;
	PVALUE val = eval_and_coerce(PGNODE, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to parent is not a record line");
		return NULL;
	}
	ged = (NODE) pvalue(val);
	set_pvalue(val, PGNODE, (WORD)nparent(ged));
	return val;
}
/*========================================+
 * __sibling -- Return next sibling of node
 *   usage: sibling(NODE) -> NODE
 *=======================================*/
PVALUE
__sibling (PNODE node,
           TABLE stab,
           BOOLEAN *eflg)
{
	NODE ged;
	PVALUE val = eval_and_coerce(PGNODE, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to sibling is not a record line");
		return NULL;
	}
	ged = (NODE) pvalue(val);
	set_pvalue(val, PGNODE, (WORD)nsibling(ged));
	return val;
}
/*===============================+
 * __level -- Return level of node
 *   usage: level(NODE) -> INT
 *==============================*/
PVALUE
__level (PNODE node,
         TABLE stab,
         BOOLEAN *eflg)
{
	NODE ged;
	INT lev = -1;
	PVALUE val = eval_and_coerce(PGNODE, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to level is not a record line");
		return NULL;
	}
	ged = (NODE) pvalue(val);
	while (ged) {
		lev++;
		ged = nparent(ged);
	}
	set_pvalue(val, PINT, (WORD)lev);
	return val;
}
/*=================================+
 * __copyfile -- Copy file to output
 *   usage: copyfile(STRING) -> VOID
 *================================*/
PVALUE
__copyfile (PNODE node,
            TABLE stab,
            BOOLEAN *eflg)
{
	FILE *cfp;
	STRING fname;
	PVALUE val = eval_and_coerce(PSTRING, iargs(node), stab, eflg);
	char buffer[1024];
	if (*eflg)  {
		prog_error(node, "the arg to copyfile is not a string");
		return NULL;
	}
	fname = (STRING) pvalue(val);
	if (!(cfp = fopenpath(fname, LLREADTEXT, llprograms, (STRING)NULL, (STRING *)NULL))) {
		*eflg = TRUE;
		prog_error(node, "the arg to copyfile is not a file name");
		return NULL;
	}
	delete_pvalue(val);
	while (fgets(buffer, 1024, cfp))
		poutput(buffer);
	fclose(cfp);
	return NULL;
}
/*========================+
 * __nl -- Newline function
 *   usage: nl() -> STRING
 *=======================*/
PVALUE
__nl (PNODE node,
      TABLE stab,
      BOOLEAN *eflg)
{
	*eflg = FALSE;
	return create_pvalue(PSTRING, (WORD)"\n");
}
/*=========================+
 * __space -- Space function
 *   usage: sp() -> STRING
 *========================*/
PVALUE
__space (PNODE node,
         TABLE stab,
         BOOLEAN *eflg)
{
	*eflg = FALSE;
	return create_pvalue(PSTRING, (WORD)" ");
}
/*=============================+
 * __qt -- Double quote function
 *   usage: qt() -> STRING
 *============================*/
PVALUE
__qt (PNODE node,
      TABLE stab,
      BOOLEAN *eflg)
{
	*eflg = FALSE;
	return create_pvalue(PSTRING, (WORD)"\"");
}
/*=============================+
 * __indi -- Convert key to INDI
 *   usage: indi(STRING) -> INDI
 *============================*/
PVALUE
__indi (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	STRING str;
	unsigned char scratch[200], *p, *q = scratch;
	INT c;
	PVALUE val = eval_and_coerce(PSTRING, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to indi is not a string");
		return NULL;
	}
	p = str = (STRING) pvalue(val);
	while ((c = *p++) && chartype(c) != DIGIT)
		;
	if (c == 0) {
		delete_pvalue(val);
		return NULL;
	}
	*q++ = 'I';
	*q++ = c;
	while (chartype(c = *p++) == DIGIT)
		*q++ = c;
	*q = 0;
	delete_pvalue(val);
	if (strlen(scratch) == 1) return NULL;
/*
 *	rec = (STRING) retrieve_record(scratch, &len);
 *	if (rec && len > 6)
 *		val = create_pvalue(PINDI, (WORD)key_to_indi_cacheel(scratch));
 *	else
 *		val = create_pvalue(PINDI, NULL);
 *	if (rec) stdfree(rec);
 */
 	val = create_pvalue(PINDI, (WORD)qkey_to_indi_cacheel(scratch));
	return val;
}
/*===========================+
 * __fam -- Convert key to FAM
 *   usage: fam(STRING) -> FAM
 *==========================*/
PVALUE
__fam (PNODE node,
       TABLE stab,
       BOOLEAN *eflg)
{
	STRING str, rec;
	unsigned char scratch[200], *p, *q = scratch;
	INT c, len;
	PVALUE val = eval_and_coerce(PSTRING, iargs(node), stab, eflg);
	if (*eflg) {
		prog_error(node, "the arg to fam is not a string");
		return NULL;
	}
	p = str = (STRING) pvalue(val);
	while ((c = *p++) && chartype(c) != DIGIT)
		;
	if (c == 0) {
		delete_pvalue(val);
		return NULL;
	}
	*q++ = 'F';
	*q++ = c;
	while (chartype(c = *p++) == DIGIT)
		*q++ = c;
	*q = 0;
	delete_pvalue(val);
	if (strlen(scratch) == 1) return NULL;
	rec = (STRING) retrieve_record(scratch, &len);
	if (rec && len > 6)
		val = create_pvalue(PFAM, (WORD)key_to_fam_cacheel(scratch));
	else
		val = create_pvalue(PFAM, NULL);
	if (rec) stdfree(rec);
	return val;
}
/*=======================================+
 * eval_indi -- Evaluate person expression
 *======================================*/
NODE
eval_indi (PNODE expr,
           TABLE stab,
           BOOLEAN *eflg,
           CACHEEL *pcel)
{
	NODE indi;
	CACHEEL cel;
	PVALUE val = eval_and_coerce(PINDI, expr, stab, eflg);

#ifdef DEBUG
	llwprintf("eval_indi: val, eflg == ");
	show_pvalue(val);
	llwprintf(", %d\n",*eflg);
#endif

	if (*eflg || !val) return NULL;
	cel = (CACHEEL) pvalue(val);
	delete_pvalue(val);
	if (!cel) return NULL;
	if (!cnode(cel)) cel = key_to_indi_cacheel(ckey(cel));
	indi = cnode(cel);
	if (nestr("INDI", ntag(indi))) {
		*eflg = TRUE;
		return NULL;
	}
	if (pcel) *pcel = cel;
	return indi;
}
/*======================================+
 * eval_fam -- Evaluate family expression
 *=====================================*/
NODE
eval_fam (PNODE expr,
          TABLE stab,
          BOOLEAN *eflg,
          CACHEEL *pcel)
{
	NODE fam;
	CACHEEL cel;
	PVALUE val = eval_and_coerce(PFAM, expr, stab, eflg);
	if (*eflg || !val) return NULL;
	cel = (CACHEEL) pvalue(val);
	delete_pvalue(val);
	if (!cel) return NULL;
	if (!cnode(cel)) cel = key_to_fam_cacheel(ckey(cel));
	fam = cnode(cel);
	if (nestr("FAM", ntag(fam))) {
		*eflg = TRUE;
		return NULL;
	}
	if (pcel) *pcel = cel;
	return fam;
}
/*=================================================+
 * __free -- free up data associated with a variable 
 *   usage: free(IDEN]) --> VOID
 *=======================================*/
PVALUE
__free (PNODE node,
        TABLE stab,
        BOOLEAN *eflg)
{
	extern LIST keysets;
	PNODE arg = (PNODE) iargs(node);
	BOOLEAN there;
	PVALUE val;
	INT i, len;
	if (!iistype(arg, IIDENT)) {
		prog_error(node, "arg to free must be a variable");
		*eflg = TRUE;
		return NULL;
	}
	val = (PVALUE) valueofbool(stab, iident(arg), &there);
	if (!there) {
	    val = (PVALUE) valueofbool(globtab, iident(arg), &there);
	}
	if (there && val) {
		switch(ptype(val)) {
		case PSTRING:
			if(pvalue(val)) stdfree((STRING)pvalue(val)); break;
		case PLIST:
			if(pvalue(val)) remove_list(pvalue(val), delete_pvalue);
			break;
		case PTABLE: break;
		case PSET:
			if(pvalue(val)) {
				remove_indiseq(pvalue(val), FALSE);
				len = length_list(keysets);
				for(i = 1; i <= len; i++) {
					if(get_list_element(keysets, i) == pvalue(val)) {
						set_list_element(keysets, i, (WORD)0);
						break;
					}
				}
			}
			break;
		}
		pvalue(val) = (WORD)0;
	}
	return NULL;
}
