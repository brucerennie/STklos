/*
 * fixnum.c     -- Fixnum operations
 *
 * Copyright © 2007-2020 Erick Gallesio - I3S-CNRS/ESSI <eg@essi.fr>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 *
 *           Author: Erick Gallesio [eg@essi.fr]
 *    Creation date:  9-May-2007 17:15 (eg)
 * Last file update:  6-Mar-2020 10:33 (eg)
 */

#include "stklos.h"
#include <stdlib.h>  /* for ldiv */
#include <math.h>    /* for sqrt & pow */


static void error_bad_fixnum(SCM obj)
{
  STk_error("bad fixnum ~S", obj);
}

static void error_division_by_0(void)
{
  STk_error("division by 0");
}

static void error_fx_at_least_1(void)
{
  STk_error("expects at least one fixnum argument");
}

static void error_negative_fixnum(SCM obj)
{
    STk_error("expected non-negative fixnum, found ~S", obj);
}

static void error_bad_fxlength(SCM obj)
{
    STk_error("bad fixnum length ~S (should be < ~S)", obj, INT_LENGTH);
}

static void error_bad_boolean(SCM obj)
{
  STk_error("bad boolean ~S", obj);
}

/* used internally by the fx../carry primitives */
inline static
long exp_2_fxwidth() {
    return ( 1L << (INT_LENGTH/2-1) ) << (INT_LENGTH/2-1);
/*    return (long) pow (2, sizeof(long)* 8 - 2);  */
}


/*
<doc EXT fixnum?
 * (fixnum? obj)
 *
 * Returns |#t| if obj is an exact integer within the fixnum range,
 * |#f| otherwise.
doc>
*/
DEFINE_PRIMITIVE("fixnum?", fixnump, subr1, (SCM obj))
{
  return MAKE_BOOLEAN(INTP(obj));
}

/*
<doc EXT fixnum-width
 * (fixnum-width)
 *
 * Returns the number of bits used to represent a fixnum number
doc>
*/
DEFINE_PRIMITIVE("fixnum-width", fixnum_width, subr0, (void))
{
  return MAKE_INT(INT_LENGTH);
}

/*
<doc EXT least-fixnum greatest-fixnum
 * (least-fixnum)
 * (greatest-fixnum)
 *
 * These procedures return the minimum value and the maximum value of
 * the fixnum range.
doc>
*/
DEFINE_PRIMITIVE("least-fixnum", least_fixnum, subr0, (void))
{
  return MAKE_INT((unsigned long)INT_MIN_VAL);
}

DEFINE_PRIMITIVE("greatest-fixnum", greatest_fixnum, subr0, (void))
{
  return MAKE_INT(INT_MAX_VAL);
}

/*
<doc  fxzero?
 * (fxzero? obj)
 *
 * |fxzero?| returns |#t| if |obj| is the fixnum zero and returns
 * |#f| if it is a non-zero fixnum.
 * @lisp
 *   (fxzero? #f)            =>  error
 *   (fxzero? (expt 100 100) =>  error
 *   (fxzero? 0)             =>  #t
 *   (fxzero? 1)             =>  #f
 * @end lisp
doc>
 */
DEFINE_PRIMITIVE("fxzero?", fxzerop, subr1, (SCM o1))
{
    /* if (!INTP(o1)) error_bad_fixnum(o1); */
    return MAKE_BOOLEAN (INT_VAL(o1)==0);
}

/*
<doc  fxpositive?
 * (fxpositive? obj)
 *
 * |fxpositive?| returns |#t| if |obj| is a positive fixnum and returns
 * |#f| if it is a non-positive fixnum.
 * @lisp
 *   (fxpositive? #f)            =>  error
 *   (fxpositive? (expt 100 100) =>  error
 *   (fxpositive? 0)             =>  #f
 *   (fxpositive? 1)             =>  #t
 *   (fxpositive? -1)            =>  #f
 * @end lisp
doc>
 */
DEFINE_PRIMITIVE("fxpositive?", fxpositivep, subr1, (SCM o1))
{
    /* if (!INTP(o1)) error_bad_fixnum(o1); */
    return MAKE_BOOLEAN (INT_VAL(o1)>0);
}

/*
<doc  fxnegative?
 * (fxnegative? obj)
 *
 * |fxnegative?| returns |#t| if |obj| is a negative fixnum and returns
 * |#f| if it is a non-negative fixnum.
 * @lisp
 *   (fxnegative? #f)            =>  error
 *   (fxnegative? (expt 100 100) =>  error
 *   (fxnegative? 0)             =>  #f
 *   (fxnegative? 1)             =>  #f
 *   (fxnegative? -1)            =>  #t
 * @end lisp
doc>
 */
