/* rowsort
 * Sort values in each row of a matrix, returning their rank orders.
 * I/O:
 * ans:   A pointer to storage space for the sorted output allocated by caller.
 * data:  A pointer to a block of doubles consisting of one or more full rows
 *        of a matrix. Same size as ans.
 * size:  The total number of data values.
 * nrows: The number of rows of data, must divide size.
 * zero:  If 1, then leave zeros as is. If zero then replace zeros with
          MAXDOUBLE (forcing them to be last).
 *
 * Returns 0 on success, -1 on error.
 *
 * Example input:
 *
 * 0.0  3.1  2.5  4.5  1.2
 * 4.4  0.0  0.4  0.5  1.0
 * 1.1  1.2  0.0  1.3  1.4
 *
 * Corresponding output for zero=1:
 *
 * 1.0  4.0  3.0  5.0  2.0
 * 5.0  1.0  2.0  3.0  4.0
 * 2.0  3.0  1.0  4.0  5.0
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <values.h>
#include "merge.h"

int
rowsort(double *ans, double *data, size_t size, size_t nrows, int zero)
{
  size_t j, l;
  int k = 0;
  if(size % nrows !=0) return -1; // problem not correctly shaped
  size_t dim = size/nrows;
  if(dim>MAXDOUBLE) return -1;  // problem too large
  size_t *i = (size_t *)malloc(dim * sizeof(size_t));
  if(!i) return -1;
  if(zero) memcpy(ans, data, size*sizeof(double));
  else
  {
    for(j=0;j<size;++j) ans[j] = (data[j]==0) ? MAXDOUBLE : data[j];
  }
  for(j=0;j<nrows;++j)
  {
    k = mergesort (&ans[j*dim], dim, i);
    if(k<0) break; // An error!
// Return just the order results
    for(l=0;l<dim;++l) ans[j*dim + i[l]] = l;
  }
  free(i);
  return 0;
}
