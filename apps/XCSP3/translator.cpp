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
        variableStore[id] = NsIntVar(pm, minValue, maxValue);
        pm.removeLastVar();
        pm.addVar(&variable(id));
        recordVar(id, variable(id));
}

namespace {

/// Leaves only the 'supports' values of the constrained variable
void removeUnsupportedValues(NsIntVar& Var, NsDeque<NsInt>& supports)
{
        // Check if there's no support
        if (supports.empty()) {
                Var.removeAll();
                return;
        }
        // Ensure that the values are ordered
        sort(supports.begin(), supports.end());
        // Remove values before the fist and after the last support
        Var.remove(NsMINUS_INF, supports.front() - 1);
        Var.remove(supports.back() + 1, NsPLUS_INF);
        // Remove gaps from the variable's domain
        for (NsDeque<NsInt>::size_type i = 0; i < supports.size() - 1; ++i)
                for (NsInt val = supports[i] + 1; val < supports[i + 1]; ++val)
                        Var.remove(val);
}

/// Converts vector to NsDeque before removing unsupported values
void removeUnsupportedValues(NsIntVar& Var, vector<int>& supports)
{
        NsDeque<NsInt> supportsDeque;
        for (auto val : supports)
                supportsDeque.push_back(val);
        removeUnsupportedValues(Var, supportsDeque);
}

} // end namespace