DEFINE_PRIMITIVE("fxnegative?", fxnegativep, subr1, (SCM o1))
{
    /* if (!INTP(o1)) error_bad_fixnum(o1); */
    return MAKE_BOOLEAN (INT_VAL(o1)<0);
}

/*
<doc  fxodd?
 * (fxodd? obj)
 *
 * |fxodd?| returns |#t| if |obj| is a odd fixnum and returns
 * |#f| if it is an even fixnum.
 * @lisp
 *   (fxodd? #f)            =>  error
 *   (fxodd? (expt 100 100) =>  error
 *   (fxodd? 0)             =>  #f
 *   (fxodd? 1)             =>  #t
 *   (fxodd? 4)             =>  #f
 * @end lisp
doc>
 */
DEFINE_PRIMITIVE("fxodd?", fxoddp, subr1, (SCM o1))
{
    /*if (!INTP(o1)) error_bad_fixnum(o1);*/
    return MAKE_BOOLEAN (INT_VAL(o1)&1);
}

/*
<doc  fxeven?
 * (fxeven? obj)
 *
 * |fxeven?| returns |#t| if |obj| is a even fixnum and returns
 * |#f| if it is an odd fixnum.
 * @lisp
 *   (fxeven? #f)            =>  error
 *   (fxeven? (expt 100 100) =>  error
 *   (fxeven? 0)             =>  #t
 *   (fxeven? 1)             =>  #f
 *   (fxeven? 4)             =>  #t
 * @end lisp
doc>
 */
DEFINE_PRIMITIVE("fxeven?", fxevenp, subr1, (SCM o1))
{
    /*if (!INTP(o1)) error_bad_fixnum(o1);*/
    return MAKE_BOOLEAN (!(INT_VAL(o1)&1));
}

/*
<doc EXT fx+ fx- fx* fxdiv fxrem fxmod fxabs
 * (fx+ fx1 fx2)
 * (fx- fx1 fx2)
 * (fx* fx1 fx2)
 * (fxdiv fx1 fx2)
 * (fxrem fx1 fx2)
 * (fxmod fx1 fx2)
 * (fxabs fx)
 * (fx- fx)
 *
 * These procedures compute (respectively) the sum, the difference, the product,
 * the quotient and the remainder and modulp of the fixnums |fx1| and |fx2|.
 * The call of  |fx-| with one parameter |fx| computes the opposite of |fx|,
 * and |fxabs| compute the absolute value of |fx|.
doc>
 */
DEFINE_PRIMITIVE("fx+", fxplus, subr2, (SCM o1, SCM o2))
{
    /* removing original STklos check:
      if (!INTP(o1)) error_bad_fixnum(o1);
      if (!INTP(o2)) error_bad_fixnum(o2); */
  return MAKE_INT(INT_VAL(o1) + INT_VAL(o2));
}

DEFINE_PRIMITIVE("fx-", fxminus, subr12, (SCM o1, SCM o2))
{
    /* removing original STklos test:
       if (!INTP(o1)) error_bad_fixnum(o1); */
  if (!o2)
    return MAKE_INT(-INT_VAL(o1));
  /* if (!INTP(o2)) error_bad_fixnum(o2); */
  return MAKE_INT(INT_VAL(o1) - INT_VAL(o2));
}

DEFINE_PRIMITIVE("fx*", fxtime, subr2, (SCM o1, SCM o2))
{
    /* removing original STklos test:
       if (!INTP(o1)) error_bad_fixnum(o1);
       if (!INTP(o2)) error_bad_fixnum(o2); */
  return MAKE_INT(INT_VAL(o1) * INT_VAL(o2));
}

DEFINE_PRIMITIVE("fxdiv", fxdiv, subr2, (SCM o1, SCM o2))
{
  int n;

    /* removing original STklos test:
       if (!INTP(o1)) error_bad_fixnum(o1);
       if (!INTP(o2)) error_bad_fixnum(o2); */

  n = INT_VAL(o2);

  if (!n) error_division_by_0();
  return MAKE_INT(INT_VAL(o1) / n);
}

DEFINE_PRIMITIVE("fxrem", fxrem, subr2, (SCM o1, SCM o2))
{
  int n;

     /* removing original STklos test:
        if (!INTP(o1)) error_bad_fixnum(o1);
        if (!INTP(o2)) error_bad_fixnum(o2); */

  n = INT_VAL(o2);

  if (!n) error_division_by_0();
  return MAKE_INT(INT_VAL(o1) % n);
}


