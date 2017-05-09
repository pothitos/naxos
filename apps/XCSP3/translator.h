/// @file
/// Interface between XCSP3 definitions and Naxos
///
/// Part of https://github.com/pothitos/naxos

/*
 * parser for CSP instances represented in XCSP3 Format
 *
 * Copyright (c) 2015 xcsp.org (contact <at> xcsp.org)
 * Copyright (c) 2008 Olivier ROUSSEL (olivier.roussel <at> cril.univ-artois.fr)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef COSOCO_XCSP3PRINTCALLBACKS_H
#define COSOCO_XCSP3PRINTCALLBACKS_H

#include <XCSP3CoreCallbacks.h>
#include <naxos.h>
#include <unordered_map>

/// The XCSP3 parser invokes the functions of this class
class Xcsp3_to_Naxos : public XCSP3Core::XCSP3CoreCallbacks {
    private:
        /// Display default parser's messages
        bool verbose;

        /// The CSP has an objective
        bool constraintOptimisationMode;

        /// The sign of the objective variable is -1 for maximization
        int objectiveSign;

        /// The CSP definition parsing has already begun
        bool instanceAlreadyBegan;

        /// The problem manager
        naxos::NsProblemManager pm;

        /// Hash table with strings (ID) as keys and variables as values
        typedef std::unordered_map<std::string, naxos::NsIntVar> stringToVar_t;

        /// Maps a string to a constrained variable
        stringToVar_t variable;

        /// The variables will be instantiated with NsgLabeling
        naxos::NsIntVarArray AllVars;

        /// The string with all the names (IDs) of the variables
        std::string AllVarsNames;

        /// Facilitates the displaying an instantiation of the variables
        void recordVar(const std::string& VarName, naxos::NsIntVar& Var)
        {
                AllVarsNames += VarName + " ";
                AllVars.push_back(Var);
        }

        /// @{
        /// @name Storing and displaying a solution

    private:
        /// The last solution found
        naxos::NsDeque<naxos::NsInt> AllVarsValues;

    public:
        /// Returns true if a solution has been recorded
        bool solutionIsRecored(void) const
        {
                return !AllVarsValues.empty();
        }

        /// Stores the current solution
        void recordSolution(void)
        {
                AllVarsValues.clear();
                for (auto Var : AllVars)
                        AllVarsValues.push_back(Var.min());
                // We store the minimum value as a representative of the
                // domain in case it contains more than one value (when
                // it is a non-instantiated/useless variable).
        }

        /// Prints the solution in the XCSP3 Competition format
        void printSolution(void) const
        {
                std::cout << "v <instantiation>\n"
                          << "v   <list> " << AllVarsNames << "</list>\n"
                          << "v   <values> ";
                for (auto value : AllVarsValues)
                        std::cout << value << " ";
                std::cout << "</values>\n"
                          << "v </instantiation>\n";
        }

        /// @}

    private:
        /// Collect created arrays here for future garbage destruction
        naxos::NsList<naxos::NsIntVarArray> arrays;

        /// Convert an XCSP3 array into a Naxos array
        void collectArray(const std::vector<XCSP3Core::XVariable*>& list)
        {
                arrays.push_back(naxos::NsIntVarArray());
                for (auto var : list)
                        arrays.back().push_back(variable[var->id]);
        }

    public:
        /// Constructor
        Xcsp3_to_Naxos(const bool verbose_init)
          : verbose(verbose_init),
            constraintOptimisationMode(false),
            instanceAlreadyBegan(false)
        {
        }

        /// Returns true if we are in COP mode
        bool constraintOptimisation(void) const
        {
                return constraintOptimisationMode;
        }

        /// Returns the objective variable's current value
        naxos::NsInt getObjectiveValue(void) const
        {
                return objectiveSign * pm.getObjective().value();
        }

        /// Exposes problem manager's search function
        bool nextSolution(void)
        {
                return pm.nextSolution();
        }

        /// @{
        /// @name Mark where the tags start or end

        virtual void beginInstance(XCSP3Core::InstanceType type) override;

        virtual void endInstance() override;

        virtual void beginVariables() override;

        virtual void endVariables() override;

        virtual void beginVariableArray(std::string id) override;

        virtual void endVariableArray() override;

        virtual void beginConstraints() override;

        virtual void endConstraints() override;

        virtual void beginGroup(std::string id) override;

        virtual void endGroup() override;

        virtual void beginBlock(std::string classes) override;

        virtual void endBlock() override;

        virtual void beginObjectives() override;

        virtual void endObjectives() override;

        /// @}

        /// @{
        /// @name Constrained variables definition

        virtual void buildVariableInteger(std::string id, int minValue,
                                          int maxValue) override;

        virtual void buildVariableInteger(std::string id,
                                          std::vector<int>& values) override;

        /// @}

        /// @{
        /// @name Intensional Constraints

        virtual void buildConstraintIntension(std::string id,
                                              std::string expr) override;

        virtual void buildConstraintPrimitive(std::string id,
                                              XCSP3Core::OrderType op,
                                              XCSP3Core::XVariable* x, int k,
                                              XCSP3Core::XVariable* y) override;

        /// @}

        /// @{
        /// @name Extensional Constraints

        virtual void
        buildConstraintExtension(std::string id,
                                 std::vector<XCSP3Core::XVariable*> list,
                                 std::vector<std::vector<int>>& tuples,
                                 bool support, bool hasStar) override;

        virtual void buildConstraintExtension(std::string id,
                                              XCSP3Core::XVariable* var,
                                              std::vector<int>& tuples,
                                              bool support,
                                              bool hasStar) override;

        virtual void
        buildConstraintExtensionAs(std::string id,
                                   std::vector<XCSP3Core::XVariable*> list,
                                   bool support, bool hasStar) override;

        /// @}

        /// @{
        /// @name AllDifferent

        virtual void buildConstraintAlldifferent(
            std::string id, std::vector<XCSP3Core::XVariable*>& list) override;

        /// @}

        /// @{
        /// @name Sum

    private:
        template <class T>
        void unfoldSumConstraintCondition(naxos::NsIntVarArray& array,
                                          const XCSP3Core::OrderType condition,
                                          T& operand);

        void unfoldSumConstraintOperand(naxos::NsIntVarArray& array,
                                        const XCSP3Core::XCondition& cond);

    public:
        virtual void
        buildConstraintSum(std::string id,
                           std::vector<XCSP3Core::XVariable*>& list,
                           XCSP3Core::XCondition& cond) override;

        virtual void buildConstraintSum(
            std::string id, std::vector<XCSP3Core::XVariable*>& list,
            std::vector<int>& coeffs, XCSP3Core::XCondition& cond) override;

        virtual void
        buildConstraintSum(std::string id,
                           std::vector<XCSP3Core::XVariable*>& list,
                           std::vector<XCSP3Core::XVariable*>& coeffs,
                           XCSP3Core::XCondition& cond) override;

        /// @}

        /// @{
        /// @name Element

        virtual void
        buildConstraintElement(std::string id,
                               std::vector<XCSP3Core::XVariable*>& list,
                               int startIndex, XCSP3Core::XVariable* index,
                               XCSP3Core::RankType rank, int value) override;

        virtual void buildConstraintElement(
            std::string id, std::vector<XCSP3Core::XVariable*>& list,
            int startIndex, XCSP3Core::XVariable* index,
            XCSP3Core::RankType rank, XCSP3Core::XVariable* value) override;

        /// @}

        /// @{
        /// @name Constraint Optimization for a variable

        virtual void
        buildObjectiveMinimizeVariable(XCSP3Core::XVariable* x) override;

        virtual void
        buildObjectiveMaximizeVariable(XCSP3Core::XVariable* x) override;

        /// @}

        /// @{
        /// @name Constraint Optimization for an array of variables

        virtual void buildObjectiveMinimize(
            XCSP3Core::ExpressionObjective type,
            std::vector<XCSP3Core::XVariable*>& list) override;

        virtual void buildObjectiveMaximize(
            XCSP3Core::ExpressionObjective type,
            std::vector<XCSP3Core::XVariable*>& list) override;

        virtual void
        buildObjectiveMinimize(XCSP3Core::ExpressionObjective type,
                               std::vector<XCSP3Core::XVariable*>& list,
                               std::vector<int>& coefs) override;

        virtual void
        buildObjectiveMaximize(XCSP3Core::ExpressionObjective type,
                               std::vector<XCSP3Core::XVariable*>& list,
                               std::vector<int>& coefs) override;

        /// @}
};

#endif // COSOCO_XCSP3PRINTCALLBACKS_H
