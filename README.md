# knn

A simple brute force k-nearest neighbors implementation for SciDB.
This operator finds the k nearest neighbors for each row of a full
distance matrix.

When used with a generic input matrix (not a distance matrix),
the operator simply identifies the k smallest values per row.

## Synopsis

Usage: knn(A, k)
where:

* A is a 2-d full distance matrix with one double precision-valued attribute chunked only along rows,
* k is the number of nearest neighbors to identify per row.

knn(A,n) returns a 2-d sparse array with schema identical to A whose
entries contain the rank order of the k smallest values per row, or
or are empty otherwise. The output matrix can be joined directly with
the input to mask it. Note that the diagonal would normally be filtered
out of the solution with something like filter(knn(A,n),i&lt;&gt;j).

## Example
```
load_library('knn')

# Create a small example symmetric test matrix:
export M="build(<x:double>[i=1:7,7,0,j=1:7,7,0],floor(abs(sin(i-j)*513))%10)" 

Compare the test matrix:   iquery -aq "$M"
with the output of:        iquery -aq "knn($M, 4)"

Looks like:

Test matrix                         knn-4 output
0 1 6 2 8 1 3                       0 1 . 3 . 2 .
1 0 1 6 2 8 1                       1 0 2 . . . 3
6 1 0 1 6 2 8                       . 1 0 2 . 3 .
2 6 1 0 1 6 2                       3 . 1 0 2 . .
8 2 6 1 0 1 6                       . 3 . 1 0 2 .
1 8 2 6 1 0 1                       1 . . . 2 0 3
3 1 8 2 6 1 0                       . 1 . 3 . 2 0
```
   

## Installing the plug in

You'll need SciDB installed, along with the SciDB development header packages.
The names vary depending on your operating system type, but they are the
package that have "-dev" in the name. You *don't* need the SciDB source code to
compile and install this.

Run `make` and copy  the `libknn.so` plugin to the `lib/scidb/plugins`
directory on each of your SciDB cluster nodes. Here is an example:

```
cd knn
make
cp libknn.so /opt/scidb/14.3/lib/scidb/plugins

iquery -aq "load_library('knn')"
```
Remember to copy the plugin to all your SciDB cluster nodes.