DEFINE_PRIMITIVE("fxmod", fxmod, subr2, (SCM o1, SCM o2))
{
      /* removing original STklos test:
         if (!INTP(o1)) error_bad_fixnum(o1);
         if (!INTP(o2)) error_bad_fixnum(o2); */
  {
    int n1 = INT_VAL(o1);
    int n2 = INT_VAL(o2);
    int r;

    if (!n2) error_division_by_0();
    r = n1 % n2;

    /* (negativep(n1) != negativep(n2) && !zerop(r)) */
    if ((((n1 < 0) && (n2 >= 0)) || ((n1 >= 0) && (n2 < 0))) &&
        r)
      r += n2;

    return MAKE_INT(r);
  }
}

DEFINE_PRIMITIVE("fxabs", fxabs, subr1, (SCM o1))
{
    /* removing original STklos test:
       if (!INTP(o1)) error_bad_fixnum(o1); */
  return MAKE_INT(abs(INT_VAL(o1)));
}

/*
<doc EXT fxsquare fxsqrt
 * (fxsquare fx1)
 * (fxsqrt fx1)
 *
 * These procedures compute (respectively) the square and the square root
 * of the fixnum |fx1|.
 * |fxsqrt| id semantically equivalent to exact-integer-sqrt (not sqrt), so
 * that |(fxsqrt n)| returns two values |a|, |b|, such that |a*a+b|=|n|.
  * @lisp
 *   (fxsqrt? #f)            =>  error
 *   (fxdqrt? (expt 100 100) =>  error
 *   (fxsqrt? -1)            =>  error
 *   (fxsqrt? 0)             =>  0, 0
 *   (fxsqrt? 1)             =>  1, 0
 *   (fxsqrt? 6)             =>  2, 2
 * @end lisp
doc>
 */
DEFINE_PRIMITIVE("fxsquare", fxsquare, subr1, (SCM o1))
{
  return MAKE_INT(INT_VAL(o1) * INT_VAL(o1));
}

DEFINE_PRIMITIVE("fxsqrt", fxsqrt, subr1, (SCM o1))
{
  long no = INT_VAL(o1);
  if (no < 0)   STk_error("non negative fixnum expected. It was: ~S", o1);
  long n1 = (long) sqrt((float)no);
  long n2 = no - (n1*n1);
  return STk_n_values(2,
                      MAKE_INT(n1),
                      MAKE_INT(n2));
}


/*
<doc  fxmax fxmin
 * (fxmax fx1 fx2 ...)
 * (fxmin fx1 fx2 ...)
 *
 * These procedures return the maximum or minimum of their fixnum arguments.
 *
 * @lisp
 * (fxmax 3 4)              =>  4    
 * (fxmax 3.9 4)            =>  error
 * (fxmax)                  =>  error
 * (fxmax 2 -1 3)           =>  3
 * @end lisp
doc>
 */
DEFINE_PRIMITIVE("fxmax", fxmax, vsubr, (int argc, SCM *argv))
{
    SCM res;
    
    if (argc == 0) error_fx_at_least_1();
    
    if (argc == 1)  return *argv;
    
    for (res = *argv--; --argc; argv--) {
        /* Do not check that the argument is a correct number 
           if (!INTP(*argv)) error_bad_fixnum(*argv); */

        /* compute max */
        if (INT_VAL(res) < INT_VAL(*argv)) res = *argv;
    }
    return res;
}

DEFINE_PRIMITIVE("fxmin", fxmin, vsubr, (int argc, SCM *argv))
{
    SCM res;
    
    if (argc == 0) error_fx_at_least_1();
    
    if (argc == 1) return *argv;
    
    for (res = *argv--; --argc; argv--) {
        /* Do not check that the argument is a correct number 
           if (!INTP(*argv)) error_bad_fixnum(*argv); */

        /* compute max */
        if (INT_VAL(res) > INT_VAL(*argv)) res = *argv;
    }
    return res;
}


/*
<doc EXT fx< fx<= fx> fx>= fx=
 * (fx< fx1 fx2)
 * (fx<= fx1 fx2)
 * (fx> fx1 fx2)
 * (fx>= fx1 fx2)
 * (fx= fx1 fx2)
 *
 * These procedures compare the fixnums |fx1| and |fx2| and retun |#t| if
 * the comparison is true and |#f| otherwise.
doc>
 */
#define SIMPLE_COMP(name, func, op) \
DEFINE_PRIMITIVE(name, func, subr2, (SCM o1, SCM o2))           \
{                                                               \
    return MAKE_BOOLEAN(INT_VAL(o1) op INT_VAL(o2));            \
}
/* above: removed original STklos test */

SIMPLE_COMP("fx<",  fxlt, <)
SIMPLE_COMP("fx<=", fxle, <=)
SIMPLE_COMP("fx>",  fxgt, >)
SIMPLE_COMP("fx>=", fxge, >=)
SIMPLE_COMP("fx=",  fxeq, ==)


