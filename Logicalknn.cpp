/**
 * @file Logicalknn.cpp
 *
 * @brief The operator: knn.
 *
 * @par Synopsis: knn(array, val).
 *  
 * @par Summary:
 *   <br>
 *
 * @par Input:
 *
 * @par Output array:
 *
 * @par Examples:
 *
 * @author some dudes.
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
        _usage = "knn(A, n)\n"
                 "where:\n"
                 "A is a 2-d full distance matrix with one attribute chunked\n"
                 "only along rows,\n"
                 "n is the number of nearest neighbors to identify.\n\n"
                 "knn(A,n) returns a 2-d sparse array of the same schema\n"
                 "as the input, enumerating XXX write me\n\n."
                 "EXAMPLE:\n"
                 "iquery -aq \"load_library('knn')\"\n";
    }

/* inferSchema helps the query planner decide on the shape of
 * the output array. All operators must define this function.
 */
    ArrayDesc inferSchema(vector< ArrayDesc> schemas, shared_ptr< Query> query)
    {
        ArrayDesc const& inputSchema = schemas[0];
        Attributes outputAttributes(inputSchema.getAttributes(true));
        Dimensions outputDimensions(inputSchema.getDimensions());
        return ArrayDesc(inputSchema.getName(), outputAttributes, outputDimensions);
    }
};

REGISTER_LOGICAL_OPERATOR_FACTORY(Logicalknn, "knn");

}
