/*  Yet another program by Bob Jenkins.  3/16/88, for 15-451 */
/*  Public Domain */

#ifndef STANDARD
#include "standard.h"
#endif
#ifndef POLY
#include "poly.h"
#endif

/* These macros are machine dependent */
#define p_sign(m) ((sb4)(m))
#define p_cast(m) ((sb2)(m))
#define p_add_pow(m1, m2) ((m1)+(m2))

/* check that the coefficients add up to a power of -2 */
word p_check(p)
poly *p;
{
  ub4 i;
  sb4 sum;
  sb4 compare;

  if (!p || !p->len) return FALSE;
  for (sum=0, i=0; i<p->len; ++i) 
  {
    sum += p->term[i].coef;
  }
  for (compare=1; compare && compare != sum; compare *= (sb4)(-2))
  {
    ;
  }
  if (!compare) return FALSE;
  return TRUE;
}

void   p_copy(inp, outp)
poly  *inp;
poly  *outp;
/*  This returns a MALLOCed copy of inp.
    It eliminates all terms with coefficients of 0. */
{
  term  *interm;
  term  *outerm;
  term  *endterm;

  if (inp->len)
  {
    outp->term = (term *)malloc((size_t)(inp->len*sizeof(term)));
    for (interm = inp->term, outerm = outp->term,
         endterm = interm+inp->len; interm != endterm; ++interm)
      if (interm->coef) *(outerm++) = *interm;
    outp->len = outerm-outp->term;
  }
  else
  {
    outp->term = (term *)0;
    outp->len  = 0;
  }
}


void   p_show(p)                   /* O(|p|). This displays the polynomial p */
poly  *p;
{
  register term  *pt;
  register term  *pend;
  register sb4    first;
  register sb4    m;
  register sb4    l;


  printf("The polynomial is: \n  ");
  if (!p->len)
  {
    printf("0");
    return;
  }
  for (first = 1, pt = p->term, pend = pt+p->len; pt != pend; ++pt)
  {
    if (!pt->coef) continue;
    m = p_sign(pt->m);
    l = p_sign(pt->l);
    if (pt->coef < ((sb4)0)) printf(" - ");
    else if (!first) printf(" + ");
    if (pt->coef > ((sb4)1) ) printf("%ld", pt->coef);
    else if (pt->coef < (sb4)(-1)) printf("%ld", -pt->coef);
    else if ((!l) && (!m)) printf("%d", 1);
    if (pt->m)
    {
      printf("M");
      if (m != 1) printf("^%ld", m);
    }
    if (pt->l)
    {
      printf("L");
      if (l != 1) printf("^%ld", l);
    }
    if (first) first = 0;
  }
  printf("\n");
}




void   p_term(coef, m, l, inp, outp)  /* outp = inp + {t.c=c; t.l=l; t.m=m;} */
sb4    coef;
sb4    m;
sb4    l;
poly  *inp;
poly  *outp;
{
  poly   p[1];
  term   t[1];

  t->coef = coef;
  t->m    = p_cast(m);
  t->l    = p_cast(l);
  p->len  = 1;
  p->term = t;
  p_add(p, inp, outp);
}



void   p_add(inp1, inp2, outp)                  /* This is like a merge sort */
poly  *inp1;
poly  *inp2;
poly  *outp;
{
  register term  *t1;
  register term  *t2;
  register term  *tout;
  register term  *end1;
  register term  *end2;
  register sb4    eq1;

  outp->term = (term *)malloc((size_t)((inp1->len+inp2->len)*sizeof(term)));
  t1         = inp1->term;
  end1       = t1+inp1->len;
  t2         = inp2->term;
  end2       = t2+inp2->len;
  tout       = outp->term;
  if (inp1->len && inp2->len)
  {
    for (;;)
    {
      eq1 = (t1->m == t2->m);
      if ((t1->m < t2->m) || (eq1 && (t1->l < t2->l)))
      {
        *(tout++) = *(t1++);
        if (t1 == end1) break;
      }
      else if ((!eq1) || (eq1 && (t1->l > t2->l)))
      {
        *(tout++) = *(t2++);
        if (t2 == end2) break;
      }
      else
      {
        *(tout)    = *(t1);
        tout->coef = (t1++)->coef+(t2++)->coef;
        if (!(tout++)->coef) --tout;
        if (t1 == end1) break;
        if (t2 == end2) break;
      }
    }
  }
  if (t2 == end2) while (t1 != end1) *(tout++) = *(t1++);
  else while (t2 != end2) *(tout++) = *(t2++);
  outp->len = tout-outp->term;
}



void   p_mult(inp1, inp2, outp)
poly  *inp1;
poly  *inp2;
poly  *outp;                                      /* *outp = (*inp1)*(*inp2) */
/*  Long multiplication.  My apologies, I did not do Fourier transforms. */
/*  It is assumed that inp1, inp2, and outp point to existing poly structs */
{
  poly            xinp2[1];
  poly            temp[1];
  register poly  *blank;
  register term  *t1;
  register term  *t2;
  register term  *end1;
  register term  *end2;
  register term  *tx;

  if (inp1->len > inp2->len)
  {
    blank = inp1;
    inp1  = inp2;
    inp2  = blank;
  }
  p_init(outp);
  xinp2->term = (term *)malloc((size_t)(inp2->len*sizeof(term)));
  xinp2->len  = inp2->len;
  for (t1 = inp1->term, end1 = t1+inp1->len,
       end2 = inp2->term+inp2->len; t1 < end1; ++t1)
  {
    for (t2 = inp2->term, tx = xinp2->term; t2 < end2;)
    {
      tx->m    = p_add_pow(t1->m, t2->m);
      tx->l    = p_add_pow(t1->l, t2->l);
      (tx++)->coef = t1->coef*(t2++)->coef;
    }
    p_add(outp, xinp2, temp);
    p_kill(outp);
    *outp = *temp;
  }
  p_kill(xinp2);
}