/*
<doc EXT fx<? fx<=? fx>? fx>=? fx=?
 * (fx<? fx1 fx2 ...)
 * (fx<=? fx1 fx2 ...)
 * (fx>? fx1 fx2 ...)
 * (fx>=? fx1 fx2 ...)
 * (fx=? fx1 fx2 ...)
 *
 * These are SRFI-143 procedures that compare the fixnums |fx1|, |fx2|, and so on.
 * |fx<?| and |fx>?| return |#t| if the arguments are in strictly increasing/decreasing
 * order;
 * |fx<=?| and |fx>=?| do the same, but admit equal neighbors;
 * |fx=?| returns |#t| if the arguments are all equal.
doc>
 */
DEFINE_PRIMITIVE("fx<?", fxlt_, vsubr, (int argc, SCM *argv))
{
    SCM p;
    if (argc == 0) error_fx_at_least_1();
    
    if (argc == 1) return *argv;
        
    for (p = *argv--; --argc; p=*argv,argv--) {
        /* Do NOT check that the argument is a correct number
           if (!INTP(*argv)) error_bad_fixnum(*argv);*/
        
        /* compute  */
        if (INT_VAL(p) >= INT_VAL(*argv)) return STk_false;
    }
    return STk_true;
}

DEFINE_PRIMITIVE("fx<=?", fxle_, vsubr, (int argc, SCM *argv))
{
    SCM p;
    if (argc == 0) error_fx_at_least_1();
    
    if (argc == 1) return *argv;
    
    
    for (p = *argv--; --argc; p=*argv,argv--) {
        /* Do NOT check that the argument is a correct number
           if (!INTP(*argv)) error_bad_fixnum(*argv);*/

        /* compute  */
        if (INT_VAL(p) > INT_VAL(*argv)) return STk_false;
    }
    return STk_true;
}


DEFINE_PRIMITIVE("fx>?", fxgt_, vsubr, (int argc, SCM *argv))
{
    SCM p;
    if (argc == 0) error_fx_at_least_1();
    
    if (argc == 1)  return *argv;
    
    for (p = *argv--; --argc; p=*argv,argv--) {
        /* Do NOT check that the argument is a correct number
           if (!INTP(*argv)) error_bad_fixnum(*argv);*/

        /* compute  */
        if (INT_VAL(p) <= INT_VAL(*argv)) return STk_false;
    }
    return STk_true;
}

DEFINE_PRIMITIVE("fx>=?", fxge_, vsubr, (int argc, SCM *argv))
{
    SCM p;
    if (argc == 0) error_fx_at_least_1();
    
    if (argc == 1) {
        if (!INTP(*argv)) error_bad_fixnum(*argv);
        return *argv;
    }
    
    for (p = *argv--; --argc; p=*argv,argv--) {
        /* Do NOT check that the argument is a correct number
           if (!INTP(*argv)) error_bad_fixnum(*argv);*/

        /* compute  */
        if (INT_VAL(p) < INT_VAL(*argv)) return STk_false;
    }
    return STk_true;
}

DEFINE_PRIMITIVE("fx=?", fxeq_, vsubr, (int argc, SCM *argv))
{
    SCM p;
    if (argc == 0) error_fx_at_least_1();
    
    if (argc == 1) return *argv;
    
    for (p = *argv--; --argc; p=*argv,argv--) {
        /* Do NOT check that the argument is a correct number
           if (!INTP(*argv)) error_bad_fixnum(*argv);*/

        /* compute  */
        if (INT_VAL(p) != INT_VAL(*argv)) return STk_false;
    }
    return STk_true;
}

DEFINE_PRIMITIVE("fxneg", fxneg, subr1, (SCM o1))
{
   return MAKE_INT(-INT_VAL(o1));
}



/*
<doc EXT fxnot fxand fxior fxxor
 * (fxnot fx1)
 * (fxand fx1 fx2)
 * (fxior fx1 fx2)
 * (fxxor fx1 fx2)
 *
 * These procedures are specified in SRFI-143, and they return
 * (respectively) the bitwise not, and, inclusive or and exclusive
 * or of their arguments, which must be fixnums.
 * @lisp
 * (fxnot 1)              => -2
 * (fxnot 0)              => -1
 * (fxand #x1010 #x1011)  => 4112  ; = #x1010
 * (fxior #x1010 #x1011)  => 4113  ; = #x1011
 * (fxxor #x1010 #x1011)  => 1     ; = #x0001
 * @end lisp
doc>
 */
DEFINE_PRIMITIVE("fxnot", fxnot, subr1, (SCM o1))
{
  return MAKE_INT(~INT_VAL(o1));
}

