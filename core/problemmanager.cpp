/// @file
/// Data structures and algorithms for a CSP management and solving
///
/// Part of https://github.com/pothitos/naxos

#include "naxos-mini.h"

using namespace naxos;
using namespace std;

Ns_StackSearch::goal_iterator::goal_iterator(Ns_StackSearch& stackOfStacks_init)
  : stackOfStacks(&stackOfStacks_init), curr_Stack_it(stackOfStacks->begin())
{
        if (curr_Stack_it != stackOfStacks->end() &&
            (curr_node_it = curr_Stack_it->stackAND.begin()) ==
                curr_Stack_it->stackAND.end()) {
                *this = curr_Stack_it->delayedGoal;
        }
}

Ns_StackSearch::goal_iterator& Ns_StackSearch::goal_iterator::operator++(void)
{
        assert_Ns(stackOfStacks != 0,
                  "Ns_StackSearch::goal_iterator::++: Unitialized '*this'");
        assert_Ns(curr_Stack_it != stackOfStacks->end() &&
                      curr_node_it != curr_Stack_it->stackAND.end(),
                  "Ns_StackSearch::goal_iterator::end: Bad request "
                  "'++(something.end())'");
        if (++curr_node_it == curr_Stack_it->stackAND.end())
                *this = curr_Stack_it->delayedGoal;
        return *this;
}

/// Writes to a file the (splits) input of a mapper
///
/// It can be called in main with the following arguments:
/// mapperInputToFile((string("/tmp/partitions") + "." +
/// getenv("mapreduce_task_partition") + ".txt").c_str(),
/// atoi(getenv("mapreduce_task_partition")))
void Ns_StackSearch::mapperInputToFile(const char* fileName, int mapperId)
{
        fileMapperInput.open(fileName);
        assert_Ns(fileMapperInput.good(),
                  "Ns_StackSearch::mapperInputToFile: Could not open file");
        mapper = mapperId;
}

/// Writes to a file a view of the search tree in a Graphviz supported format
void Ns_StackSearch::searchToGraphFile(const char* fileName)
{
        fileSearchGraph.open(fileName);
        assert_Ns(fileSearchGraph.good(),
                  "Ns_StackSearch::searchToGraphFile: Could not open file");
        fileSearchGraph << "digraph \"Search Tree\" {\n\n"
                        << "\tnode [shape=point];\n\n"
                        << "\tedge [arrowhead=none];\n";
}

/// Creates a new search tree node
///
/// Also creates a validHistoryId entry for the current search
/// node, if it does not exist. Returns false if the node
/// shouldn't be explored, due to a specific search tree part
/// exploration.
bool Ns_StackSearch::push(const value_type& newNode)
{
        ++nSearchTreeNodes;
        if (!empty())
                ++top().children;
        if (!startNode.empty()) {
                if (startNode.front()-- > 1) {
                        return false;
                } else {
                        startNode.pop_front();
                        if (startNode.empty()) {
                                cout << "SolveStart\t" << time(0) << "\t";
                                currentPath();
                                cout << "\n";
                        }
                }
        }
        bool matchesEndNodePrevious =
            ((empty() && !endNode.empty()) ||
             (!empty() && top().matchesEndNode &&
              (size() > endNode.size() ||
               top().children >= endNode[size() - 1])));
        NsStack<Ns_SearchNode>::push(newNode);
        top().matchesEndNode = matchesEndNodePrevious;
        if (history_time.size() < size())
                history_time.push_back(history_time_t());
        history_time[size() - 1].searchTreeNodeNum = nSearchTreeNodes;
        return true;
}

/// Returns true when a specific search tree part exploration ends
bool Ns_StackSearch::splitEnded(void)
{
        if (top().matchesEndNode &&
            ((size() - 1 < endNode.size() &&
              (*(++begin())).children > endNode[size() - 2]) ||
             (size() < endNode.size() &&
              top().children > endNode[size() - 1]) ||
             (size() == endNode.size() &&
              top().children >= endNode[size() - 1]) ||
             size() > endNode.size())) {
                return true;
        } else {
                assert_Ns_3(!TEST_splitEnded(), "The split has already ended!");
                return false;
        }
}

