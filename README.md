# knn

A basic operator that finds k-nearest neighbors from a distance matrix.

## Synopsis

WRITE ME

   
## Example

WRITE ME

```
load_library('knn')
help('knn')

EXAMPLE:
iquery -aq "load_library('knn')"
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
