/// @file
/// Implementation of the interface between XCSP3 and Naxos
///
/// Part of https://github.com/pothitos/naxos

#include "translator.h"
#include <algorithm>

using namespace XCSP3Core;
using namespace naxos;
using namespace std;

template <typename T>
void displayList(vector<T>& list, string separator = " ")
{
        typename vector<T>::size_type i;
        if (list.size() > 8) {
                for (i = 0; i < 3; i++)
                        cout << list[i] << separator;
                cout << "..." << separator;
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
                cout << "..." << separator;
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
        if (AllVars.empty())
                throw invalid_argument("No constrained variables defined");
        pm.addGoal(new NsgLabeling(AllVars));
}

void Xcsp3_to_Naxos::beginVariables()
{
        if (verbose)
                cout << "  start variables\n";
}

void Xcsp3_to_Naxos::endVariables()
{
        if (verbose)
                cout << "  end variables\n";
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
                cout << "  start constraints\n";
}

void Xcsp3_to_Naxos::endConstraints()
{
        if (verbose)
                cout << "  end constraints\n";
}

void Xcsp3_to_Naxos::beginGroup(string id)
{
        if (verbose)
                cout << "    start group " << id << "\n";
}

void Xcsp3_to_Naxos::endGroup()
{
        if (verbose)
                cout << "    end group\n";
}

void Xcsp3_to_Naxos::beginBlock(string classes)
{
        if (verbose)
                cout << "    start block of class " << classes << "\n";
}

void Xcsp3_to_Naxos::endBlock()
{
        if (verbose)
                cout << "    end block\n";
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
        variable[id] = NsIntVar(pm, minValue, maxValue);
        recordVar(id, variable[id]);
}

void Xcsp3_to_Naxos::buildVariableInteger(string id, vector<int>& values)
{
        if (verbose) {
                cout << "    var " << id << ": ";
                displayList(values);
        }
        if (values.empty()) {
                throw domain_error("The domain of a constrained variable "
                                   "cannot be empty");
        }
        // Ensure that the values are ordered
        sort(values.begin(), values.end());
        // Set variable's domain to be the whole values[0]..values[N-1]
        variable[id] = NsIntVar(pm, values.front(), values.back());
        // Remove gaps from the variable's domain
        for (vector<int>::size_type i = 0; i < values.size() - 1; ++i)
                for (int val = values[i] + 1; val < values[i + 1]; ++val)
                        variable[id].remove(val);
        recordVar(id, variable[id]);
}

namespace {

// Returns true upon success of conversion of str to num
bool strToLong(const string& str, long& num)
{
        try {
                size_t endPosition;
                num = stol(str, &endPosition);
                if (endPosition != str.size())
                        return false; // There're stray characters after number
                return true;
        } catch (...) {
                return false;
        }
}

/// Converts a comparison string to the corresponding enum constant
OrderType parseComparison(const string& comparison)
{
        if (comparison == "eq")
                return EQ;
        else if (comparison == "ne")
                return NE;
        else if (comparison == "lt")
                return LT;
        else if (comparison == "le")
                return LE;
        else if (comparison == "gt")
                return GT;
        else if (comparison == "ge")
                return GE;
        else
                throw invalid_argument("Invalid intension constraint");
}

/// Tokenizes the expression inside a comparison, i.e. "add(X,1),Y"
///
/// The above expression is tokenized into operation="add",
/// operand1="X", operand2="1", tokenRight="Y". The string
/// tokenLeft is empty; this means that it is represented by the
/// "operation(operand1,operand2)" strings.
///
/// Similarly, the expression "X,sub(Y-1)" is broken up into
/// tokenLeft="X", operation="sub", operand1="Y", operand2="1".
/// The string tokenRight is empty, because it is substituted by
/// the three oper* strings.
void parseExpression(const string& expr, string& tokenLeft, string& tokenRight,
                     string& operation, string& operand1, string& operand2)
{
        string currentToken;
        bool insideParentheses = false;
        for (auto c : expr) {
                if (c == '(') {
                        if (insideParentheses) {
                                throw invalid_argument("Only one arithmetic "
                                                       "operation is "
                                                       "permitted");
                        }
                        insideParentheses = true;
                        operation = currentToken;
                        currentToken = "";
                } else if (c == ')') {
                        if (!insideParentheses)
                                throw invalid_argument("Unmatched parenthesis");
                        insideParentheses = false;
                        operand2 = currentToken;
                        currentToken = "";
                } else if (c == ',') {
                        if (insideParentheses) {
                                operand1 = currentToken;
                                currentToken = "";
                        } else {
                                tokenLeft = currentToken;
                                currentToken = "";
                        }
                } else {
                        currentToken += c;
                }
        }
        tokenRight = currentToken;
}

} // end namespace

/// Convert the left-hand constraint part into its corresponding type
void Xcsp3_to_Naxos::unfoldLeftToken(OrderType comparison, string& tokenLeft,
                                     string& tokenRight, string& operation,
                                     string& operand1, string& operand2)
{
        // TODO
}

/// Intension constraint
void Xcsp3_to_Naxos::buildConstraintIntension(string id, string expr)
{
        if (verbose)
                cout << "    intension " << id << ": " << expr << "\n";
        if (expr.size() < 4 || expr[2] != '(' || expr[expr.size() - 1] != ')')
                throw invalid_argument("Invalid intension constraint");
        string comparison = expr.substr(0, 2);
        // Store comparison into 'comp' variable
        OrderType comp = parseComparison(comparison);
        // Get rid of the "xx()" comparison
        expr = expr.substr(3, expr.size() - 4);
        // Break internal expression into tokens
        string tokenLeft, tokenRight;
        string operation, operand1, operand2;
        parseExpression(expr, tokenLeft, tokenRight, operation, operand1,
                        operand2);
        // Set the constraint
        unfoldLeftToken(comp, tokenLeft, tokenRight, operation, operand1,
                        operand2);
}

/// Primitive constraint x +- k op y
void Xcsp3_to_Naxos::buildConstraintPrimitive(string id, OrderType op,
                                              XVariable* x, int k, XVariable* y)
{
        if (verbose) {
                cout << "    intension " << id << ": " << x->id
                     << (k >= 0 ? "+" : "") << k << " op " << y->id << "\n";
        }
        NsIntVar& VarX = variable[x->id];
        NsIntVar& VarY = variable[y->id];
        switch (op) {
        case EQ:
                pm.add(VarX + k == VarY);
                break;
        case NE:
                pm.add(VarX + k != VarY);
                break;
        case LT:
                pm.add(VarX + k < VarY);
                break;
        case LE:
                pm.add(VarX + k <= VarY);
                break;
        case GT:
                pm.add(VarX + k > VarY);
                break;
        case GE:
                pm.add(VarX + k >= VarY);
                break;
        default:
                throw invalid_argument("Unsupported operator for intension "
                                       "constraint");
                break;
        }
}

/// Unary extension constraint
void Xcsp3_to_Naxos::buildConstraintExtension(string id, XVariable* var,
                                              vector<int>& tuples, bool support,
                                              bool hasStar)
{
        if (verbose) {
                cout << "    extension " << (support ? "support" : "conflict")
                     << " with one variable " << id << "\n"
                     << "      "
                     << "tuples: " << tuples.size() << ", "
                     << "star: " << hasStar << "\n"
                     << "      " << *var << "\n";
        }
        if (hasStar) {
                throw invalid_argument("Short tables, i.e. tables with tuples "
                                       "containing '*', are not accepted");
        }
        // TODO
}

/// Extension constraint
void Xcsp3_to_Naxos::buildConstraintExtension(string id,
                                              vector<XVariable*> list,
                                              vector<vector<int>>& tuples,
                                              bool support, bool hasStar)
{
        if (verbose) {
                cout << "    extension " << (support ? "support" : "conflict")
                     << " " << id << "\n"
                     << "      "
                     << "arity: " << list.size() << ", "
                     << "tuples: " << tuples.size() << ", "
                     << "star: " << hasStar << "\n"
                     << "      ";
                displayList(list);
        }
        if (hasStar) {
                throw invalid_argument("Short tables, i.e. tables with tuples "
                                       "containing '*', are not accepted");
        }
        // TODO
}

/// Extension constraint with exactly the same tuples as the previous one
void Xcsp3_to_Naxos::buildConstraintExtensionAs(string id,
                                                vector<XVariable*> list,
                                                bool support, bool hasStar)
{
        if (verbose)
                cout << "    extension same as above " << id << "\n";
        if (hasStar) {
                throw invalid_argument("Short tables, i.e. tables with tuples "
                                       "containing '*', are not accepted");
        }
        // TODO
}

void Xcsp3_to_Naxos::buildConstraintAlldifferent(string id,
                                                 vector<XVariable*>& list)
{
        if (verbose) {
                cout << "    allDifferent " << id << ": ";
                displayList(list);
        }
        collectArray(list);
        pm.add(NsAllDiff(arrays.back()));
}

/// Enforces sum constraint for every possible condition
template <typename T>
void Xcsp3_to_Naxos::unfoldSumConstraintCondition(NsIntVarArray& array,
                                                  const OrderType condition,
                                                  T& operand)
{
        switch (condition) {
        case EQ:
                pm.add(NsSum(array) == operand);
                break;
        case NE:
                pm.add(NsSum(array) != operand);
                break;
        case LT:
                pm.add(NsSum(array) < operand);
                break;
        case LE:
                pm.add(NsSum(array) <= operand);
                break;
        case GT:
                pm.add(NsSum(array) > operand);
                break;
        case GE:
                pm.add(NsSum(array) >= operand);
                break;
        default:
                throw invalid_argument("Unsupported sum condition");
                break;
        }
}

/// Enforces sum constraint for every possible operand
void Xcsp3_to_Naxos::unfoldSumConstraintOperand(NsIntVarArray& array,
                                                const XCondition& cond)
{
        switch (cond.operandType) {
        case INTEGER:
                unfoldSumConstraintCondition(array, cond.op, cond.val);
                break;
        case VARIABLE:
                unfoldSumConstraintCondition(array, cond.op,
                                             variable[cond.var]);
                break;
        default:
                throw invalid_argument("Unsupported operand for sum");
                break;
        }
}

/// Unweighted sum constraint
void Xcsp3_to_Naxos::buildConstraintSum(string id, vector<XVariable*>& list,
                                        XCondition& cond)
{
        if (verbose) {
                cout << "    sum " << id << ": ";
                displayList(list, "+");
                cout << cond << "\n";
        }
        collectArray(list);
        unfoldSumConstraintOperand(arrays.back(), cond);
}

/// Weighted sum constraint
void Xcsp3_to_Naxos::buildConstraintSum(string id, vector<XVariable*>& list,
                                        vector<int>& coeffs, XCondition& cond)
{
        if (verbose) {
                cout << "    sum " << id << ": ";
                if (list.size() > 8) {
                        for (int i = 0; i < 3; i++)
                                cout << (coeffs.size() == 0 ? 1 : coeffs[i])
                                     << "*" << *list[i] << " ";
                        cout << "... ";
                        for (unsigned int i = list.size() - 4; i < list.size();
                             i++)
                                cout << (coeffs.size() == 0 ? 1 : coeffs[i])
                                     << "*" << *list[i] << " ";
                } else {
                        for (unsigned int i = 0; i < list.size(); i++)
                                cout << (coeffs.size() == 0 ? 1 : coeffs[i])
                                     << "*" << *list[i] << " ";
                }
                cout << cond << "\n";
        }
        // Push a new array in the end of the arrays' collection
        arrays.push_back(NsIntVarArray());
        // Create a reference to the new array
        NsIntVarArray& array = arrays.back();
        // Add the new array's items
        for (vector<XVariable*>::size_type i = 0; i < list.size(); ++i)
                array.push_back(coeffs.at(i) * variable[list[i]->id]);
        unfoldSumConstraintOperand(array, cond);
}

/// Sum constraint with variables as weights
void Xcsp3_to_Naxos::buildConstraintSum(string id, vector<XVariable*>& list,
                                        vector<XVariable*>& coeffs,
                                        XCondition& cond)
{
        if (verbose) {
                cout << "    scalar sum " << id << ": ";
                if (list.size() > 8) {
                        for (int i = 0; i < 3; i++)
                                cout << coeffs[i]->id << "*" << *list[i] << " ";
                        cout << "... ";
                        for (unsigned int i = list.size() - 4; i < list.size();
                             i++)
                                cout << coeffs[i]->id << "*" << *list[i] << " ";
                } else {
                        for (unsigned int i = 0; i < list.size(); i++)
                                cout << coeffs[i]->id << "*" << *list[i] << " ";
                }
                cout << cond << "\n";
        }
        // Push a new array in the end of the arrays' collection
        arrays.push_back(NsIntVarArray());
        // Create a reference to the new array
        NsIntVarArray& array = arrays.back();
        // Add the new array's items
        for (vector<XVariable*>::size_type i = 0; i < list.size(); ++i)
                array.push_back(variable[coeffs.at(i)->id] *
                                variable[list[i]->id]);
        unfoldSumConstraintOperand(array, cond);
}

/// Value is in position index inside the list
void Xcsp3_to_Naxos::buildConstraintElement(string id, vector<XVariable*>& list,
                                            int startIndex, XVariable* index,
                                            RankType rank, int value)
{
        if (verbose) {
                cout << "    element constant (with index) " << id << ": ";
                displayList(list);
                cout << "      "
                     << "value: " << value << ", "
                     << "start index: " << startIndex << ", "
                     << "index: " << *index << "\n";
        }
        if (startIndex != 0) {
                throw invalid_argument("startIndex argument of element "
                                       "constraint should be 0");
        }
        if (rank != ANY) {
                throw invalid_argument("rank argument of element "
                                       "constraint should be any");
        }
        NsIntVar& VarIndex = variable[index->id];
        collectArray(list);
        // TODO: Implement VarArr[Var] constraint
        // pm.add(arrays.back()[VarIndex] == value);
}

/// Variable is in position index inside the list
void Xcsp3_to_Naxos::buildConstraintElement(string id, vector<XVariable*>& list,
                                            int startIndex, XVariable* index,
                                            RankType rank, XVariable* value)
{
        if (verbose) {
                cout << "    element variable (with index) " << id << ": ";
                displayList(list);
                cout << "      "
                     << "value: " << *value << ", "
                     << "start index: " << startIndex << ", "
                     << "index: " << *index << "\n";
        }
        if (startIndex != 0) {
                throw invalid_argument("startIndex argument of element "
                                       "constraint should be 0");
        }
        if (rank != ANY) {
                throw invalid_argument("rank argument of element "
                                       "constraint should be any");
        }
        NsIntVar& VarIndex = variable[index->id];
        NsIntVar& VarValue = variable[value->id];
        collectArray(list);
        // TODO: Implement VarArr[Var] constraint
        // pm.add(arrays.back()[VarIndex] == VarValue);
}

void Xcsp3_to_Naxos::buildObjectiveMinimizeVariable(XVariable* x)
{
        if (verbose)
                cout << "    minimize var " << x << "\n";
        constraintOptimisationMode = true;
        objectiveSign = +1;
        pm.minimize(objectiveSign * variable[x->id]);
}

void Xcsp3_to_Naxos::buildObjectiveMaximizeVariable(XVariable* x)
{
        if (verbose)
                cout << "    maximize var " << x << "\n";
        constraintOptimisationMode = true;
        objectiveSign = -1;
        pm.minimize(objectiveSign * variable[x->id]);
}

void Xcsp3_to_Naxos::buildObjectiveMinimize(ExpressionObjective type,
                                            vector<XVariable*>& list)
{
        constraintOptimisationMode = true;
        objectiveSign = +1;
        // TODO
}

void Xcsp3_to_Naxos::buildObjectiveMaximize(ExpressionObjective type,
                                            vector<XVariable*>& list)
{
        constraintOptimisationMode = true;
        objectiveSign = -1;
        // TODO
}

void Xcsp3_to_Naxos::buildObjectiveMinimize(ExpressionObjective type,
                                            vector<XVariable*>& list,
                                            vector<int>& coefs)
{
        constraintOptimisationMode = true;
        objectiveSign = +1;
        // TODO
}

void Xcsp3_to_Naxos::buildObjectiveMaximize(ExpressionObjective type,
                                            vector<XVariable*>& list,
                                            vector<int>& coefs)
{
        constraintOptimisationMode = true;
        objectiveSign = -1;
        // TODO
}
