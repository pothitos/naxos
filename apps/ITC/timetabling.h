// Part of https://github.com/pothitos/naxos

#ifndef ITC_TIMETABLING_H
#define ITC_TIMETABLING_H

#include <amorgos.h>
#include <naxos.h>

const bool saveSearchTree = false;
const bool saveLog = true;

struct itcRoom_t;
struct itcCourse_t;
struct itcGroup_t;

enum itcSearchMethod_t {
        NONE,
        DFS,
        LDS,
        CREDIT,
        DBS,
        LAN,
        DBDS,
        IBROAD,
        BBS,
        RDFS,
        GNS,
        FNS,
        ISAMP,
        ONESAMP,
        DOMAIN_SPLITTING,
        PoPS
};
enum itcHeurType_t {
        NORMAL = 1,
        STATIC,
        RANDOM,
        FAIR /* for domain-splitting */
};
enum itcFnsFunction_t { FSAMPLE = 1, FSAMPLE_MIDDLE, FSAMPLE_EXP };

struct itcProblem_t {

        char* name;
        int days, hours;
        int nrooms;
        struct itcRoom_t* room;
        int nprofessors;
        char** professorName;
        int ncourses;
        struct itcCourse_t* course;
        int ngroups;
        struct itcGroup_t* group;

        int nConstraints;
        int nranks; // used for information purposes
        int solutions;

        int timeLimit;
        // bool               isCpuTime;

        itcSearchMethod_t searchMethod;
        int searchMethodParameter[4];
        itcHeurType_t varHeuristicType;
        itcHeurType_t valHeuristicType;
        // bool               valHeuristicTransform;
        // bool               search_roomBacktrack;

        bool isActiveLocalSearch;

        itcSearchMethod_t searchMethod_ls;
        int searchMethodParameter_ls[4];
        itcHeurType_t varHeuristicType_ls;
        itcHeurType_t valHeuristicType_ls;
        bool valHeuristicTransform_ls;

        int timeLimitDirectMethodRound;
        int timeLimitIndirectMethodRound;

        double constrainingFactorExponent;
        double conf;
        double leastConstrainingValueHeuristicWeight;
        double objectiveOrientedValueHeuristicWeight;

        double max2ndVarHeurCriterion;
        double max3rdVarHeurCriterion;

        bool quiet;
};

struct itcRoom_t {

        char* name;
        int capacity;

        int rank; // used for information purposes
};

struct itcCourse_t {

        char* name;
        int professor;
        int lectures;
        int minWorkingDays;
        int students;
        int nunavail;
        int* unavail;
        int lecturesScheduled;
        int* periodScheduled;
        int* roomScheduled;
};

struct itcLectureInfo_t {

        int courseIndex;
        double staticHeuristic;
};

struct itcCourseInfo_t {

        // int  duration;
        int firstLectIndex;
        int lastLectIndex;
        // int  *classprefer;
        // int  nclassprefer;
        naxos::NsDeque<int> group;
        naxos::NsDeque<int> isConnectedToCourse;
};

struct itcGroup_t {

        char* name;
        int ncourses;
        int* course;
};

const char* const colorCode[] = {
    "FF8080", "FFFF80", "80FF80", "00FF80", "80FFFF", "0080FF", "FF80C0",
    "FF80FF", "FF0000", "FFFF00", "80FF00", "00FF40", "00FFFF", "0080C0",
    "8080C0", "FF00FF", "804040", "FF8040", "00FF00", "008080", "004080",
    "8080FF", "800040", "FF0080", "800000", "FF8000", "008000", "008040",
    "0000FF", "0000A0", "800080", "8000FF", "400000", "804000", "004000",
    "004040", "000080", "000040", "400040", "400080", "808000", "808040",
    "808080", "408080", "C0C0C0", "400040", "FFFFFF"};

void createInstance(struct itcProblem_t& pr, const int argc, char* argv[]);

void destroyInstance(const struct itcProblem_t& pr);

void writeSolution(const struct itcProblem_t& pr,
                   naxos::NsIntVarArray& vLectPeriod,
                   naxos::NsIntVarArray& vLectRoom,
                   const char* solutionFileName);

naxos::NsGoal*
metaSearchMethodGoal(struct itcProblem_t& pr, naxos::NsIntVarArray& vLectPeriod,
                     naxos::NsIntVarArray& vLectPeriodSortedByHeur,
                     naxos::VariableHeuristic& varHeur,
                     naxos::ValueHeuristic& valHeur,
                     naxos::NsDeque<unsigned>& lanAssigns);
#endif // ITC_TIMETABLING_H