DEFINE_PRIMITIVE("fxand", fxand, subr2, (SCM o1, SCM o2))
{
  return MAKE_INT(INT_VAL(o1) & INT_VAL(o2));
}

DEFINE_PRIMITIVE("fxior", fxior, subr2, (SCM o1, SCM o2))
{
  return MAKE_INT(INT_VAL(o1) | INT_VAL(o2));
}

DEFINE_PRIMITIVE("fxxor", fxxor, subr2, (SCM o1, SCM o2))
{
  return MAKE_INT(INT_VAL(o1) ^ INT_VAL(o2));
}

/*
<doc EXT fxarithmetic-shift-right fxarithmetic-shift-left fxarithmetic-shift
 * (fxarithmetic-shift-right fx count)
 * (fxarithmetic-shift-left fx count)
 * (fxarithmetic-shift fx count)
 *
 * These procedures are specified in SRFI-143, and they perform
 * bitwise right-shift, left-shft and shift with arbitrary direction
 * on fixnums. The strictly left and right shifts are more efficient.
 * @lisp
 * (fxarithmetic-shift-right #b100110 3) =>   4 ; = #b100
 * (fxarithmetic-shift-left  #b100110 3) => 304 ; = #b100110000
 * (fxarithmetic-shift #b101 2)          => 20  ; = #b10100
 * (fxarithmetic-shift #b101 -2)         =>  1  ; =#b1
 * @end lisp
doc>
 */
DEFINE_PRIMITIVE("fxarithmetic-shift-right", fxarithmetic_shiftr, subr2, (SCM o1, SCM o2))
{
  if (!INTP(o2)) error_bad_fixnum(o2);
  long k = INT_VAL(o2);
  if (k>=0)
      return MAKE_INT( INT_VAL(o1) >> k );
  error_negative_fixnum(o2);
  return STk_false; /* should never get here */
}

DEFINE_PRIMITIVE("fxarithmetic-shift-left", fxarithmetic_shiftl, subr2, (SCM o1, SCM o2))
{
  if (!INTP(o2)) error_bad_fixnum(o2);
  long k = INT_VAL(o2);
  if (k>=0)
      return MAKE_INT( INT_VAL(o1) << k );
  error_negative_fixnum(o2);
  return STk_false; /* should never get here */
}

DEFINE_PRIMITIVE("fxarithmetic-shift", fxarithmetic_shift, subr2, (SCM o1, SCM o2))
{
  if (!INTP(o2)) error_bad_fixnum(o2);
  long k = INT_VAL(o2);
/*  if (abs(k) > INT_LENGTH) error_bad_fxlength(o2); */
  if (k<0) return MAKE_INT( INT_VAL(o1) >> -k );
  else     return MAKE_INT( INT_VAL(o1) << k );
}

/*
<doc EXT fxlength
 * (fxlength fx)
 *
 * This is a SRFI-143 procedure that returns the length of the fixnum in
 * bits (that is, the number of bits necessary to represent the number).
 * @lisp
 * (fxlength #b101)          =>  3
 * (fxlength #b1101)         =>  4
 * (fxlength #b0101)         =>  3
 * @end lisp
doc>
 */
DEFINE_PRIMITIVE("fxlength", fxlength, subr1, (SCM o1))
{
  long n = INT_VAL(o1);
  if (n == -1 || n == 0) return MAKE_INT(0);
  if (n>0)  return MAKE_INT( (long) log2( (float) n) + 1 );
  return MAKE_INT( (long) log2( (float) abs(n+1) ) + 1 ); /* n<-1 */
}

/*
<doc EXT fxif
 * (fxif mask fx1 fx2)
 *
 * This is a SRFI-143 procedure that merge the fixnum bitstrings |fx1| and |fx2|, with bitstring
 * mask determining from which string to take each bit. That is, if the kth bit of mask
 * is 1, then the kth bit of the result is the kth bit of |fx1|, otherwise the kth bit of
 * |fx2|.
 * @lisp
 * (fxif 3 1 8)                            => 9
 * (fxif 3 8 1)                            => 0
 * (fxif 1 1 2)                            => 3
 * (fxif #b00111100 #b11110000 #b00001111) => #b00110011
 * @end lisp
doc>
 */
DEFINE_PRIMITIVE("fxif", fxif, subr3, (SCM m, SCM o1, SCM o2))
{
  unsigned long mask = INT_VAL(m);
  return MAKE_INT(  (  mask  & INT_VAL(o1))  |
                    ((~mask) & INT_VAL(o2)));
}

/*
<doc EXT fxbit-set?
 * (fxbit-set? index fx)
 *
 * This is a SRFI-143 procedure that returns |#t| if the |index|-th bit of
 * |fx|.
 * @lisp
 * (fxbit-set? 1 3)          => #t
 * (fxbit-set? 2 7)          => #t
 * (fxbit-set? 3 6)          => #f
 * (fxbit-set? 5 #b00111100) => #t
 * @end lisp
doc>
 */