void Xcsp3_to_Naxos::buildVariableInteger(string id, vector<int>& values)
{
        if (verbose) {
                cout << "    var " << id << ": ";
                displayList(values);
        }
        if (values.empty()) {
                throw domain_error("The domain of the constrained variable '" +
                                   id + "' cannot be empty");
        }
        // Ensure that the values are ordered
        sort(values.begin(), values.end());
        // Set variable's domain to be the whole values[0]..values[N-1]
        variableStore[id] = NsIntVar(pm, values.front(), values.back());
        pm.removeLastVar();
        pm.addVar(&variable(id));
        removeUnsupportedValues(variable(id), values);
        recordVar(id, variable(id));
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
                throw invalid_argument("Invalid intension constraint '" +
                                       comparison + "'");
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
                        if (insideParentheses || !operation.empty()) {
                                throw invalid_argument("Only one arithmetic "
                                                       "operation is permitted "
                                                       "in '" +
                                                       expr + "'");
                        }
                        insideParentheses = true;
                        operation = currentToken;
                        currentToken = "";
                } else if (c == ')') {
                        if (!insideParentheses) {
                                throw invalid_argument(
                                    "Unmatched parenthesis in '" + expr + "'");
                        }
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

/// Converts the left-hand constraint part into its corresponding type
void Xcsp3_to_Naxos::unfoldLeftToken(
    OrderType comparison, const string& tokenLeft, const string& tokenRight,
    const string& operation, const string& operand1, const string& operand2)
{
        NsInt constant;
        if (tokenLeft.empty()) { // Left token is an expression
                NsIntVar& VarTmp =
                    unfoldArithmExprToken1(operation, operand1, operand2);
                unfoldRightToken(comparison, VarTmp, tokenRight);
        } else if (strToLong(tokenLeft, constant)) { // Left token is a constant
                unfoldRightToken(comparison, constant, tokenRight, operation,
                                 operand1, operand2);
        } else { // Left token is a variable
                unfoldRightToken(comparison, variable(tokenLeft), tokenRight,
                                 operation, operand1, operand2);
        }
}

/// Materializes the right-hand constraint part, when the left is a constant
void Xcsp3_to_Naxos::unfoldRightToken(OrderType comparison, NsInt tokenLeft,
                                      const string& tokenRight,
                                      const string& operation,
                                      const string& operand1,
                                      const string& operand2)
{
        if (tokenRight.empty()) { // Right token is an expression
                NsIntVar& VarTmp =
                    unfoldArithmExprToken1(operation, operand1, operand2);
                addIntensionConstraint(comparison, tokenLeft, VarTmp);
        } else { // Right token is a variable
                addIntensionConstraint(comparison, tokenLeft,
                                       variable(tokenRight));
        }
}

/// Converts the right-hand constraint part into its corresponding type
void Xcsp3_to_Naxos::unfoldRightToken(OrderType comparison, NsIntVar& tokenLeft,
                                      const string& tokenRight,
                                      const string& operation,
                                      const string& operand1,
                                      const string& operand2)
{
        NsInt constant;
        if (tokenRight.empty()) { // Right token is an expression
                NsIntVar& VarTmp =
                    unfoldArithmExprToken1(operation, operand1, operand2);
                addIntensionConstraint(comparison, tokenLeft, VarTmp);
        } else if (strToLong(tokenRight,
                             constant)) { // Right token is a constant
                addIntensionConstraint(comparison, tokenLeft, constant);
        } else { // Right token is a variable
                addIntensionConstraint(comparison, tokenLeft,
                                       variable(tokenRight));
        }
}

/// Materializes the intensional constraint
template <typename T1, typename T2>
void Xcsp3_to_Naxos::addIntensionConstraint(OrderType comparison, T1& tokenLeft,
                                            T2& tokenRight)
{
        switch (comparison) {
        case EQ:
                pm.add(tokenLeft == tokenRight);
                break;
        case NE:
                pm.add(tokenLeft != tokenRight);
                break;
        case LT:
                pm.add(tokenLeft < tokenRight);
                break;
        case LE:
                pm.add(tokenLeft <= tokenRight);
                break;
        case GT:
                pm.add(tokenLeft > tokenRight);
                break;
        case GE:
                pm.add(tokenLeft >= tokenRight);
                break;
        default:
                throw invalid_argument("Unsupported operator for intension "
                                       "constraint");
                break;
        }
}

/// Converts the string 'operand1 operation operand2' into a Naxos expression
NsIntVar& Xcsp3_to_Naxos::unfoldArithmExprToken1(const string& operation,
                                                 const string& operand1,
                                                 const string& operand2)
{
        NsInt constant;
        if (strToLong(operand1, constant)) // Left operand is a constant
                return unfoldArithmExprToken2(operation, constant, operand2);
        else // Left operand is a variable
                return unfoldArithmExprToken2(operation, variable(operand1),
                                              operand2);
}

/// Materializes the right-hand operand, when the left is a constant
NsIntVar& Xcsp3_to_Naxos::unfoldArithmExprToken2(const string& operation,
                                                 NsInt operand1,
                                                 const string& operand2)
{
        // Right operand is necessarily a variable
        return unfoldArithmExprOperation(operation, operand1,
                                         variable(operand2));
}

/// Converts the right-hand operand into its corresponding type
NsIntVar& Xcsp3_to_Naxos::unfoldArithmExprToken2(const string& operation,
                                                 NsIntVar& operand1,
                                                 const string& operand2)
{
        NsInt constant;
        if (strToLong(operand2, constant)) // Right operand is a constant
                return unfoldArithmExprOperation(operation, operand1, constant);
        else // Right operand is a variable
                return unfoldArithmExprOperation(operation, operand1,
                                                 variable(operand2));
}

/// Materializes the arithmetic operation string
template <typename T1, typename T2>
NsIntVar& Xcsp3_to_Naxos::unfoldArithmExprOperation(const string& operation,
                                                    T1& operand1, T2& operand2)
{
        if (operation == "add")
                return (operand1 + operand2).post();
        else if (operation == "sub")
                return (operand1 - operand2).post();
        else if (operation == "mul")
                return (operand1 * operand2).post();
        else if (operation == "div")
                return (operand1 / operand2).post();
        else if (operation == "mod")
                return (operand1 % operand2).post();
        else if (operation == "dist")
                return (NsAbs(operand1 - operand2)).post();
        else
                throw invalid_argument("Unsupported operator '" + operation +
                                       "' for intension constraint");
}

/// Intension constraint
void Xcsp3_to_Naxos::buildConstraintIntension(string id, string expr)
{
        if (verbose)
                cout << "    intension " << id << ": " << expr << "\n";
        if (expr.size() < 4 || expr[2] != '(' || expr[expr.size() - 1] != ')') {
                throw invalid_argument("Invalid intension constraint '" + expr +
                                       "'");
        }
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
        NsIntVar& VarX = variable(x->id);
        NsIntVar& VarY = variable(y->id);
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

/// Materializes the unary extensional constraint
void Xcsp3_to_Naxos::addUnaryExtensionConstraint(XVariable* var, bool support)
{
        NsIntVar& Var = variable(var->id);
        NsDeque<NsInt>& tuple = tuplesStore.back().back();
        if (support) {
                removeUnsupportedValues(Var, tuple);
        } else {
                for (auto val : tuple)
                        Var.remove(val);
        }
}

/// Materializes the extensional constraint
void Xcsp3_to_Naxos::addExtensionConstraint(vector<XVariable*> list,
                                            bool support)
{
        collectArray(list);
        if (support)
                pm.add(NsSupports(arrays.back(), tuplesStore.back()));
        else
                pm.add(NsConflicts(arrays.back(), tuplesStore.back()));
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
        collectTuples(tuples);
        addUnaryExtensionConstraint(var, support);
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
        collectTuples(tuples);
        addExtensionConstraint(list, support);
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
        if (list.size() == 1)
                addUnaryExtensionConstraint(list[0], support);
        else
                addExtensionConstraint(list, support);
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
                                             variable(cond.var));
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
        collectArray(list, coeffs);
        unfoldSumConstraintOperand(arrays.back(), cond);
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
                array.push_back(variable(coeffs.at(i)->id) *
                                variable(list[i]->id));
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
        NsIntVar& VarIndex = variable(index->id);
        collectArray(list);
        pm.add(arrays.back()[VarIndex] == value);
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
        NsIntVar& VarIndex = variable(index->id);
        NsIntVar& VarValue = variable(value->id);
        collectArray(list);
        pm.add(arrays.back()[VarIndex] == VarValue);
}

void Xcsp3_to_Naxos::buildObjectiveMinimizeVariable(XVariable* x)
{
        if (verbose)
                cout << "    minimize var " << x << "\n";
        constrainedOptimizationMode = true;
        objectiveSign = +1;
        pm.minimize(objectiveSign * variable(x->id));
}

void Xcsp3_to_Naxos::buildObjectiveMaximizeVariable(XVariable* x)
{
        if (verbose)
                cout << "    maximize var " << x << "\n";
        constrainedOptimizationMode = true;
        objectiveSign = -1;
        pm.minimize(objectiveSign * variable(x->id));
}

void Xcsp3_to_Naxos::buildObjectiveMinimize(ExpressionObjective type,
                                            vector<XVariable*>& list)
{
        objectiveSign = +1;
        collectArray(list);
        addObjectiveArray(type);
}

void Xcsp3_to_Naxos::buildObjectiveMaximize(ExpressionObjective type,
                                            vector<XVariable*>& list)
{
        objectiveSign = -1;
        collectArray(list);
        addObjectiveArray(type);
}

void Xcsp3_to_Naxos::buildObjectiveMinimize(ExpressionObjective type,
                                            vector<XVariable*>& list,
                                            vector<int>& coefs)
{
        objectiveSign = +1;
        collectArray(list, coefs);
        addObjectiveArray(type);
}

void Xcsp3_to_Naxos::buildObjectiveMaximize(ExpressionObjective type,
                                            vector<XVariable*>& list,
                                            vector<int>& coefs)
{
        objectiveSign = -1;
        collectArray(list, coefs);
        addObjectiveArray(type);
}

/// Sets the objective goal for arrays
void Xcsp3_to_Naxos::addObjectiveArray(ExpressionObjective type)
{
        constrainedOptimizationMode = true;
        NsIntVarArray& objArray = arrays.back();
        switch (type) {
        case SUM_O:
                pm.minimize(objectiveSign * NsSum(objArray));
                break;
        case MINIMUM_O:
                pm.minimize(objectiveSign * NsMin(objArray));
                break;
        case MAXIMUM_O:
                pm.minimize(objectiveSign * NsMax(objArray));
                break;
        default:
                throw invalid_argument("Unsupported objective type");
                break;
        }
}
