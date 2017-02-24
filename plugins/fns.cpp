#include "fns.h"
#include "heuristics.h"
#include "ibroad.h"

using namespace naxos;
using namespace std;

NsGoal* AmFnsLabeling::GOAL(void)
{
        int i, level, breadthLimit, n;
        level = 0;
        n = Vars.size();
        // what level of the search tree are we on?	(0 - n-1)
        for (i = 0; i < n; ++i)
                if (Vars[i].isBound())
                        level++;
        int index = varHeur->select(Vars);
        if (index == -1)
                return 0; // all variables are bound => success
        // breadthLimit for this level
        breadthLimit = f(Vars[index].size(), level, n);
        return (
            new NsgAND(new AmIbroadInDomain(Vars[index], breadthLimit, valHeur),
                       new AmFnsLabeling(*this)));
}

namespace naxos {

int fSample(int nChildren, int depth, int treeHeight)
{
        // if close to leaves OR close to root, explore all children
        if (depth > treeHeight - 5 || depth < 5)
                return nChildren;
        // else only the first one
        else
                return 1;
}

int fSampleMiddle(int nChildren, int depth, int treeHeight)
{
        // if close to middle, explore all children
        if (depth > treeHeight / 2 - 3 && depth <= treeHeight / 2 + 3)
                return nChildren;
        // else only the first one
        else
                return 1;
}

int fSampleExp(int nChildren, int depth, int treeHeight)
{
        treeHeight = treeHeight; // prevent gcc warning (unused parameter)
        double percentage = pow(2.0, -depth);
        double retvalDouble = nChildren * percentage;
        int retval = (int)floor(retvalDouble) + 1;
        return retval <= nChildren ? retval : nChildren;
}
} // end namespace