DEFINE_PRIMITIVE("fxbit-set?", fxbit_setp, subr2, (SCM o1, SCM o2))
{
/* don't check arguments
   if (!INTP(o1)) error_bad_fixnum(o1);
   if (!INTP(o2)) error_bad_fixnum(o2); 
   if (INT_VAL(o1) > INT_LENGTH-1) error_bad_fixnum(o1);
   if (INT_VAL(o1) < 0) error_negative_fixnum(); */
  return MAKE_BOOLEAN(  (1 << INT_VAL(o1)) & INT_VAL(o2) );
}

/*
<doc EXT fxcopy-bit
 * (fxcopy-bit index fx value)
 *
 * This is a SRFI-143 procedure that sets the |index|-th bit if |fx|
 * to one if |value| is |#t|, and to zero if |value| is |#f|.
 * @lisp
 * (fxcopy-bit 2 3 #t)          =>  7
 * (fxcopy-bit 2 7 #f)          =>  3
 * (fxcopy-bit 5 #b00111100 #f) => 28 ; = #b00011100
 * @end lisp
doc>
 */
DEFINE_PRIMITIVE("fxcopy-bit", fxcopy_bit, subr3, (SCM o1, SCM o2, SCM o3))
{
/* don't check arguments
   if (!INTP(o1)) error_bad_fixnum(o1);
   if (!INTP(o2)) error_bad_fixnum(o2); 
   if (!BOOLEANP(o3)) error_bad_boolean(o3); */
/*  if (INT_VAL(o1)>INT_LENGTH -1) error_bad_fixnum(o1); */
  unsigned long mask = 1 << INT_VAL(o1);
  if (o3==STk_true) return MAKE_INT( INT_VAL(o2) | mask);
  else              return MAKE_INT( INT_VAL(o2) & (~mask) );
}


/*
<doc EXT fxbit-count
 * (fxbit-count fx1)
 *
 * This is a SRFI-143 procedure that returns the quantity of bits equal to one in
 * the fixnum |fx| (that is, computes its Hamming weight).
 * @lisp
 * (fxbit-count 8)                         => 1
 * (fxbit-count 3)                         => 2
 * (fxbit-count 7)                         => 3
 * (fxbit-count #b00111010)                => 4
 * @end lisp
doc>
 */
DEFINE_PRIMITIVE("fxbit-count", fxbit_count, subr1, (SCM o1))
/* TODO: Someewhat efficient, but there is a better way, described
   in "Hacker's Delight", but it needs to be adapted to the
   variable length datum. */
{
  long n = INT_VAL(o1);
  int total=0;
  for (int i=0; i< INT_LENGTH; i++, n=n>> 1)
      if ( n & 1 )
          total++;
  return MAKE_INT(total);
}

/*
<doc EXT fxfirst-set-bit
 * (fxfirst-set-bit fx1)
 *
 * This is a SRFI-143 procedure that returns the index of the first (smallest index)
 * 1 bit in bitstring |fx|. Returns -1 if |fx| contains no 1 bits (i.e., if |fx|
 * is zero). 
 * @lisp
 * (fxfirst-set-bit  8)                         => 3
 * (fxfirst-set-bit  3)                         => 0
 * (fxfirst-set-bit  7)                         => 0
 * (fxfirst-set-bit  #b10110000)                => 4
 * @end lisp
doc>
 */
DEFINE_PRIMITIVE("fxfirst-set-bit", fxfirst_set_bit, subr1, (SCM o1))
{
  unsigned long n = INT_VAL(o1);
  for (int i=0; i< INT_LENGTH; i++, n=n>> 1)
      if ( n & 1 )
          return MAKE_INT(i);
  return MAKE_INT(-1);
}

/*
<doc EXT fxbit-field
 * (fxbit-field fx1 start end)
 *
 * This is a SRFI-143 procedure that extracts a bit field from the fixnum |fx1|.
 * The bit field is the sequence of bits between |start| (including) and |end|
 * (excluding)
 * @lisp
 * (fxbit-field  #b10110000 3 5)  => 6 ; = #b110
 * @end lisp
doc>
 */
DEFINE_PRIMITIVE("fxbit-field", fxbit_field, subr3, (SCM o1, SCM o2, SCM o3))
{
    int start = INT_VAL(o2);
    int end = INT_VAL(o3);
    unsigned long n = INT_VAL(o1);
    int leftshift = INT_LENGTH +2 -end;
    n = (n << leftshift) >> (leftshift+start);
    return MAKE_INT(n);
}


