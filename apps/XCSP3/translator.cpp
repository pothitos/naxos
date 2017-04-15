/// @file
/// Implementation of the interface between XCSP3 and Naxos
///
/// Part of https://github.com/pothitos/naxos

#include "translator.h"

using namespace XCSP3Core;
using namespace std;

template <class T>
void displayList(vector<T>& list, string separator = " ")
{
        typename vector<T>::size_type i;
        if (list.size() > 8) {
                for (i = 0; i < 3; i++)
                        cout << list[i] << separator;
                cout << " ... ";
                for (i = list.size() - 4; i < list.size(); i++)
                        cout << list[i] << separator;
                cout << "\n";
                return;
        }
        for (i = 0; i < list.size(); i++)
                cout << list[i] << separator;
        cout << "\n";
}

void displayList(vector<XVariable*>& list, string separator = " ")
{
        vector<XVariable*>::size_type i;
        if (list.size() > 8) {
                for (i = 0; i < 3; i++)
                        cout << list[i]->id << separator;
                cout << " ... ";
                for (i = list.size() - 4; i < list.size(); i++)
                        cout << list[i]->id << separator;
                cout << "\n";
                return;
        }
        for (i = 0; i < list.size(); i++)
                cout << list[i]->id << separator;
        cout << "\n";
}

void Xcsp3_to_Naxos::beginInstance(InstanceType type)
{
        if (verbose)
                cout << "start instance of type " << type << "\n";
        if (instanceAlreadyBegan)
                throw invalid_argument("Only one CSP definition can be read");
}

void Xcsp3_to_Naxos::endInstance()
{
        if (verbose)
                cout << "end SAX parsing\n";
}

void Xcsp3_to_Naxos::beginVariables()
{
        if (verbose)
                cout << "  start variables declaration\n";
}

void Xcsp3_to_Naxos::endVariables()
{
        if (verbose)
                cout << "  end variables declaration" << "\n";
}

void Xcsp3_to_Naxos::beginVariableArray(string id)
{
        if (verbose)
                cout << "    array " << id << "\n";
}

void Xcsp3_to_Naxos::endVariableArray()
{
}

void Xcsp3_to_Naxos::beginConstraints()
{
        if (verbose)
                cout << "  start constraints declaration\n";
}

void Xcsp3_to_Naxos::endConstraints()
{
        if (verbose)
                cout << "  end constraints declaration\n";
}

void Xcsp3_to_Naxos::beginGroup(string id)
{
        if (verbose)
                cout << "    start group of constraint " << id << "\n";
}

void Xcsp3_to_Naxos::endGroup()
{
        if (verbose)
                cout << "    end group of constraint\n";
}

void Xcsp3_to_Naxos::beginBlock(string classes)
{
        if (verbose)
                cout << "    start block of constraint classes = " << classes
                     << "\n";
}

void Xcsp3_to_Naxos::endBlock()
{
        if (verbose)
                cout << "    end group of constraint\n";
}

void Xcsp3_to_Naxos::beginObjectives()
{
        if (verbose)
                cout << "  start objective\n";
}

void Xcsp3_to_Naxos::endObjectives()
{
        if (verbose)
                cout << "  end objective\n";
}

void Xcsp3_to_Naxos::buildVariableInteger(string id, int minValue, int maxValue)
{
        if (verbose) {
                cout << "    var " << id << ": " << minValue << "..."
                     << maxValue << "\n";
        }
}

void Xcsp3_to_Naxos::buildVariableInteger(string id, vector<int>& values)
{
        if (verbose) {
                cout << "    var " << id << ": ";
                displayList(values);
        }
}

/// Intension constraint
void Xcsp3_to_Naxos::buildConstraintIntension(string id, string expr)
{
        if (verbose) {
                cout << "\n    intension constraint : " << id << " : " << expr
                     << "\n";
        }
}

/// Primitive constraint x +- k op y
void Xcsp3_to_Naxos::buildConstraintPrimitive(string id, OrderType op,
                                              XVariable* x, int k, XVariable* y)
{
        if (verbose) {
                cout << "\n   intension constraint " << id << ": " << x->id
                     << (k >= 0 ? "+" : "") << k << " op " << y->id << "\n";
        }
}

/// Extension constraint
void Xcsp3_to_Naxos::buildConstraintExtension(string id,
                                              vector<XVariable*> list,
                                              vector<vector<int>>& tuples,
                                              bool support, bool hasStar)
{
        if (verbose) {
                cout << "\n    extension constraint : " << id << "\n";
                cout << "        " << (support ? "support" : "conflict")
                     << " arity:" << list.size()
                     << " nb tuples: " << tuples.size() << " star: " << hasStar
                     << "\n";
                cout << "        ";
                displayList(list);
        }
        if (hasStar)
                throw invalid_argument("Short tables, i.e. tables with tuples "
                                       "containing '*', are not accepted");
}

/// Unary extension constraint
void Xcsp3_to_Naxos::buildConstraintExtension(string id, XVariable* variable,
                                              vector<int>& tuples, bool support,
                                              bool hasStar)
{
        if (verbose) {
                cout << "\n    extension constraint with one variable: " << id
                     << "\n";
                cout << "        " << (support ? "support" : "conflict")
                     << " nb tuples: " << tuples.size() << " star: " << hasStar
                     << "\n";
                cout << (*variable) << "\n";
        }
        if (hasStar)
                throw invalid_argument("Short tables, i.e. tables with tuples "
                                       "containing '*', are not accepted");
}

