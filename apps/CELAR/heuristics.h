// Part of https://github.com/pothitos/naxos

#ifndef HEURISTICS_H
#define HEURISTICS_H

#include <stdexcept>
#include <string>

class CelarException : public std::logic_error {

    public:
        CelarException(const std::string& error)
          : logic_error("CELAR: " + error)
        {
        }
};

// Panic Function //
inline void assert_that(const bool test, const char* error)
// Here we used the type `char*' for `error_message', instead
//  of `string', plainly for time-performance reasons.
{
        if (!test)
                throw CelarException(error);
}

#include <amorgos.h>
#include <naxos.h>

struct CelarInfo {

        naxos::NsDeque<naxos::NsIntVarArray> varsConnected;
};

class VarHeurCelar : public naxos::VariableHeuristic {

    private:
        const CelarInfo& info;

        double conf;

    public:
        VarHeurCelar(const CelarInfo& info_init, const double conf_init = -1)
          : info(info_init), conf(conf_init)
        {
        }

        int select(const naxos::NsIntVarArray& Vars);
};

class ValHeurCelar : public naxos::ValueHeuristic {

    private:
        const naxos::NsIntVarArray& Vars;

        const CelarInfo& info;

        double conf;

    public:
        ValHeurCelar(const naxos::NsIntVarArray& Vars_init,
                     const CelarInfo& info_init, const double conf_init = -1)
          : Vars(Vars_init), info(info_init), conf(conf_init)
        {
        }

        naxos::NsInt select(const naxos::NsIntVar& V);
};
#endif // HEURISTICS_H