/*
<doc EXT fxbit-field-rotate
 * (fxbit-field-rotate fx)
 *
 * This is a SRFI-143 procedure that returns fx with the field cyclically permuted by count bits towards high-order. 
 * @lisp
 * (fxbit-field-rotate #b101011100 -2 1 5)     => 342  = #b101010110
 * (fxbit-field-rotate #b101011011110 -3 2 10) => 3034 = #b101111011010
 * (fxbit-field-rotate #b101011011110 3 2 10)  => 2806 = #b101011110110
 * 
 * @end lisp
doc>
 */
DEFINE_PRIMITIVE("fxbit-field-rotate", fxbit_field_rotate, subr4, (SCM o1, SCM o2, SCM o3, SCM o4))
{
    int count = INT_VAL(o2);
    int start = INT_VAL(o3);
    int end = INT_VAL(o4);
    unsigned long n = INT_VAL(o1);

    unsigned long start_mask = (1 << start);
    unsigned long last_mask  = (1 << (end-1));

    if (count < 0) {
        for (int j=0; j<-count; j++) {
            
            /* single rotation to RIGHT: */
            unsigned long start_bit = n & start_mask; /* save start-th bit of n */
            for (int i=start; i<end-1; i++)
                n = (n & (~(1 << i))) /* set ith-bit to zero */
                    | ( (n & (1 << (i+1)) ) >> 1); /* now OR it with i+1-th bit, shifted down */
            
            n = (n & ~last_mask)
                | (start_bit << (end-start-1));
        }
    } else if (count > 0) {
        for (int j=0; j < count; j++) {
            
            /* single rotation to LEFT: */
            unsigned long end_bit = n & last_mask; /* save start-th bit of n */
            for (int i=end-1; i>start; i--)
                n = (n & (~(1 << i))) /* set ith-bit to zero */
                    | ( (n & (1 << (i-1)) ) << 1); /* now OR it with i-1-th bit, shifted up */

            n = (n & ~start_mask)
                | (end_bit >> (end-start-1));
        }
    } else return o1;
        
    return MAKE_INT((long)n);
}


/*
  This is a helper function for the fx../carry functions. It is
  similar to balanced/ from SRFI-141, but works on long integers.
 */
inline static
void fxbalanced_div (long *qq, long *rr, long x, long y)
{
    if (y==0) error_division_by_0();
    ldiv_t qr = ldiv(x,y);
    long q = qr.quot;
    long r = qr.rem;
    
    if (x >= 0) {
        if (y > 0) {
            if (2*r >= y) {
                *qq = (q+1);
                *rr = r-y;
            }
            else {
                *qq = q;
                *rr = r;
            }
        } else { /* y <= 0 */
            if (2*r >= -y) {
                *qq = q-1;
                *rr = r+y;
            } else {
                *qq = q;
                *rr = r;
            }
        }
    } else { /* x < 0 */
        if (y > 0) {
            if (2*r < -y) {
                *qq = q-1;
                *rr = r+y;
            } else {
                *qq = q;
                *rr = r;
            }
        } else { /* y <= 0 */
            if (2*r < y) {
                *qq = q+1;
                *rr = r-y;
            } else {
                *qq = q;
                *rr = r;
            }
        }
    }
}



/*
<doc EXT fx+/carry
 * (fx+/carry i j k)
 *
 * Returns two values: |i|+|j|+|k|, and carry: it is the value of the computation
 * (let*-values (((s) (+ i j k))
 *        ((q r) (balanced/ s (expt 2 fx-width))))
 *   (values r q))
doc>
*/
DEFINE_PRIMITIVE("fx+/carry", fxsum_carry, subr3, (SCM o1, SCM o2, SCM o3))
{
    long s = INT_VAL(o1) + INT_VAL(o2) + INT_VAL(o3);
    long e = exp_2_fxwidth();
    long q;
    long r;
    fxbalanced_div (&q,&r,s,e);
    STk_n_values(2, MAKE_INT(r), MAKE_INT(q));
}

/*
<doc EXT fx-/carry
 * (fx-/carry i j k)
 *
 * Returns two values: |i|-|j|-|k|, and carry: it is the value of the computation
 * (let*-values (((s) (- i j k))
 *        ((q r) (balanced/ s (expt 2 fx-width))))
 *   (values r q))
doc>
*/
DEFINE_PRIMITIVE("fx-/carry", fxminus_carry, subr3, (SCM o1, SCM o2, SCM o3)) 
{ 
    long d = INT_VAL(o1) - INT_VAL(o2) - INT_VAL(o3); 
    long e = exp_2_fxwidth();
    long q;
    long r;
    fxbalanced_div (&q,&r,d,e);
    STk_n_values(2, MAKE_INT(r), MAKE_INT(q));
}