/// Extension constraint with exactly the same tuples as the previous one
void Xcsp3_to_Naxos::buildConstraintExtensionAs(string id,
                                                vector<XVariable*> list,
                                                bool support, bool hasStar)
{
        if (verbose) {
                cout << "\n    extension constraint similar as previous one: "
                     << id << "\n";
        }
        if (hasStar)
                throw invalid_argument("Short tables, i.e. tables with tuples "
                                       "containing '*', are not accepted");
}

void Xcsp3_to_Naxos::buildConstraintAlldifferent(string id,
                                                 vector<XVariable*>& list)
{
        if (verbose) {
                cout << "\n    allDiff constraint" << id << "\n";
                cout << "        ";
                displayList(list);
        }
}

/// Unweighted sum constraint
void Xcsp3_to_Naxos::buildConstraintSum(string id, vector<XVariable*>& list,
                                        XCondition& cond)
{
        if (verbose) {
                cout << "\n        unweighted sum constraint " << id << ":";
                cout << "        ";
                displayList(list, "+");
                cout << cond << "\n";
        }
}

/// Weighted sum constraint
void Xcsp3_to_Naxos::buildConstraintSum(string id, vector<XVariable*>& list,
                                        vector<int>& coeffs, XCondition& cond)
{
        if (verbose) {
                cout << "\n        sum constraint " << id << ":";
                if (list.size() > 8) {
                        for (int i = 0; i < 3; i++)
                                cout << (coeffs.size() == 0 ? 1 : coeffs[i])
                                     << "*" << *(list[i]) << " ";
                        cout << " ... ";
                        for (unsigned int i = list.size() - 4; i < list.size();
                             i++)
                                cout << (coeffs.size() == 0 ? 1 : coeffs[i])
                                     << "*" << *(list[i]) << " ";
                } else {
                        for (unsigned int i = 0; i < list.size(); i++)
                                cout << (coeffs.size() == 0 ? 1 : coeffs[i])
                                     << "*" << *(list[i]) << " ";
                }
                cout << cond << "\n";
        }
}

/// Sum constraint with variables as weights
void Xcsp3_to_Naxos::buildConstraintSum(string id, vector<XVariable*>& list,
                                        vector<XVariable*>& coeffs,
                                        XCondition& cond)
{
        if (verbose) {
                cout << "\n        scalar sum constraint " << id << ":";
                if (list.size() > 8) {
                        for (int i = 0; i < 3; i++)
                                cout << coeffs[i]->id << "*" << *(list[i])
                                     << " ";
                        cout << " ... ";
                        for (unsigned int i = list.size() - 4; i < list.size();
                             i++)
                                cout << coeffs[i]->id << "*" << *(list[i])
                                     << " ";
                } else {
                        for (unsigned int i = 0; i < list.size(); i++)
                                cout << coeffs[i]->id << "*" << *(list[i])
                                     << " ";
                }
                cout << cond << "\n";
        }
}

/// Value is in position index inside the list
void Xcsp3_to_Naxos::buildConstraintElement(string id, vector<XVariable*>& list,
                                            int startIndex, XVariable* index,
                                            RankType rank, int value)
{
        if (verbose) {
                cout << "\n    element constant (with index) constraint " << id
                     << "\n";
                cout << "        ";
                displayList(list);
                cout << "        value: " << value << "\n";
                cout << "        Start index : " << startIndex << "\n";
                cout << "        index : " << *index << "\n";
        }
        if (startIndex != 0)
                throw invalid_argument("startIndex argument of element "
                                       "constraint should be 0");
        if (rank != ANY)
                throw invalid_argument("rank argument of element "
                                       "constraint should be any");
}

/// Variable is in position index inside the list
void Xcsp3_to_Naxos::buildConstraintElement(string id, vector<XVariable*>& list,
                                            int startIndex, XVariable* index,
                                            RankType rank, XVariable* value)
{
        if (verbose) {
                cout << "\n    element variable (with index) constraint" << id
                     << "\n";
                cout << "        ";
                displayList(list);
                cout << "        value: " << *value << "\n";
                cout << "        Start index : " << startIndex << "\n";
                cout << "        index : " << *index << "\n";
        }
        if (startIndex != 0)
                throw invalid_argument("startIndex argument of element "
                                       "constraint should be 0");
        if (rank != ANY)
                throw invalid_argument("rank argument of element "
                                       "constraint should be any");
}

void Xcsp3_to_Naxos::buildObjectiveMinimizeVariable(XVariable* x)
{
        if (verbose)
                cout << "    minimize var " << x << "\n";
}

void Xcsp3_to_Naxos::buildObjectiveMaximizeVariable(XVariable* x)
{
        if (verbose)
                cout << "    maximize var " << x << "\n";
}

void Xcsp3_to_Naxos::buildObjectiveMinimize(ExpressionObjective type,
                                            vector<XVariable*>& list)
{
        XCSP3CoreCallbacks::buildObjectiveMinimize(type, list);
}

void Xcsp3_to_Naxos::buildObjectiveMaximize(ExpressionObjective type,
                                            vector<XVariable*>& list)
{
        XCSP3CoreCallbacks::buildObjectiveMaximize(type, list);
}

void Xcsp3_to_Naxos::buildObjectiveMinimize(ExpressionObjective type,
                                            vector<XVariable*>& list,
                                            vector<int>& coefs)
{
        XCSP3CoreCallbacks::buildObjectiveMinimize(type, list, coefs);
}

void Xcsp3_to_Naxos::buildObjectiveMaximize(ExpressionObjective type,
                                            vector<XVariable*>& list,
                                            vector<int>& coefs)
{
        XCSP3CoreCallbacks::buildObjectiveMaximize(type, list, coefs);
}
