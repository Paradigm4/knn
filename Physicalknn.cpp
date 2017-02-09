/*
 *    _____      _ ____  ____
 *   / ___/_____(_) __ \/ __ )
 *   \__ \/ ___/ / / / / __  |
 *  ___/ / /__/ / /_/ / /_/ /
 * /____/\___/_/_____/_____/
 *
 *
 * BEGIN_COPYRIGHT
 *
 * This file is part of SciDB.
 * Copyright (C) 2008-2014 SciDB, Inc.
 *
 * SciDB is free software: you can redistribute it and/or modify
 * it under the terms of the AFFERO GNU General Public License as published by
 * the Free Software Foundation.
 *
 * SciDB is distributed "AS-IS" AND WITHOUT ANY WARRANTY OF ANY KIND,
 * INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY,
 * NON-INFRINGEMENT, OR FITNESS FOR A PARTICULAR PURPOSE. See
 * the AFFERO GNU General Public License for the complete license terms.
 *
 * You should have received a copy of the AFFERO GNU General Public License
 * along with SciDB.  If not, see <http://www.gnu.org/licenses/agpl-3.0.html>
 *
 * END_COPYRIGHT
 */
#include "query/Operator.h"
#include "rowsort.h"

using namespace std;

namespace scidb
{

class Physicalknn : public PhysicalOperator
{
public:
    Physicalknn(string const& logicalName,
                string const& physicalName,
                Parameters const& parameters,
                ArrayDesc const& schema):
        PhysicalOperator(logicalName, physicalName, parameters, schema)
    {}

    /**
      * [Optimizer API] Determine if operator changes result chunk distribution.
      * @param sourceSchemas shapes of all arrays that will given as inputs.
      * @return true if will changes output chunk distribution, false if otherwise
      */
    virtual bool changesDistribution(std::vector<ArrayDesc> const& sourceSchemas) const
    {
        return false;
    }

    /* The instance-parallel 'main' routine of this operator.
       This runs on each instance in the SciDB cluster.
     */
    shared_ptr< Array> execute(vector< shared_ptr< Array> >& inputArrays, shared_ptr<Query> query)
    {
        shared_ptr<Array> inputArray = inputArrays[0];
        shared_ptr<ConstArrayIterator> arrayIter(inputArray->getConstIterator(0));
        shared_ptr<ConstChunkIterator> chunkIter;
        shared_ptr<Array> output(new MemArray(inputArray->getArrayDesc(), query));
        shared_ptr<ArrayIterator> outputArrayIterator(output->getIterator(0));
        int n = ((boost::shared_ptr<OperatorParamPhysicalExpression>&)_parameters[0])->getExpression()->evaluate().getInt32();
        while (!arrayIter->end())
        {
            chunkIter = arrayIter->getChunk().getConstIterator(0);
            Coordinates start = chunkIter->getPosition();
            int64_t last_row = start[0];
            vector<double> chunkdata;
            for(;;)
            {
                if(!chunkIter->end())
                {
                    Value const& val = chunkIter->getItem();
                    Coordinates coords = chunkIter->getPosition();
                    chunkdata.push_back(val.getDouble());
                    last_row = coords[0];
                    ++(*chunkIter);
                }
                if(chunkIter->end()) break;
            }
// Compute neighbor orders for each row in this block
            double *ans = (double *)malloc(chunkdata.size() * sizeof(double));
            if(!ans) throw SYSTEM_EXCEPTION(SCIDB_SE_OPERATOR, SCIDB_LE_ILLEGAL_OPERATION)
                        << "memory allocation error";
            rowsort(ans, chunkdata.data(), chunkdata.size(), last_row-start[0]+1);
// write the output (has same schema as input)
            shared_ptr<ChunkIterator> outputChunkIter = outputArrayIterator->newChunk(start).getIterator(query, ChunkIterator::SEQUENTIAL_WRITE);
            size_t j = 0;
            Value val;
            for(;;)
            {
                if(!outputChunkIter->end())
                {
                    if(&ans[j]!=0)
                    {
                      val.setData(&ans[j],sizeof(double));
                      if(ans[j] < n) outputChunkIter->writeItem(val);
                    }
                    ++j;
                    ++(*outputChunkIter);
                }
                if(outputChunkIter->end() || j>chunkdata.size()) break;
            }
            free(ans);
            outputChunkIter->flush();
            if(outputChunkIter) outputChunkIter->restart();
// Advance the array iterators in lock step
            ++(*arrayIter);
            ++(*outputArrayIterator);
        }
        return output;
    }
};
REGISTER_PHYSICAL_OPERATOR_FACTORY(Physicalknn, "knn", "Physicalknn");
} //namespace scidb
