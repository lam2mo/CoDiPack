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
 *          Prof. Robin Hogan, (Univ. of Reading).
 *
 * Originally based on Adept 1.0 (http://www.met.rdg.ac.uk/clouds/adept/)
 * released under GPL 3.0 (Copyright (C) 2012-2013 Robin Hogan and the University of Reading).
 */

/*
 * In order to include this file the user has to define the preprocessor macro NAME, FUNCTION and PRIMAL_FUNCTION.
 * NAME contains the name of the generated operation. FUNCTION represents the normal name of that function
 * e.g. 'operator -' or 'sin'. PRIMAL_FUNCTION is the name of a function which can call the primal operator.
 *
 * The defines NAME, FUNCTION and PRIMAL_FUNCTION will be undefined at the end of this template.
 *
 * The user needs to define further the following functions:
 *
 * gradName
 */

#ifndef NAME
  #error Please define a name for the binary expression.
#endif
#ifndef FUNCTION 
  #error Please define the primal function representation.
#endif
#ifndef PRIMAL_FUNCTION 
  #error Please define a function which calls the primal functions representation.
#endif

#define COMBINE2(A,B) A ## B
#define COMBINE(A,B) COMBINE2(A,B)

#define OP NAME
#define FUNC FUNCTION
#define PRIMAL_CALL PRIMAL_FUNCTION
#define GRADIENT_FUNC   COMBINE(grad, NAME)

/* predefine the struct and the function for higher order derivatives */
template<typename Real, class A> struct OP;

template <typename Real, class A>
inline  codi:: OP<Real, A> FUNC(const codi::Expression<Real, A>& a);

/** 
 * @brief Expression implementation for OP.
 *
 * @tparam Real  The real type used in the active types.
 * @tparam    A  The expression for the argument of the function.
 */
template<typename Real, class A>
struct OP : public Expression<Real, OP<Real, A> > {
  private:
    /** @brief The argument of the function. */
    const A a_;
    /** @brief The result of the function. It is always precomputed. */
    Real result_;
  public:
    /** 
     * @brief Stores the argument of the expression.
     *
     * @param[in] a Argument of the expression.
     */
    OP(const Expression<Real, A>& a) :
      a_(a.cast()),
      result_(PRIMAL_CALL(a.getValue())) {}

  /** 
   * @brief Calculates the jacobie of the expression and hands them down to the argument.
   *
   * For f(x) it calculates df/dx and passes this value as the multiplier to the argument.
   *
   * @param[inout] gradient A helper value for forward implementations. The value is the gradient of the lhs of the expression.
   */
  inline void calcGradient(Real& gradient) const {
    a_.calcGradient(gradient, GRADIENT_FUNC(a_.getValue(), result_));
  }

  /** 
   * @brief Calculates the jacobi of the expression and hands them down to the argument. 
   *
   * For f(x) it calculates multiplier * df/dx and passes this value as the multiplier to the argument.
   *
   * @param[inout] gradient  A helper value for forward implementations. The value is the gradient of the lhs of the expression.
   * @param[in]  multiplier  The Jacobi from the expression where this expression was used as an argument.
   */
  inline void calcGradient(Real& gradient, const Real& multiplier) const {
    a_.calcGradient(gradient, GRADIENT_FUNC(a_.getValue(), result_)*multiplier);
  }

  /** 
   * @brief Return the numerical value of the expression. 
   *
   * @return The value of the expression. 
   */
  inline const Real& getValue() const {
    return result_;
  }
};

/** 
 * @brief Overload for FUNC with the CoDiPack expressions.
 *
 * @param[in] a The argument of the operation.
 *
 * @return The implementing expression OP.
 *
 * @tparam Real The real type used in the active types.
 * @tparam A The expression for the first argument of the function.
 */
template <typename Real, class A>
inline codi:: OP<Real, A> FUNC(const codi::Expression<Real, A>& a) {
  return codi:: OP<Real, A>(a.cast());
}

#undef OP
#undef FUNC
#undef PRIMAL_CALL
#undef GRADIENT_FUNC
#undef COMBINE
#undef COMBINE2

#undef PRIMAL_FUNCTION
#undef FUNCTION
#undef NAME