/// Tests whether splitEnded() tells the truth
bool Ns_StackSearch::TEST_splitEnded(void) const
{
        NsUInt depth;
        bool equal, greater;
        TEST_CurrentVsEndNode(begin(), depth, equal, greater);
        return ((equal && depth == endNode.size()) || greater);
}

/// Checks if the current node ID is greater or equal than the endNode ID
///
/// Sets the boolean arguments 'greater' and 'equal' accordingly.
void Ns_StackSearch::TEST_CurrentVsEndNode(const_iterator it, NsUInt& depth,
                                           bool& equal, bool& greater) const
{
        if (it == end()) {
                depth = 0;
                equal = true;
                greater = false;
        } else {
                NsUInt children = it->children;
                TEST_CurrentVsEndNode(++it, depth, equal, greater);
                ++depth;
                equal = (equal && depth <= endNode.size() &&
                         children == endNode[depth - 1]);
                greater =
                    (greater || (equal && ((depth <= endNode.size() &&
                                            children > endNode[depth - 1]) ||
                                           depth > endNode.size())));
        }
}

/// Records the solution node to the goals graph file (if created)
void Ns_StackSearch::solutionNode(const NsIntVar* vObjective)
{
        if (fileSearchGraph.is_open()) {
                fileSearchGraph
                    << "\n\t\"(" << size() - 1 << ","
                    << history_time[size() - 1].validHistoryId << ")"
                    << ((top().children > 0) ? "LastChild" : "")
                    << "\" [shape=doublecircle, height=0.1, label=\"\"];\n";
                // If the node has children, and it is a solution, then it is
                // the last child of itself. Besides, after the success it will
                // be popped by the backtracking algorithm.
                if (vObjective != 0) {
                        recordObjective = true;
                        objectiveValue = vObjective->max();
                }
        }
}

/// Invalidates the validHistoryId for the current search node
void Ns_StackSearch::pop(void)
{
        if (!startNode.empty()) {
                cout << "SolveStart\t" << time(0) << "\t";
                currentPath();
                cout << "\n";
        }
        startNode.clear();
        if (fileSearchGraph.is_open() && size() - 1 > 0 && top().children > 0) {
                fileSearchGraph << "\n\t\"(" << size() - 1 << ","
                                << history_time[size() - 1].validHistoryId
                                << ")\" -> \"(" << size() - 1 << ","
                                << history_time[size() - 1].validHistoryId
                                << ")LastChild\"";
                if (recordObjective) {
                        recordObjective = false;
                        fileSearchGraph << " [fontsize=9, headlabel=\""
                                        << objectiveValue << "\"\"]";
                }
                fileSearchGraph << ";\n";
                // If the node has children, then it it is the last child of
                // itself. Besides, the 'goalNextChoice'--the second subgoal of
                // an OR-goal--is executed one level before the execution of
                // the first subgoal, in the search tree.
        }
        if (fileSearchGraph.is_open() && size() - 1 > 1) {
                fileSearchGraph << "\n\t\"(" << size() - 2 << ","
                                << history_time[size() - 2].validHistoryId
                                << ")\" -> \"(" << size() - 1 << ","
                                << history_time[size() - 1].validHistoryId
                                << ")\"";
                if (recordObjective) {
                        recordObjective = false;
                        fileSearchGraph << " [fontsize=9, headlabel=\""
                                        << objectiveValue << "\"\"]";
                }
                fileSearchGraph << ";\n";
        }
        double timeSimChild = top().timeSimChild;
        double descSimChild = top().descSimChild;
        history_time[size() - 1].invalidate(top().timeBorn, timeSimChild,
                                            numSearchTreeNodes(),
                                            top().descBorn, descSimChild);
        NsStack<Ns_SearchNode>::pop();
        if (!empty()) {
                top().timeSimChild += timeSimChild;
                top().descSimChild += descSimChild;
        }
}

