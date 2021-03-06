/*
------------------------------------------------------------------------------
 dllink.c - routines for dealing with doubly linked, circular lists
 Bob Jenkins, 1990.  Public Domain.
------------------------------------------------------------------------------
*/
#ifndef STANDARD
#include "standard.h"
#endif
#ifndef DLLINK
#include "dllink.h"
#endif

/*
------------------------------------------------------------------------------
  This displays all the nodes in a single string
------------------------------------------------------------------------------
*/
void   l_show(l)
register dllink *l;
{
  register dllink  *count;

  if ((count = l) != 0)
  {
    printf("%d ", count->c);
    count = count->z;
    while (count != l)
    {
      printf("%d ", count->c);
      count = count->z;
    }
  }
  printf("\n");
}


/*
------------------------------------------------------------------------------
    This takes a dllink inp, adds a dllink before it, and places a
    pointer to the added dllink in outp
------------------------------------------------------------------------------
*/
void   l_add(inp,crossings,outp)
register dllink *inp;
word             crossings;
dllink         **outp;
{
  register dllink  *newlink;

  newlink    = (dllink *)malloc(sizeof(dllink));
  newlink->c = crossings;
  if (inp == 0)
  {
    newlink->a = newlink;
    newlink->z = newlink;
    inp        = newlink;
  }
  else
  {
    newlink->a = inp->a;
    newlink->z = inp;
    inp->a->z  = newlink;
    inp->a     = newlink;
  }
  *outp = inp->a;
}




/*
------------------------------------------------------------------------------
  This deletes a given dllink from a loop
------------------------------------------------------------------------------
*/
void   l_del(l)
dllink **l;
{
  register dllink  *ll = *l;

  if (!ll) return;
  if (ll->a != ll)
  {
    ll->z->a = ll->a;
    ll->a->z = ll->z;
  }
  free((char *)ll);
  *l = (dllink *)0;
}