/*
<doc EXT fx{*}/carry
 * (fx{*}/carry i j k)
 *
 * Returns two values: |i|-|j|-|k|, and carry: it is the value of the computation
 * (let*-values (((s) (+ (* i j) k))
 *        ((q r) (balanced/ s (expt 2 fx-width))))
 *   (values r q))
doc>
*/
DEFINE_PRIMITIVE("fx*/carry", fxmul_carry, subr3, (SCM o1, SCM o2, SCM o3))
{
    long s = INT_VAL(o1) * INT_VAL(o2) + INT_VAL(o3);
    long e = exp_2_fxwidth();
    long q;
    long r;
    fxbalanced_div (&q,&r,s,e);
    STk_n_values(2, MAKE_INT(r), MAKE_INT(q));
}


/*
<doc EXT fxbit-field-reverse
 * (fxbit-field-reverse fx)
 *
 * This is a SRFI-143 procedure that returns fx with the order of the bits in the field reversed.
 * @lisp
 * (fxbit-field-reverse #b101011100 1 5)     => #b101001110
 * (fxbit-field-reverse #b101011011110 2 10) => #b101110110110
 * 
 * @end lisp
doc>
 */
DEFINE_PRIMITIVE("fxbit-field-reverse", fxbit_field_reverse, subr3, (SCM o1, SCM o2, SCM o3))
{
    int start = INT_VAL(o2);
    int end = INT_VAL(o3);
    unsigned long n = INT_VAL(o1);
    int size = end-start;

    for (int i=0; i < size/2; i++) {
        long mask_left  = 1<<(start+i);
        long mask_right = 1<<(start+size-i-1);
        long left = n & mask_left;
        long right= n & mask_right;
        
        if (left)
            n = n | mask_right;
        else
            n = n & (~mask_right);
        
        if (right)
            n = n | mask_left;
        else
            n = n & (~mask_left);
    }
    return MAKE_INT(n);
}

int STk_init_fixnum(void)
{
  ADD_PRIMITIVE(fixnump);
  ADD_PRIMITIVE(fixnum_width);
  ADD_PRIMITIVE(least_fixnum);
  ADD_PRIMITIVE(greatest_fixnum);

  ADD_PRIMITIVE(fxzerop);
  ADD_PRIMITIVE(fxpositivep);
  ADD_PRIMITIVE(fxnegativep);
  ADD_PRIMITIVE(fxoddp);
  ADD_PRIMITIVE(fxevenp);

  ADD_PRIMITIVE(fxplus);
  ADD_PRIMITIVE(fxminus);
  ADD_PRIMITIVE(fxtime);
  ADD_PRIMITIVE(fxdiv);
  ADD_PRIMITIVE(fxneg);
  ADD_PRIMITIVE(fxmod);
  ADD_PRIMITIVE(fxrem);
  ADD_PRIMITIVE(fxabs);
  ADD_PRIMITIVE(fxsquare);
  ADD_PRIMITIVE(fxsqrt);
  
  ADD_PRIMITIVE(fxmax);
  ADD_PRIMITIVE(fxmin);

  ADD_PRIMITIVE(fxlt);
  ADD_PRIMITIVE(fxle);
  ADD_PRIMITIVE(fxgt);
  ADD_PRIMITIVE(fxge);
  ADD_PRIMITIVE(fxeq);

  ADD_PRIMITIVE(fxlt_);
  ADD_PRIMITIVE(fxle_);
  ADD_PRIMITIVE(fxgt_);
  ADD_PRIMITIVE(fxge_);
  ADD_PRIMITIVE(fxeq_);

  ADD_PRIMITIVE(fxnot);
  ADD_PRIMITIVE(fxand);
  ADD_PRIMITIVE(fxior);
  ADD_PRIMITIVE(fxxor);
  ADD_PRIMITIVE(fxarithmetic_shift);
  ADD_PRIMITIVE(fxarithmetic_shiftr);
  ADD_PRIMITIVE(fxarithmetic_shiftl);
  ADD_PRIMITIVE(fxbit_count);
  ADD_PRIMITIVE(fxlength);
  ADD_PRIMITIVE(fxif);
  ADD_PRIMITIVE(fxbit_setp);
  ADD_PRIMITIVE(fxcopy_bit);
  ADD_PRIMITIVE(fxfirst_set_bit);
  ADD_PRIMITIVE(fxbit_field);
  ADD_PRIMITIVE(fxbit_field_rotate);
  ADD_PRIMITIVE(fxbit_field_reverse);

  ADD_PRIMITIVE(fxsum_carry);
  ADD_PRIMITIVE(fxminus_carry);
  ADD_PRIMITIVE(fxmul_carry);

  return TRUE;
}