/// Writes to a file a view of the constraint network in a Graphviz format
void NsProblemManager::constraintsToGraphFile(const char* fileName)
{
        fileConstraintsGraph.open(fileName);
        assert_Ns(
            fileConstraintsGraph.good(),
            "NsProblemManager::constraintsToGraphFile: Could not open file");
        fileConstraintsGraph
            << "digraph \"Constraint Network\" {\n\n"
            << "\tnode [shape=plaintext, fontsize=13, height=0.05];\n\n"
            << "\tedge [arrowsize=0.5, fontsize=10];\n";
        fileConstraintsGraph << "\n\n\n\t/* Variables */\n";
        for (NsDeque<const NsIntVar*>::const_iterator v = vars.begin();
             v != vars.end(); ++v) {
                fileConstraintsGraph << "\n\tVar" << *v << " [label=\"["
                                     << (*v)->min();
                if (!(*v)->isBound())
                        fileConstraintsGraph << ".." << (*v)->max();
                fileConstraintsGraph << "]\"];\n";
        }
        fileConstraintsGraph << "\n\n\n\t/* Intermediate Variables (drawn with "
                                "a smaller font) */\n";
        for (NsDeque<NsIntVar*>::const_iterator v = intermediateVars.begin();
             v != intermediateVars.end(); ++v) {
                fileConstraintsGraph << "\n\tVar" << *v << " [fontsize=9];\n";
        }
        fileConstraintsGraph << "\n\n\n\t/* Constraints */\n";
        for (Ns_constraints_array_t::iterator c = constraints.begin();
             c != constraints.end(); ++c) {
                (*c)->toGraphFile(fileConstraintsGraph);
        }
        fileConstraintsGraph << "}\n";
        fileConstraintsGraph.close();
}

#include <sstream>

const char* NsProblemManager::SPLIT_HEADER = "Split:";

/// Explores specific search tree splits described in standard input
bool Ns_StackSearch::readSplit(string& splitEnd)
{
        if (!getline(cin, mapperLine) || mapperLine.empty())
                return false;
        if (fileMapperInput.is_open() && !mapperLine.empty()) {
                fileMapperInput
                    << fixed
                    << ((clock() - mapperLineStartTime) / CLOCKS_PER_SEC)
                    << "\t" << mapper << "\t" << mapperLine << "\n";
        }
        if (fileMapperInput.is_open())
                mapperLineStartTime = clock();
        istringstream line(mapperLine);
        string lineHeader;
        assert_Ns((line >> lineHeader).good(),
                  "Ns_StackSearch::readSplit: Cannot read split line header");
        assert_Ns(lineHeader == NsProblemManager::SPLIT_HEADER,
                  "Ns_StackSearch::readSplit: Wrong split line header");
        NsUInt node;
        startNode.clear();
        while (line >> node)
                startNode.push_back(node);
        line.clear(); // Clears read failure
        getline(line, splitEnd);
        istringstream lineRest(splitEnd);
        endNode.clear();
        while (lineRest >> node)
                endNode.push_back(node);
        updateMatchesEndNode();
        return true;
}

/// Output the remaining search tree splits
///
/// Called when the time for normal search is exhausted.
void NsProblemManager::simulate(const double splitTime,
                                const double simulationRatio)
{
        cout << "SolveEnd\t" << time(0) << "\t";
        searchNodes.currentPath();
        cout << "\n";
        if (timeIsUp) {
                cout << "SimulateStart\t" << time(0) << "\t";
                searchNodes.currentPath();
                cout << "\n";
                timeLimit(0);
                splitTimeLimit(splitTime, simulationRatio);
                while (nextSolution() != false)
                        /*VOID*/;
                cout << " -" << splitEnd << "\n";
                splitTimeLimit(0, simulationRatio);
                cout << "SimulateEnd\t" << time(0) << "\t";
                searchNodes.currentPath();
                cout << "\n";
        }
        restart();
}

void Ns_StackSearch::currentPathRec(const_iterator it) const
{
        if (it != end()) {
                NsUInt children = it->children;
                currentPathRec(++it);
                if (it != end())
                        cout << " ";
                cout << children;
        }
}

