// Part of https://github.com/pothitos/naxos

#ifndef HEURISTICS_H
#define HEURISTICS_H

#include "timetabling.h"
#include <amorgos.h>
#include <naxos.h>

class VarHeurTimetabling : public naxos::VariableHeuristic {

    private:
        naxos::NsIntVarArray& vLectRoom;
        const struct itcProblem_t& pr;
        const naxos::NsDeque<struct itcLectureInfo_t>& lectInfo;
        const naxos::NsDeque<struct itcCourseInfo_t>& courseInfo;
        naxos::NsDeque<unsigned>& lanAssigns;

        const naxos::NsIntVarArray& vRooms;
        const naxos::NsIntVarArray& vBusyDays;

        int roundRobinVarPosition;

    public:
        VarHeurTimetabling(
            naxos::NsIntVarArray& vLectRoom_init,
            const struct itcProblem_t& pr_init,
            const naxos::NsDeque<struct itcLectureInfo_t>& lectInfo_init,
            const naxos::NsDeque<struct itcCourseInfo_t>& courseInfo_init,
            naxos::NsDeque<unsigned>& lanAssigns_init,
            const naxos::NsIntVarArray& vRooms_init,
            const naxos::NsIntVarArray& vBusyDays_init)
          : vLectRoom(vLectRoom_init),
            pr(pr_init),
            lectInfo(lectInfo_init),
            courseInfo(courseInfo_init),
            lanAssigns(lanAssigns_init),
            vRooms(vRooms_init),
            vBusyDays(vBusyDays_init),
            roundRobinVarPosition(0)
        {
        }

        int select(const naxos::NsIntVarArray& vLectPeriod);
};

class ValHeurTimetabling : public naxos::ValueHeuristic {

    private:
        const naxos::NsIntVarArray& vLectPeriod;
        const naxos::NsIntVarArray& vLectRoom;

        const struct itcProblem_t& pr;
        const naxos::NsDeque<struct itcLectureInfo_t>& lectInfo;
        const naxos::NsDeque<struct itcCourseInfo_t>& courseInfo;

        const naxos::NsIntVarArray& vRooms;
        const naxos::NsDeque<naxos::NsIntVarArray>& vLectDay;
        const naxos::NsDeque<naxos::NsIntVarArray>& vIsBusyDay;
        const naxos::NsIntVarArray& vBusyDays;
        const naxos::NsDeque<naxos::NsIntVarArray>& vGroup;
        const naxos::NsDeque<naxos::NsIntVarArray>& vIsolatedLect;

    public:
        ValHeurTimetabling(
            const naxos::NsIntVarArray& vLectPeriod_init,
            const naxos::NsIntVarArray& vLectRoom_init,
            const struct itcProblem_t& pr_init,
            const naxos::NsDeque<struct itcLectureInfo_t>& lectInfo_init,
            const naxos::NsDeque<struct itcCourseInfo_t>& courseInfo_init,
            const naxos::NsIntVarArray& vRooms_init,
            const naxos::NsDeque<naxos::NsIntVarArray>& vLectDay_init,
            const naxos::NsDeque<naxos::NsIntVarArray>& vIsBusyDay_init,
            const naxos::NsIntVarArray& vBusyDays_init,
            const naxos::NsDeque<naxos::NsIntVarArray>& vGroup_init,
            const naxos::NsDeque<naxos::NsIntVarArray>& vIsolatedLect_init)
          : vLectPeriod(vLectPeriod_init),
            vLectRoom(vLectRoom_init),
            pr(pr_init),
            lectInfo(lectInfo_init),
            courseInfo(courseInfo_init),
            vRooms(vRooms_init),
            vLectDay(vLectDay_init),
            vIsBusyDay(vIsBusyDay_init),
            vBusyDays(vBusyDays_init),
            vGroup(vGroup_init),
            vIsolatedLect(vIsolatedLect_init)
        {
        }

        naxos::NsInt select(const naxos::NsIntVar& V);
        naxos::NsInt select(const naxos::NsIntVar& V, double& piece);
};

class SplitValHeurTimetabling : public naxos::SplitValHeuristic {

    private:
        const naxos::NsIntVarArray& vLectPeriod;
        const naxos::NsIntVarArray& vLectRoom;

        const struct itcProblem_t& pr;
        const naxos::NsDeque<struct itcLectureInfo_t>& lectInfo;
        const naxos::NsDeque<struct itcCourseInfo_t>& courseInfo;

        const naxos::NsIntVarArray& vRooms;

    public:
        SplitValHeurTimetabling(
            const naxos::NsIntVarArray& vLectPeriod_init,
            const naxos::NsIntVarArray& vLectRoom_init,
            const struct itcProblem_t& pr_init,
            const naxos::NsDeque<struct itcLectureInfo_t>& lectInfo_init,
            const naxos::NsDeque<struct itcCourseInfo_t>& courseInfo_init,
            const naxos::NsIntVarArray& vRooms_init)
          : vLectPeriod(vLectPeriod_init),
            vLectRoom(vLectRoom_init),
            pr(pr_init),
            lectInfo(lectInfo_init),
            courseInfo(courseInfo_init),
            vRooms(vRooms_init)
        {
        }

        double select(const naxos::NsIntVar& V);
};

double heurEstimationForVariable(
    const int index, const int courseIndex,
    const naxos::NsDeque<struct itcCourseInfo_t>& courseInfo,
    const naxos::NsIntVarArray& vLectPeriod, const struct itcProblem_t& pr,
    const naxos::NsIntVarArray& vBusyDays);

int unboundVarsConnecedTo(
    const int courseIndex,
    const naxos::NsDeque<struct itcCourseInfo_t>& courseInfo,
    const naxos::NsIntVarArray& vLectPeriod, const struct itcProblem_t& pr);

int varHeurObjectiveCriterion(
    const int courseIndex,
    const naxos::NsDeque<struct itcCourseInfo_t>& courseInfo,
    const struct itcProblem_t& pr, const naxos::NsIntVarArray& vBusyDays);
#endif // HEURISTICS_H
