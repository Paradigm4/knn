#include <stdlib.h>
#include <stdio.h>

/* 
 * mergesort for doubles with ordering (stable sort).
 * Returns 0 on success, -1 on memory allocation error.
 * Arguments:
 * d: (input/output) A vector of double-precision values to be sorted.
 * n: (input) The dimension of d.
 * index: (optional input/output) Either NULL, or if non-null, then a
 *        size_t vector of dimension n to return the corresponding order values
 *        in {0, 1, ..., n}. If non-null must be allocated by caller to be large
 *        enough to hold n values.
 * Adapted from a nice compact implementation by pts@fazekas.hu, see
 http://ptspts.blogspot.it/2013/08/non-recursive-fast-compact-stable-sort.html
 * and the references therein.
 *
 * See the bottom of this file for a short example.
 */

typedef struct
{
  double v;			// value
  size_t i;			// index
} value;

int
mergesort (double *d, size_t n, size_t * index)
{
  value t;
  size_t j;
  value *tmp = (value *) malloc (n * sizeof (value));
  if (!tmp)
    return -1;
  value *a = (value *) malloc (n * sizeof (value));
  if (!a)
    {
      free (tmp);
      return -1;
    }
  for (j = 0; j < n; ++j)
    {
      a[j].v = d[j];
      a[j].i = j;
    }
  for (j = 1; j < n; j += 2)
    {
      if (a[j].v < a[j - 1].v)
	{
	  t = a[j];
	  a[j] = a[j - 1];
	  a[j - 1] = t;
	}
    }
  int s = 0;
  size_t p;
  for (p = 2; p != 0 && p < n; p <<= 1, s = !s)
    {
      value *z = tmp;
      size_t i;
      for (i = 0; i < n; i += p << 1)
	{
	  value *x = a + i;
	  value *y = x + p;
	  size_t xn = p < n - i ? p : n - i;
	  size_t yn = (p << 1) < n - i ? p : p < n - i ? n - p - i : 0;
	  if (xn > 0 && yn > 0 && (*y).v < x[xn - 1].v)
	    {
	      for (;;)
		{
		  if ((*y).v < (*x).v)
		    {
		      *z++ = *y++;
		      if (--yn == 0)
			break;
		    }
		  else
		    {
		      *z++ = *x++;
		      if (--xn == 0)
			break;
		    }
		}
	    }
	  while (xn > 0)
	    {
	      *z++ = *x++;
	      --xn;
	    }
	  while (yn > 0)
	    {
	      *z++ = *y++;
	      --yn;
	    }
	}
      z = a;
      a = tmp;
      tmp = z;
    }
  if (s)
    {
      value *x, *y;
      size_t k;
      x = tmp;
      y = a;
      for (k = 0; k < n; ++k)
	*x++ = *y++;
    }
  free (tmp);
  for (j = 0; j < n; ++j)
    {
      d[j] = a[j].v;
      if (index)
	index[j] = a[j].i;
    }
  return 0;
}

/* Example use
#include "merge.h"

int
main (int argc, char **argv)
{
  int j;
  double a[] = { 5, 1, 3, 6, 9, 2, 2 };
  size_t *i = (size_t *) malloc (7 * sizeof (size_t));
  for (j = 0; j < 7; ++j)
    {
      printf ("a[%d]=%f\n", j, a[j]);
      i[j] = j;
    }
  printf ("\n");
  mergesort (a, 7, i);
  for (j = 0; j < 7; ++j)
    printf ("a[%d]=%f i[%d]=%lu\n", j, a[j], j, i[j]);
  return 0;
}
*/

/* The output of the above example program should look like:
a[0]=5.000000
a[1]=1.000000
a[2]=3.000000
a[3]=6.000000
a[4]=9.000000
a[5]=2.000000
a[6]=2.000000

a[0]=1.000000 i[0]=1
a[1]=2.000000 i[1]=5
a[2]=2.000000 i[2]=6
a[3]=3.000000 i[3]=2
a[4]=5.000000 i[4]=0
a[5]=6.000000 i[5]=3
a[6]=9.000000 i[6]=4
*/