bool Ns_StackSearch::updateMatchesEndNodeRec(iterator it, NsUInt& depth)
{
        if (it == end()) {
                depth = 0;
                return !endNode.empty();
        }
        NsUInt children = it->children;
        bool& matchesEndNode = it->matchesEndNode;
        matchesEndNode = updateMatchesEndNodeRec(++it, depth);
        ++depth;
        return (matchesEndNode &&
                (depth > endNode.size() || children >= endNode[depth - 1]));
}

namespace {

void destroy_goal(NsGoal* g)
{
        if (g != 0) {
                if (g->isGoalAND() || g->isGoalOR()) {
                        destroy_goal(g->getFirstSubGoal());
                        destroy_goal(g->getSecondSubGoal());
                }
                delete g;
        }
}

/// Reimplements difftime() to address MinGW issues
inline double DiffTime(time_t time2, time_t time1)
{
        return (time2 - time1);
}

} // end namespace

Ns_StackSearch::Ns_StackSearch(void)
  : nSearchTreeNodes(0), timeSimulated(0.0), recordObjective(false)
{
}

void Ns_StackSearch::clear(void)
{
        while (!empty()) {
                destroy_goal(top().goalNextChoice);
                pop();
        }
}

Ns_StackSearch::~Ns_StackSearch(void)
{
        clear();
        if (fileSearchGraph.is_open()) {
                fileSearchGraph << "}\n";
                fileSearchGraph.close();
        }
        if (fileMapperInput.is_open() && !mapperLine.empty()) {
                fileMapperInput
                    << fixed
                    << ((clock() - mapperLineStartTime) / CLOCKS_PER_SEC)
                    << "\t" << mapper << "\t" << mapperLine << "\n";
        }
}

Ns_StackGoals::~Ns_StackGoals(void)
{
        while (!empty()) {
                destroy_goal(top());
                pop();
        }
}

NsProblemManager::~NsProblemManager(void)
{
        // Delete the queue first, because it depends to the following
        getQueue().clear();
        // Constraints destruction
        for (Ns_constraints_array_t::iterator c = constraints.begin();
             c != constraints.end(); ++c) {
                delete *c;
        }
        // Intermediate variables destruction
        for (NsDeque<NsIntVar*>::iterator v = intermediateVars.begin();
             v != intermediateVars.end(); ++v) {
                delete *v;
        }
}

/// Fetches the next constraint to currentConstr that affects variable varFired
///
/// In implementing the queue, to reduce the number of queue
/// operations, one way is simply enqueue the variable whose
/// domain has shrunk, instead of enqueue all relevant arcs.
/// When we dequeue a variable from the queue, we just revise
/// all constraints pointing to this variable. The method is
/// also called variable oriented propagation. This idea
/// appeared in [McG79] and in [CJ98]. Source: "An Optimal
/// Coarse-grained Arc Consistency Algorithm," by C. Bessiere et
/// al.
///
/// Strictly speaking, in AC-3, arc (i,j) is not enqueued when
/// arc (j,i) is made consistent. This optimization could be
/// added in AC-5 by adding j as an argument to Enqueue and
/// adding the constraint k != j to its definition. Source: "A
/// Generic Arc-Consistency Algorithm and its Specializations,"
/// by P. van Hentenryck et al.
///
///    NsIntVar V1(pm, 0, 0), V2(pm, 0, 1);
///    for (int i = 0; i < 640; ++i)
///            pm.add(V1 < V2);
///    pm.nextSolution();
///
/// The following, commented-out code did not work with the
/// above code that a Solver's programmer could develop. We
/// think that the culprit is an STL bug...
///
///    Ns_Constraint* temp;
///    for (/*VOID*/; currentConstr != varFired->constraints_begin();
///         ++currentConstr) {
///            temp = *currentConstr;
///            if (temp != constrFired) {
///                    ++currentConstr;
///                    return temp;
///            }
///    }
Ns_Constraint* Ns_QueueItem::getNextConstraint(void)
{
        for (/*VOID*/; currentConstr < varFired->constraints.size();
             ++currentConstr) {
                switch (
                    varFired->constraints[currentConstr].constr->revisionType) {
                case Ns_Constraint::VALUE_CONSISTENCY:
                        do {
                                ++currentRemovedValue;
                        } while (
                            currentRemovedValue - 1 < removedValues.size() &&
                            removedValues[currentRemovedValue - 1]
                                    .constrFired ==
                                varFired->constraints[currentConstr].constr);
                        if (currentRemovedValue - 1 == removedValues.size())
                                currentRemovedValue = 0;
                        else
                                return varFired->constraints[currentConstr]
                                    .constr;
                        break;
                case Ns_Constraint::BOUNDS_CONSISTENCY:
                        if (removedBoundRec.removedBound &&
                            varFired->constraints[currentConstr].constr !=
                                removedBoundRec.constrFired) {
                                // No need to check the constraint that
                                // initiated the propagation.
                                return varFired->constraints[currentConstr++]
                                    .constr;
                        }
                        break;
                case Ns_Constraint::BIDIRECTIONAL_CONSISTENCY:
                        if (removedBoundRec.removedBound &&
                            removedBoundRec.removalTime >=
                                varFired->constraints[currentConstr]
                                    .constr->lastConstraintCheckTime) {
                                return varFired->constraints[currentConstr++]
                                    .constr;
                        }
                        break;
                default:
                        throw NsException("Ns_QueueItem::getNextConstraint: "
                                          "Invalid 'constr->revisionType'");
                        break;
                };
        }
        return 0;
}

