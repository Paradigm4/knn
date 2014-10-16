/**
 * @file Logicalknn.cpp
 *
 * @brief Brute force k nearest neighbors.
 *
 * @par Synopsis: knn(A, k).
 *  
 * @par Summary:
 * Simple brute force k nearest neighbor enumeration for a full distance
 * matrix. When used without a generic matrix input, it simply identifies
 * the k smallest values per row.
 * <br>
 *
 * @par Input:
 * A is a 2-d full distance matrix with one double precision-valued attribute
 * chunked only along rows k number of nearest neighbors to identify
 * <br>
 *
 * @par Output array:
 * Has same schema as A, but is sparse with the k nearest neighbors
 * enumerated.
 * <br>
 *
 * @par Examples:
 * See help('knn')
 * <br>
 *
 * @author B. W. Lewis <blewis@paradigm4.com>
 */

#include "query/Operator.h"
namespace scidb
{

class Logicalknn : public LogicalOperator
{
public:
    Logicalknn(const string& logicalName, const string& alias):
        LogicalOperator(logicalName, alias)
    {
        ADD_PARAM_INPUT()
        ADD_PARAM_CONSTANT("int32")
        _usage = "knn(A, k)\n"
                 "where:\n"
                 "  - A is a 2-d full distance matrix with one attribute chunked\n"
                 "    only along rows,\n"
                 "  - k is the number of nearest neighbors to identify per row.\n"
                 "    (if n is negative identify the -n furthest neighbors instead)\n\n"
                 "knn(A,n) returns a 2-d sparse array with schema identical to A whose\n"
                 "entries contain the rank order of the k smallest values per row, or\n"
                 "or are empty otherwise. The output martix can be joined directly with\n"
                 "the input to mask it. Note that the diagonal would normally be filtered\n"
                 "out of the solution with something like filter(knn(A,n),i<>j).\n\n" 
                 "EXAMPLE:\n"
"iquery -aq \"load_library('knn')\"\n"
"export M=\"build(<x:double>[i=1:7,7,0,j=1:7,7,0],floor(abs(sin(i-j)*513))%10)\" \n"
"Compare the test matrix:   iquery -aq \"$M\"\n"
"with the output of:        iquery -aq \"knn($M, 3)\"\n"
"looks like:\n"
"Test matrix                         knn-4 output\n"
"0 1 6 2 8 1 3                       0 1 . 3 . 2 .\n"
"1 0 1 6 2 8 1                       1 0 2 . . . 3\n"
"6 1 0 1 6 2 8                       . 1 0 2 . 3 .\n"
"2 6 1 0 1 6 2                       3 . 1 0 2 . .\n"
"8 2 6 1 0 1 6                       . 3 . 1 0 2 .\n"
"1 8 2 6 1 0 1                       1 . . . 2 0 3\n"
"3 1 8 2 6 1 0                       . 1 . 3 . 2 0\n\n";}

/* inferSchema helps the query planner decide on the shape of
 * the output array. All operators must define this function.
 */
    ArrayDesc inferSchema(vector< ArrayDesc> schemas, shared_ptr< Query> query)
    {
        ArrayDesc const& matrix = schemas[0];
        if(matrix.getAttributes(true)[0].getType() != TID_DOUBLE)
           throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) <<  "knn requires a single double precision-valued attribute";
        if(matrix.getDimensions().size() !=2 )
           throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) <<  "knn requires a matrix input";
        if (matrix.getDimensions()[1].getChunkInterval() != static_cast<int64_t>(matrix.getDimensions()[1].getLength()))
            throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << "knn does not accept column partitioning of the input matrix, use repart first";
        Attributes outputAttributes(matrix.getAttributes());
        Dimensions outputDimensions(matrix.getDimensions());
        return ArrayDesc(matrix.getName(), outputAttributes, outputDimensions);
    }
};

REGISTER_LOGICAL_OPERATOR_FACTORY(Logicalknn, "knn");

}
