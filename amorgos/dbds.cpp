
#include "dbds.h"

using namespace naxos;
using namespace std;



NsGoal* goalDbds::GOAL (void)
{
	return new goalDbdsStepping( Vars,
	                             0,
	                             varHeur, valHeur   );
}

NsGoal* goalDbdsStepping::GOAL (void)
{
	if ( depthLimit > Vars.size() )   {
		Vars[0].removeAll();	// fail
		return  0;
	}

	// cout << "depth limit: " << depthLimit << endl;

	return new NsgOR(new goalDbdsLabeling( Vars,
	                                       0,
	                                       depthLimit,
	                                       varHeur, valHeur  ),

	                 new goalDbdsStepping( Vars,
	                                       depthLimit + 1,
	                                       varHeur, valHeur  ));
}



NsGoal* goalDbdsLabeling::GOAL (void)
{
	varHeur->select(Vars);	//ITC

	if (currDepth>0) assert( Vars[currDepth-1].isBound() );

	if ( (unsigned)currDepth   ==   Vars.size() )
		return 0;		// success

	NsIntVar& V = Vars[currDepth];	// rename for convenience

	if ( currDepth   ==   depthLimit - 1 ){
		//make a discrepancy and continue by heuristic

		assert ( V.size() > 0 );

		NsInt val = valHeur->select( V );

		return new NsgAND(new NsgRemoveValue( V, val ),
		                  new NsgAND(new goalDfsInDomain( V, valHeur),
		                             new goalDbdsLabeling( Vars,
									                       currDepth+1,
														   depthLimit,
														   varHeur, valHeur)));
	}else if ( currDepth >= depthLimit ){
		// follow heuristic all the way down
		assert ( V.size() > 0 );

		return new NsgAND( new goalOnesampInDomain( V, valHeur ),
		                   new goalDbdsLabeling( Vars,
		                                         currDepth+1,
											     depthLimit,
											     varHeur, valHeur));
	}else{
		// explore all values
		return new NsgAND(new goalDfsInDomain( V, valHeur ),
		                  new goalDbdsLabeling( Vars,
						                        currDepth+1,
											    depthLimit,
											    varHeur, valHeur));
	}
}

