// Part of https://github.com/pothitos/naxos

#include <naxos.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
//#include "strtok.h"

using namespace std;
using namespace naxos;

int
main (int argc, char *argv[])
{
        try {
                //  Checking input arguments.
                if ( argc != 2  &&  argc != 3 ) {
                        cerr << "Correct syntax is:\t" << argv[0]
                             << " <flight_data_file> [precision]\n";
                        return  1;
                }
                double  precision = ( (argc==3)? atof(argv[2]) : 1.0 );
                ////  Opening input filename, e.g. `flight_data.pl'.
                //ifstream  file(argv[1]);
                //if ( ! file )   {
                //  cerr << "Could not open `" << argv[1] << "'\n";
                //  return  1;
                //}
                //file.exceptions( ifstream::eofbit | ifstream::failbit | ifstream::badbit );
                string  input;
                //getline(file, input);	// Ignore the first line.
                //for (int I=1;  I <= 16;  ++I)   {
                //cout << "I = " << I << "\n";
                unsigned  pairing, flight;
                NsInt                      N;
                NsDeque< NsDeque<NsInt> >  P;
                NsDeque<NsInt>             C;
                //if ( I  <=  6 )    {
                //	cout << "CS" << I << "\t";
                //	string  tok;
                //	file >> input;	// First line is `get_flight_data(1,10,'.
                //	StrTokenizer  first_tokens(input, ",");
                //	first_tokens.next();	// First token is ignored.
                //	tok = first_tokens.next();
                //	N  =  atoi(tok.c_str());
                //	while ( file >> input )    {
                //		StrTokenizer  tokens(input, ",] ");
                //		while ( ! (tok = tokens.next()).empty()
                //				&& tok != ")." )
                //		{
                //			if ( tok[0]  ==  '[' )   {
                //				P.push_back( NsDeque<NsInt>() );
                //				tok = tok.substr(1);
                //				if ( tok[0]  ==  '[' )
                //					tok = tok.substr(1);
                //			}
                //			P.back().push_back(atoi(tok.c_str()));
                //		}
                //		if ( tok  ==  ")." )
                //			break;
                //	}
                //	C = P.back();
                //	P.pop_back();
                //}  else  {
                //	string  filename;
                //	switch ( I )   {
                //		case  7:
                //			cout << "NW41\t";
                //			filename = "CSPs/OR-Library/acs197x17";
                //			break;
                //		case  8:
                //			cout << "NW32\t";
                //			filename = "CSPs/OR-Library/acs294x19";
                //			break;
                //		case  9:
                //			cout << "NW40\t";
                //			filename = "CSPs/OR-Library/acs404x19";
                //			break;
                //		case  10:
                //			cout << "NW08\t";
                //			filename = "CSPs/OR-Library/acs434x24";
                //			break;
                //		case  11:
                //			cout << "NW15\t";
                //			filename = "CSPs/OR-Library/acs467x31";
                //			break;
                //		case  12:
                //			cout << "NW21\t";
                //			filename = "CSPs/OR-Library/acs577x25";
                //			break;
                //		case  13:
                //			cout << "NW22\t";
                //			filename = "CSPs/OR-Library/acs619x23";
                //			break;
                //		case  14:
                //			cout << "NW12\t";
                //			filename = "CSPs/OR-Library/acs626x27";
                //			break;
                //		case  15:
                //			cout << "NW20\t";
                //			filename = "CSPs/OR-Library/acs711x19";
                //			break;
                //		case  16:
                //			cout << "NW23\t";
                //			filename = "CSPs/OR-Library/acs685x22";
                //			break;
                //		default:
                //			cerr << "Wrong `I'\n";
                //			return  1;
                //			break;
                //	}
                ifstream  fileAcs(argv[1]);
                if ( ! fileAcs ) {
                        cerr << "Could not open `" << argv[1] << "'\n";
                        return  1;
                }
                fileAcs.exceptions( ifstream::eofbit | ifstream::failbit | ifstream::badbit );
                fileAcs >> N;
                unsigned   M;
                fileAcs >> M;
                ////  Ignoring three tokens.
                //fileAcs >> input;
                //fileAcs >> input;
                //fileAcs >> input;
                NsInt  number;
                for (pairing=0;  pairing < M;  ++pairing) {
                        fileAcs >> number;
                        C.push_back(number*precision + 0.5);
                        P.push_back( NsDeque<NsInt>() );
                        unsigned  K;
                        fileAcs >> K;
                        ////  Ignoring three tokens.
                        //fileAcs >> input;
                        //fileAcs >> input;
                        //fileAcs >> input;
                        for (flight=0;  flight < K;  ++flight) {
                                fileAcs >> number;
                                P.back().push_back(number);
                        }
                }
                fileAcs.close();
                //}
                //  Print N, P, C.
                /*cout << "N = " << N << "\n";
                cout << "P = [";
                for (pairing=0;  pairing < P.size();  ++pairing)    {

                	if ( pairing > 0 )
                		cout << ",\n     ";

                	cout << "[";
                	for (flight=0;  flight < P[pairing].size();  ++flight)   {
                		if ( flight > 0 )
                			cout << ", ";
                		cout << P[pairing][flight];
                	}
                	cout << "]";

                	cout << " / " << C[pairing];
                }
                cout << "]\n";*/
                NsProblemManager  pm;
                NsDeque< NsIntVarArray >  vFlightScheduledWithP;
                for (flight=0;  flight < N;  ++flight)
                        vFlightScheduledWithP.push_back( NsIntVarArray() );
                NsIntVarArray  vCostTerms;
                NsIntVarArray  vIncludePairing;
                for (pairing=0;  pairing < P.size();  ++pairing) {
                        vIncludePairing.push_back( NsIntVar(pm,0,1) );
                        for (flight=0;  flight < P[pairing].size();  ++flight) {
                                NsInt  f = P[pairing][flight] - 1;
                                vFlightScheduledWithP[f].push_back( vIncludePairing[pairing] );
                        }
                        vCostTerms.push_back( C[pairing] * vIncludePairing[pairing] );
                }
                for (flight=0;  flight < N;  ++flight)
                        pm.add( NsSum(vFlightScheduledWithP[flight]) == 1 );
                NsIntVar  vCost = NsSum(vCostTerms);
                pm.minimize( vCost );
                pm.addGoal( new NsgLabeling(vIncludePairing) );
                //pm.printCspParameters();
                //continue;
                NsDeque<NsInt>  bestIncludePairing(vIncludePairing.size());
                NsInt           bestCost = -1;
                //double          bestTime = -1;
                while ( pm.nextSolution()  !=  false ) {
                        //bestTime  =  difftime(time(0),timeBegin);
                        //  Recording the (current) best solution.
                        for (pairing=0;  pairing < P.size();  ++pairing)
                                bestIncludePairing[pairing] = vIncludePairing[pairing].value();
                        bestCost  =  vCost.value();
                }
                pm.printCspParameters();
                ////  Printing the best solution.
                //if ( bestCost  !=  -1 )    {
                //	bool  firstPairing = true;
                //	cout << "Pairings = [";
                //	for (pairing=0;  pairing < P.size();  ++pairing)    {
                //		if ( bestIncludePairing[pairing] == 1 )   {
                //			if ( firstPairing )
                //				firstPairing  =  false;
                //			else
                //				cout << ",\n            ";
                //			cout << "[";
                //			for (flight=0;  flight < P[pairing].size();  ++flight)   {
                //				if ( flight > 0 )
                //					cout << ", ";
                //				cout << P[pairing][flight];
                //			}
                //			cout << "]";
                //			cout << " / " << C[pairing];
                //		}
                //	}
                //	cout << "]\n";
                //	cout << "Cost = " << bestCost << "\n";
                //}
                //cout << "\n";
                //}
                //file.close();
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
        } catch (...) {
                cerr << "Unknown exception" << "\n";
        }
}
