/**
 * CoDiPack, a Code Differentiation Package
 *
 * Copyright (C) 2015 Chair for Scientific Computing (SciComp), TU Kaiserslautern
 * Homepage: http://www.scicomp.uni-kl.de
 * Contact:  Prof. Nicolas R. Gauger (codi@scicomp.uni-kl.de)
 *
 * Lead developers: Max Sagebaum, Tim Albring (SciComp, TU Kaiserslautern)
 *
 * This file is part of CoDiPack (http://www.scicomp.uni-kl.de/software/codi).
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
 * Authors: Max Sagebaum, Tim Albring, (SciComp, TU Kaiserslautern)
 */

#pragma once

#include "../configure.h"
#include "../typeTraits.hpp"

namespace codi {

  /**
   * @brief Interface common to all tapes.
   *
   * The basic interface each tape has to implement. It defines functions which are used
   * by the active type to signal the tape when an active type is created or destroyed. For
   * each operation the active type calls also the store operation to inform the tape that
   * an expression is assigned to an active type.
   *
   * @tparam             Real  Floating point type of the gradients.
   * @tparam GradientDataType  The data the tape uses to identify each active variable
   *                             and where the tape can store information about the
   *                             gradient.
   */
  template <typename Real, typename GradientDataType>
  class TapeInterface {
  public:

    /**
     * @brief The data for the gradient information of the tape.
     *
     * Each tape can define a data for the gradient which each active type will
     * define in its own class. The tape can use this data to identify each active
     * type and compute the gradient information.
     */
    typedef GradientDataType GradientData;

    typedef typename TypeTraits<Real>::PassiveReal PassiveReal;

    /**
     * This functions are called from the expression templates. They tell the
     * tape about the operations which is evaluated.
     */

    /**
     * @brief Set the value of the expression and evaluate the gradient.
     *
     * This function is called for every operation which resembles
     *
     *   lhs = rhs;
     *
     * Store sets the primal value of the operation which is stored and the tape can
     * do the evaluation of the gradient and perform other appropriate steps.
     *
     * @param[out]          lhsValue    The primal value of the lhs. This value is set to the value
     *                                  of the right hand side.
     * @param[out]   lhsGradientData    The gradient data of the lhs. The tape will update the gradient data
     *                                  according to the expression on the right hand side.
     * @param[in]                rhs    The right hand side expression of the assignment.
     *
     * @tparam Rhs The expression on the rhs of the statement.
     */
    template<typename Rhs>
    void store(Real& lhsValue, GradientData& lhsGradientData, const Rhs& rhs);

    inline void pushPassive(const PassiveReal& value) {
      CODI_UNUSED(value);
    }

    /**
     * @brief Add a jacobi of 1.0 to the tape.
     *
     * The optimized version of push jacobi which signals the tape that the jacobi
     * corresponding to the tape is 1.0.
     *
     * @param[inout]   gradient  A handle to the gradient of the operation. Mostly used for the forward mode.
     * @param[in]         value  The value of the active type which pushes the jacobi.
     * @param[in]  gradientData  The gradient data of the active type which pushes the jacobi.
     *
     * @tparam EvalData  Data for the evaluation of the AD mode.
     */
    template<typename EvalData>
    void pushJacobi(EvalData& gradient, const Real& value, const GradientData& gradientData);

    /**
     * @brief Add a jacobi to the tape.
     *
     * The general version of push jacobi which signals the tape that the jacobi is used
     * in the evaluation and needs to be evaluated or stored.
     *
     * @param[inout]   gradient  A handle to the gradient of the operation. Mostly used for the forward mode.
     * @param[in]        jacobi  The value of the jacobi.
     * @param[in]         value  The value of the active type which pushes the jacobi.
     * @param[in]  gradientData  The gradient data of the active type which pushes the jacobi.
     *
     * @tparam EvalData  Data for the evaluation of the AD mode.
     */
    template<typename EvalData>
    void pushJacobi(EvalData& gradient, const Real& jacobi, const Real& value, const GradientData& gradientData);

    /**
     * @brief Called in the construction of a active type.
     *
     * The tape can initialize its gradient data for the active type.
     *
     * @param[inout]        value  The value of the active type.
     * @param[inout] gradientData  The gradient data which needs to be initialized.
     */
    virtual void initGradientData(Real& value, GradientData& gradientData) = 0;

    /**
     * @brief Destroy the gradient data of a active type.
     *
     * The tape can destroy the gradient data of the active type. This method is
     * called in the destructor of an active type.
     *
     * @param[inout]        value  The value of the active type.
     * @param[inout] gradientData  The gradient data which needs to be destroyed.
     */
    virtual void destroyGradientData(Real& value, GradientData& gradientData) = 0;


    /**
     * Access functions for the gradient information.
     */

    /**
     * @brief Set the gradient of the gradient data.
     *
     * The tape can set the gradient which corresponds to the gradient data.
     *
     * @param[inout]  value  The gradient data of the active type.
     * @param[in]   gradient  The new gradient value for the active type.
     */
    virtual void setGradient(GradientData& value, const Real& gradient) = 0;

    /**
     * @brief Get the gradient of the gradient data.
     *
     * The tape  returns the gradient which corresponds to the gradient data.
     *
     * @param[in] value  The gradient data of the active type.
     *
     * @return The gradient which belongs to the active type.
     */
    virtual Real getGradient(const GradientData& value) const = 0;

    /**
    * @brief Get the gradient of the gradient data as reference.
    *
    * The tape  returns the gradient which corresponds to the gradient data as a reference.
    *
    * @param[in] value  The gradient data of the active type.
    *
    * @return The gradient which belongs to the active type as a reference.
    */
    virtual Real& gradient(GradientData& value) = 0;
  };
}
