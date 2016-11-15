
  //  Naxos Solver:  A Constraint Programming Library         //
 //  Copyright 2007-2013  Nikolaos Pothitos                  //
//  See ../license/LICENSE for the license of the library.  //





#include <naxos.h>

#include <iostream>
#include <cstdlib>

using namespace std;
using namespace naxos;


int  main (int argc, char *argv[])
{
    try  {

      //time_t  timeBegin=time(0);


      int  N = (argc > 1) ? atoi(argv[1]) : 8;
      NsProblemManager  pm;

      NsIntVarArray  Var, VarPlus, VarMinus;
      for (int i=0;  i<N;  ++i)   {
          Var.push_back( NsIntVar(pm, 0, N-1) );
          VarPlus.push_back(  Var[i] + i );
          VarMinus.push_back( Var[i] - i );
      }
      pm.add( NsAllDiff(Var) );
      pm.add( NsAllDiff(VarPlus) );
      pm.add( NsAllDiff(VarMinus) );

      pm.addGoal( new NsgLabeling(Var) );

      //bool  AcMode = true;//( string(argv[0]).find(".AC") != string::npos );

      unsigned  solutions=0;
      //if ( AcMode )
      //    cout << N << "\t" << solutions << "\t";
      //cout << difftime(time(0),timeBegin) << "\n";
      while ( pm.nextSolution()  !=  false )   {
	      ++solutions;	//watch solutions==9798635

          //if ( ++solutions % 1000 == 0 )   {
          //    if ( AcMode )
          //        cout << N << "\t" << solutions << "\t";
          //    cout << difftime(time(0),timeBegin) << "\n";
          //}
          //if ( solutions  >=  14770000 )
              //cout << "Solution: " << Var << "\n";
      }

      //cout << difftime(time(0),timeBegin) << "\tX\t";
      //pm.printCspParameters();

    } catch (exception& exc)  {
	cerr << exc.what() << "\n";

    } catch (...)  {
	cerr << "Unknown exception" << "\n";
    }
}