/// Adds a constraint to the problem
void NsProblemManager::add(const Ns_ExprConstr& expr)
{
        assert_Ns(firstNextSolution, "NsProblemManager::add: Cannot add a "
                                     "constraint because search has already "
                                     "started");
        Ns_Constraint* newConstr = expr.postConstraint();
        if (newConstr == 0)
                return; // unary constraint
        newConstr->ArcCons();
        recordConstraint(newConstr);
}

/// Adds a soft constraint to the problem, with the corresponding weight
void NsProblemManager::add(const Ns_ExprConstr& expr, const NsInt weight)
{
        assert_Ns(firstNextSolution, "NsProblemManager::add: Cannot add a "
                                     "constraint because search has already "
                                     "started");
        assert_Ns(vObjective == 0, "NsProblemManager::add: "
                                   "'NsProblemManager::minimize()' should not "
                                   "be used together with soft constraints");
        vSoftConstraintsTerms.push_back(weight * expr.post());
}

/// Imposes arc consistency
bool NsProblemManager::arcConsistent(void)
{
        if (foundInconsistency) {
                foundInconsistency = false;
                getQueue().clear();
                ++nFailures;
                return false;
        }
        Ns_Constraint* c;
        NsIntVar* vFired;
        while (!getQueue().empty()) {
                vFired = getQueue().front().getVarFired();
                // To avoid changing the queue item
                // Q.front() during this iteration...
                vFired->queueItem = 0;
                while ((c = getQueue().front().getNextConstraint()) != 0) {
                        // Change the following for AC-3.
                        // c->ArcCons();
                        c->LocalArcCons(getQueue().front());
                        c->lastConstraintCheckTime = ++nConstraintChecks;
                        if (foundInconsistency) {
                                foundInconsistency = false;
                                getQueue().clear();
                                ++nFailures;
                                return false;
                        }
                }
                getQueue().pop();
        }
        return true;
}

/// Backtracks the search process to the previous choice point
bool NsProblemManager::backtrack(void)
{
        NsGoal* goalNextChoice;
        for (;;) {
                if (backtrackLim != 0 && nBacktracks >= backtrackLim)
                        return false;
                ++nBacktracks;
                assert_Ns(
                    !searchNodes.empty(),
                    "NsProblemManager::backtrack: 'searchNodes' is empty");
                goalNextChoice = searchNodes.top().goalNextChoice;
                if (goalNextChoice == 0)
                        return false;
                searchNodes.top().bitsetsStore.restore();
                searchNodes.pop();
                searchNodes.top().stackAND.push(goalNextChoice);
                if (vObjective != 0) {
                        vObjective->remove(bestObjective, NsPLUS_INF);
                        if (foundInconsistency) {
                                foundInconsistency = false;
                                getQueue().clear();
                                continue;
                        }
                }
                return true;
        }
}

