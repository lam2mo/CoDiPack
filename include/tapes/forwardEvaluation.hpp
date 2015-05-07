/**
 * CoDiPack, a Code Differentiation Package
 *
 * Copyright (C) 2015 Chair for Scientific Computing, TU Kaiserslautern
 *
 * This file is part of CoDiPack.
 *
 * CoDiPack is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * CoDiPack is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU
 * General Public License along with CoDiPack.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: TODO
 */
#pragma once

#include "activeReal.hpp"
#include "tapeInterface.hpp"

namespace codi {
  /**
   * @brief Tape for the tangent or forward AD mode
   *
   * This tape implements the forward or tangent AD mode. For each statement
   * \f[ y = f(x) \f]
   * the rhs of the equation
   * \f[ \dot{y} = \frac{df}{dx}(x)\cdot \dot {x} \f]
   * is evaluated and stored into the GradientData of \f$y\f$. This is done by calling the store routine of the tape
   * in the assignment operator of ActiveReal. Using expression templates, the evaluation of each expression on the rhs leads to an
   * ActiveReal that then calls the pushJacobi routine to
   * add the Jacobian (the partial derivative of the expression with respect to the inputs)
   * multiplied by the tangent value of the input to the tangent value of \f$y\f$.
   *
   * GradientData is just the same as the active type
   * uses for the storage of the floating point values.
   *
   * @tparam  Real  The floating point type of the tangent data.
   */
  template<typename Real>
  class ForwardEvaluation : public TapeInterface<Real, Real>{
  public:

    typedef Real GradientData;

    /**
     * @brief Evaluates the primal expression and the tangent
     *
     * The store method evaluates the forward AD equation and the primal equation.
     *
     * @param[out]      value  The value of the rhs.
     * @param[out] lhsTangent  The tangent of the lhs.
     * @param[in]         rhs  The expression of the rhs.
     */
    template<typename Rhs>
    inline void store(Real& value, GradientData& lhsTangent, const Rhs& rhs) {
      lhsTangent = Real();
      rhs.calcGradient(lhsTangent);
    }

    /**
     * @brief Evaluates the primal expression and the tangent
     *
     * The store method evaluates the forward AD equation and the primal equation.
     *
     * @param[out]      value  The value of the rhs.
     * @param[out] lhsTangent  The tangent of the lhs.
     * @param[in]         rhs  The expression of the rhs.
     */
    inline void store(Real& value, GradientData& lhsTangent, const ActiveReal<Real, ForwardEvaluation<Real> >& rhs) {
      lhsTangent = rhs.getGradient();
      value = rhs.getValue();
    }

    /**
     * @brief Specialization for store which has a constant value on the rhs.
     *
     * This implementation of store sets the gradient of th active type to zero as the rhs
     * is inactive.
     */
    inline void store(Real& value, GradientData& tangent, const typename TypeTraits<Real>::PassiveReal& rhs) {
      tangent = Real();
      value = rhs;
    }

    /**
     * @brief Adds the jacobi to the tangent value of the expression.
     *
     * This method is called for each value on the rhs. The tangent of the value is added to the
     * tangent of the lhs.
     *
     * @param[in/out] lhsTangent  The tangent of the lhs.
     * @param[]            value  Not used
     * @param[in]     curTangent  The tangent of the current rhs value.
     */
    inline void pushJacobi(Real& lhsTangent, const Real& /*value*/, const GradientData& curTangent) {
      lhsTangent += curTangent;
    }

    /**
     * @brief Adds the jacobi to the tangent value of the expression.
     *
     * This method is called for each value on the rhs. The tangent of the value times the jacobi is added to the
     * tangent of the lhs.
     *
     * @param[in/out] lhsTangent  The tangent of the lhs.
     * @param[in]         jacobi  The jacobi value of the operation.
     * @param[]            value  Not used
     * @param[in]     curTangent  The tangent of the current rhs value.
     */
    inline void pushJacobi(Real& lhsTangent, const Real& jacobi, const Real& /*value*/, const GradientData& curTangent) {
      lhsTangent += jacobi * curTangent;
    }

    /**
     * @brief Tangent is set to zero.
     *
     * The tangent is initialized with zero.
     *
     * @param[]      value  Not used.
     * @param[out] tangent  Set to zero.
     */
    inline void initGradientData(Real& /*value*/, GradientData& tangent) {
      tangent = Real();
    }

    /**
     * @brief Nothing to do.
     */
    inline void destroyGradientData(Real& /*value*/, GradientData& /*tangent*/) {
      /* do nothing */
    }

    /**
     * Sets the gradient data to the tangent value.
     *
     * @param[out]   tangent  The tangent value of the active type.
     * @param[in] newTangent  The new tangent value.
     */
    inline void setGradient(GradientData& tangent, const Real& newTangent) {
      tangent = newTangent;
    }

    /**
     * Returns the tangent value of the active type.
     *
     * @param[in]  tangent  The gradient data of the active type is the tangent.
     *
     * @return The tangent value of the active type.
     */
    inline Real getGradient(const GradientData& tangent) const {
      return tangent;
    }

    /**
     * Returns the tangent value of the active type.
     *
     * @param[in]  tangent  The gradient data of the active type is the tangent.
     *
     * @return The tangent value of the active type.
     */
    inline Real& gradient(GradientData& tangent) {
      return tangent;
    }
  };
}


