#include "rdfs.h"
#include "heuristics.h"

using namespace naxos;
using namespace std;



NsGoal*  goalRdfs::GOAL (void)
{
	int  *timeout = new int(timeoutLimit);
	return (new NsgOR(new goalRdfsStepping(Vars,times,timeout,
	                                       timeoutLimit,varHeur,valHeur),
	                  new goalRdfsDestructor(Vars,timeout)));
}


NsGoal* goalRdfsStepping::GOAL (void)
{
	if (times == 0)   {
		Vars[0].removeAll();	// fail
		return  0;
	}

	// cout << "tries left: " << times << endl;

	*timeout = timeoutLimit;

	return(new NsgOR(new goalRdfsLabeling(Vars,timeout,varHeur,valHeur),
	                 new goalRdfsStepping(Vars,times-1,timeout,
					                      timeoutLimit,varHeur,valHeur)));
}


NsGoal* goalRdfsLabeling::GOAL (void)
{
	(*timeout)--;

	if(*timeout <= 0){
		Vars[0].removeAll();	//fail
		return 0;
	}

	int  index = varHeur->select(Vars);

	if (index == -1)
		return  0;		// all variables are bound => success


	return(new NsgAND(new goalRdfsInDomain(Vars[index], timeout, valHeur),
	                  new goalRdfsLabeling(Vars, timeout, varHeur, valHeur)));
}

// we need this in order to check timeout every time
NsGoal* goalRdfsInDomain::GOAL(void)
{
	(*timeout)--;

	if(*timeout <= 0){
		Var.removeAll();	//fail
		return 0;
	}

	if (Var.isBound())
		return 0;
	NsInt value = valHeur->select(Var);
	
	return ( new NsgOR( new NsgSetValue(Var,value) ,
	                    new NsgAND( new NsgRemoveValue(Var,value) ,
	                                new goalRdfsInDomain(*this))));
}


