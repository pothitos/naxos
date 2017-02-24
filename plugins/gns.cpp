#include "gns.h"
#include "heuristics.h"
#include "ibroad.h"

using namespace naxos;
using namespace std;

NsGoal* AmGnsLabeling::GOAL(void)
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
        // calculate breadthLimit for this level
        // breadthLimit = ceil( Vars[index].size() *
        // ((double)(n-level)/(double)n) );
        breadthLimit = (Vars[index].size() * (n - level) - 1) / n + 1;
        return (
            new NsgAND(new AmIbroadInDomain(Vars[index], breadthLimit, valHeur),
                       new AmGnsLabeling(*this)));
}
