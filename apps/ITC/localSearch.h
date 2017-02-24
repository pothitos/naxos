// Part of https://github.com/pothitos/naxos

#ifndef LOCALSEARCH_H
#define LOCALSEARCH_H

#include "heuristics.h"
#include "timetabling.h"
#include <amorgos.h>
#include <naxos.h>

#if 0
class goalLocalSearch : public naxos::NsGoal {
    private:
        naxos::NsIntVarArray&  Vars;
        naxos::NsIntVarArray&  classVars;

        struct course_t  *course_arg;
        int  ncourse;

        const naxos::NsDeque<CLectureInfo>&  vi;
        struct setup_t&  setup;
        const int  days, hours;
        int& cancel;
        int& progress;

        int  currentWidthBound;

    public:
        goalLocalSearch (naxos::NsIntVarArray& Vars_init, naxos::NsIntVarArray& classVars_init,
                         struct course_t course_arg_init[], int ncourse_init,
                         const naxos::NsDeque<CLectureInfo>& vi_init, struct setup_t& setup_init, int d, int h,
                         int& cancel_init, int& progress_init);

        NsGoal  *GOAL (void);
};
#endif

class goalLsWeek : public naxos::NsGoal {

    private:
        naxos::NsIntVarArray& vLectPeriod;
        naxos::NsIntVarArray& vLectPeriodSortedByHeur;
        naxos::NsIntVarArray& vLectRoom;

        struct itcProblem_t& pr;

        naxos::VariableHeuristic& varHeur;
        naxos::ValueHeuristic& valHeur;
        naxos::NsDeque<unsigned>& lanAssigns;

        int currentDay;

    public:
        goalLsWeek(naxos::NsIntVarArray& vLectPeriod_init,
                   naxos::NsIntVarArray& vLectPeriodSortedByHeur_init,
                   naxos::NsIntVarArray& vLectRoom_init,
                   struct itcProblem_t& pr_init,
                   naxos::VariableHeuristic& varHeur_init,
                   naxos::ValueHeuristic& valHeur_init,
                   naxos::NsDeque<unsigned>& lanAssigns_init,
                   const int currentDay_init)
          : vLectPeriod(vLectPeriod_init),
            vLectPeriodSortedByHeur(vLectPeriodSortedByHeur_init),
            vLectRoom(vLectRoom_init),
            pr(pr_init),
            varHeur(varHeur_init),
            valHeur(valHeur_init),
            lanAssigns(lanAssigns_init),
            currentDay(currentDay_init)
        {
        }

        NsGoal* GOAL(void);
};

class goalLsDay : public naxos::NsGoal {

    private:
        naxos::NsIntVarArray& vLectPeriod;
        naxos::NsIntVarArray& vLectPeriodSortedByHeur;
        naxos::NsIntVarArray& vLectRoom;

        struct itcProblem_t& pr;

        naxos::VariableHeuristic& varHeur;
        naxos::ValueHeuristic& valHeur;
        naxos::NsDeque<unsigned>& lanAssigns;

        const int currentDay;

    public:
        goalLsDay(naxos::NsIntVarArray& vLectPeriod_init,
                  naxos::NsIntVarArray& vLectPeriodSortedByHeur_init,
                  naxos::NsIntVarArray& vLectRoom_init,
                  struct itcProblem_t& pr_init,
                  naxos::VariableHeuristic& varHeur_init,
                  naxos::ValueHeuristic& valHeur_init,
                  naxos::NsDeque<unsigned>& lanAssigns_init,
                  const int currentDay_init)
          : vLectPeriod(vLectPeriod_init),
            vLectPeriodSortedByHeur(vLectPeriodSortedByHeur_init),
            vLectRoom(vLectRoom_init),
            pr(pr_init),
            varHeur(varHeur_init),
            valHeur(valHeur_init),
            lanAssigns(lanAssigns_init),
            currentDay(currentDay_init)
        {
        }

        NsGoal* GOAL(void);
};

#if 0
class goalLsSlices : public naxos::NsGoal {
    private:
        naxos::NsIntVarArray&  Vars;
        naxos::NsIntVarArray&  classVars;

        struct course_t  *course_arg;
        int  ncourse;

        const naxos::NsDeque<CLectureInfo>&  vi;
        const struct setup_t&  setup;
        const int  days, hours;
        int& cancel;
        int& progress;

        int  currentWidth;
        int  currentPosition;
        int  currentWidthBound;

    public:
        goalLsSlices (naxos::NsIntVarArray& Vars_init, naxos::NsIntVarArray& classVars_init,
                      struct course_t course_arg_init[], int ncourse_init,
                      const naxos::NsDeque<CLectureInfo>& vi_init, const struct setup_t& setup_init, int d, int h,
                      int& cancel_init, int& progress_init, int currentWidthBound_init)
                : Vars(Vars_init), classVars(classVars_init), course_arg(course_arg_init), ncourse(ncourse_init),
                  vi(vi_init), setup(setup_init), days(d), hours(h),
                  cancel(cancel_init), progress(progress_init),
                  currentWidth(2), currentPosition(0), currentWidthBound(currentWidthBound_init)    {    }

        NsGoal  *GOAL (void);
};

class goalLsOneSlice : public naxos::NsGoal {
    private:
        naxos::NsIntVarArray&  Vars;
        naxos::NsIntVarArray&  classVars;

        struct course_t  *course_arg;
        int  ncourse;

        const naxos::NsDeque<CLectureInfo>&  vi;
        const struct setup_t&  setup;
        const int  days, hours;
        int& cancel;
        int& progress;

        int  currentPosition;
        int  currentWidth;

    public:
        goalLsOneSlice (naxos::NsIntVarArray& Vars_init, naxos::NsIntVarArray& classVars_init,
                        struct course_t course_arg_init[], int ncourse_init,
                        const naxos::NsDeque<CLectureInfo>& vi_init, const struct setup_t& setup_init, int d, int h,
                        int& cancel_init, int& progress_init,
                        int currentPosition_init, int currentWidth_init)
                : Vars(Vars_init), classVars(classVars_init), course_arg(course_arg_init), ncourse(ncourse_init),
                  vi(vi_init), setup(setup_init), days(d), hours(h),
                  cancel(cancel_init), progress(progress_init),
                  currentPosition(currentPosition_init), currentWidth(currentWidth_init)    {    }

        NsGoal  *GOAL (void);
};
#endif
#endif // LOCALSEARCH_H