/// Reverts the domains of the constrained variables
///
/// Except for the 'objective' variable, it restores every
/// variable in the state it was after the first arcConsistent()
/// call.
void NsProblemManager::restart(void)
{
        firstNextSolution = true;
        // For any case, we clear the propagation engine's members.
        foundInconsistency = false;
        getQueue().clear();
        bool foundSecondFrame = false;
        NsGoal* goalNextChoice;
        assert_Ns(!searchNodes.empty(),
                  "NsProblemManager::restart: 'searchNodes' is empty");
        do {
                goalNextChoice = searchNodes.top().goalNextChoice;
                if (goalNextChoice == 0)
                        foundSecondFrame = true;
                searchNodes.top().bitsetsStore.restore();
                searchNodes.pop();
                searchNodes.top().stackAND.push(goalNextChoice);
                // We kept the above line because of Memory Management
                // reasons (in order to delete the 'goalNextChoice').
                assert_Ns(!searchNodes.empty(),
                          "'restart()' call, before 'nextSolution()'");
        } while (!foundSecondFrame);
        searchNodes.pop();
        searchNodes.reset();
        assert_Ns(searchNodes.push(Ns_SearchNode(0, searchNodes.gbegin(),
                                                 numSearchTreeNodes())),
                  "NsProblemManager::restart: First push should succeed");
        if (vObjective != 0)
                vObjective->remove(bestObjective, NsPLUS_INF);
}

/// Finds next solution of the problem; returns false when no solution found
bool NsProblemManager::nextSolution(void)
{
        timeIsUp = false;
        bool isArcCons = true;
        if (firstNextSolution) {
                firstNextSolution = false;
                // Soft constraints objective
                if (vObjective == 0 && !vSoftConstraintsTerms.empty())
                        minimize(-NsSum(vSoftConstraintsTerms));
                isArcCons = arcConsistent();
                // Throwing away unnesessary 'bitsetsStore' in the first frame
                searchNodes.top().bitsetsStore.clear();
                // (A) Cutting from the stackAND of the base frame...
                Ns_StackGoals tempStackAND;
                while (!searchNodes.top().stackAND.empty()) {
                        tempStackAND.push(searchNodes.top().stackAND.top());
                        searchNodes.top().stackAND.pop();
                }
                // A push of frame, for the purposes of
                // NsProblemManager::restart(). We took care placing it _after_
                // the arcConsistent() call because in future, we will not be
                // able to revert to the current 'Q'.
                assert_Ns(searchNodes.push(Ns_SearchNode(
                              0, searchNodes.gbegin(), numSearchTreeNodes())),
                          "NsProblemManager::nextSolution: First push should "
                          "succeed");
                // (B) ...and pasting to the stackAND of the new frame.
                while (!tempStackAND.empty()) {
                        searchNodes.top().stackAND.push(tempStackAND.top());
                        tempStackAND.pop();
                }
                if (searchNodes.top().stackAND.empty() &&
                    searchNodes.top().delayedGoal == searchNodes.gend())
                        return isArcCons;
        }
        if (calledTimeLimit && timeLim != 0) {
                calledTimeLimit = false;
                if (isRealTime) {
                        startRealTime = time(0);
                        assert_Ns(startRealTime != -1,
                                  "Could not find time for 'realTimeLimit'");
                } else {
                        startTime = clock();
                        assert_Ns(startTime != -1,
                                  "Could not find time for 'timeLimit'");
                }
        }
        if ((!isArcCons || !arcConsistent()) ||
            (searchNodes.top().stackAND.empty() &&
             searchNodes.top().delayedGoal == searchNodes.gend())) {
                if (!backtrack())
                        return false;
        }
        NsGoal *CurrGoal, *NewGoal;
        bool popped_a_goal;
        while (timeLim == 0 ||
               (isRealTime && DiffTime(time(0), startRealTime) <= timeLim) ||
               (!isRealTime &&
                static_cast<unsigned long>(clock() - startTime) <=
                    timeLim * static_cast<unsigned long>(CLOCKS_PER_SEC))) {
                if (timeSplitLim != 0 && getCurrentNodeNum() > startNodeId &&
                    clock() - startSplitTime + searchNodes.timeSimulated >=
                        timeSplitLim) {
                        startNodeId = getCurrentNodeNum();
                        startSplitTime = clock();
                        searchNodes.timeSimulated = 0;
                        cout << " - ";
                        searchNodes.currentPath();
                        cout << "\n";
                        splitHeader();
                        searchNodes.currentPath();
                }
                popped_a_goal = false;
                if (!searchNodes.top().stackAND.empty()) {
                        CurrGoal = searchNodes.top().stackAND.top();
                        searchNodes.top().stackAND.pop();
                        popped_a_goal = true;
                } else {
                        assert_Ns(searchNodes.top().delayedGoal !=
                                      searchNodes.gend(),
                                  "NsProblemManager::nextSolution: No goal to "
                                  "execute");
                        CurrGoal = *searchNodes.top().delayedGoal;
                        ++searchNodes.top().delayedGoal;
                }
                assert_Ns(
                    CurrGoal != 0,
                    "NsProblemManager::nextSolution: Zero goal to execute");
                if (CurrGoal->isGoalAND()) {
                        searchNodes.top().stackAND.push(
                            CurrGoal->getSecondSubGoal());
                        searchNodes.top().stackAND.push(
                            CurrGoal->getFirstSubGoal());
                        if (popped_a_goal)
                                delete CurrGoal;
                } else if (CurrGoal->isGoalOR()) {
                        if (timeSplitLim != 0 &&
                            searchNodes.overrideNextLevel()) {
                                double timeSim = searchNodes.nextMeanTime();
                                double descSim = searchNodes.nextMeanDesc();
                                searchNodes.timeSimulated += timeSim;
                                searchNodes.top().timeSimChild += timeSim;
                                searchNodes.top().descSimChild += descSim;
                                destroy_goal(CurrGoal->getFirstSubGoal());
                                searchNodes.top().stackAND.push(
                                    CurrGoal->getSecondSubGoal());
                        } else if (searchNodes.push(Ns_SearchNode(
                                       CurrGoal->getSecondSubGoal(),
                                       searchNodes.gbegin(),
                                       numSearchTreeNodes()))) {
                                searchNodes.top().stackAND.push(
                                    CurrGoal->getFirstSubGoal());
                        } else {
                                destroy_goal(CurrGoal->getFirstSubGoal());
                                searchNodes.top().stackAND.push(
                                    CurrGoal->getSecondSubGoal());
                        }
                        if (popped_a_goal)
                                delete CurrGoal;
                        if (searchNodes.splitEnded())
                                return false;
                } else {
                        ++nGoals;
                        NewGoal = CurrGoal->GOAL();
                        if (popped_a_goal)
                                delete CurrGoal;
                        if (!arcConsistent()) {
                                destroy_goal(NewGoal);
                                if (!backtrack())
                                        return false;
                        } else if (NewGoal != 0) {
                                searchNodes.top().stackAND.push(NewGoal);
                        } else if (searchNodes.top().stackAND.empty() &&
                                   searchNodes.top().delayedGoal ==
                                       searchNodes.gend()) {
                                if (vObjective != 0) {
                                        assert_Ns(
                                            bestObjective > vObjective->max(),
                                            "NsProblemManager::nextSolution: "
                                            "Wrong objective value");
                                        bestObjective = vObjective->max();
                                        // We have taken care about the rare
                                        // and odd case where the domain of
                                        // vObjective has been augmented.
                                }
                                searchNodes.solutionNode(vObjective);
                                return true;
                        }
                }
        }
        timeIsUp = true;
        return false;
}
