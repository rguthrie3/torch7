#ifndef TH_GENERIC_FILE
#define TH_GENERIC_FILE "generic/THVectorDispatch.c"
#else

#include "simd/simd.h"
#include <assert.h>

/* For now there are only SIMD implementations for FLOAT and DOUBLE.
 * Hopefully in the future this can be made totally generic (e.g, there are SIMD implementations
 * for a lot of functions */
#if defined(TH_REAL_IS_DOUBLE) || defined(TH_REAL_IS_FLOAT)
/* Each function with multiple implementations has:
 * 1. A DISPATCHPTR which will be initialized to point to the best available implementation for the host
 * 2. A DISPATCHTABLE which holds pointers to each implementation of a function, and a value indicating
 *    which SIMD extension a given implementation uses
 * 3. A dispatch stub, which is what is actually called by clients, that simply wraps the dispatch pointer.
 */

static void (*THVector_(fill_DISPATCHPTR))(real *, const real, const long) = NULL;
static FunctionDescription THVector_(fill_DISPATCHTABLE)[] = {
  FUNCTION_IMPL(THVector_(fill_SSE), SIMDExtension_SSE),
  FUNCTION_IMPL(THVector_(fill_SCALAR), SIMDExtension_SCALAR)
};
static TH_INLINE void THVector_(fill)(real *x, const real c, const long n) {
  assert(THVector_(fill_DISPATCHPTR));
  THVector_(fill_DISPATCHPTR)(x, c, n);
}


static void (*THVector_(add_DISPATCHPTR))(real *, const real *, const real, const long) = NULL;
static FunctionDescription THVector_(add_DISPATCHTABLE)[] = {
  FUNCTION_IMPL(THVector_(add_SSE), SIMDExtension_SSE),
  FUNCTION_IMPL(THVector_(add_SCALAR), SIMDExtension_SCALAR)
};
static TH_INLINE void THVector_(add)(real *y, const real *x, const real c, const long n) {
  assert(THVector_(add_DISPATCHPTR));
  THVector_(add_DISPATCHPTR)(y, x, c, n);
}


static void (*THVector_(diff_DISPATCHPTR))(real *, const real *, const real *, const long) = NULL;
static FunctionDescription THVector_(diff_DISPATCHTABLE)[] = {
  FUNCTION_IMPL(THVector_(diff_SSE), SIMDExtension_SSE),
  FUNCTION_IMPL(THVector_(diff_SCALAR), SIMDExtension_SCALAR)
};
static TH_INLINE void THVector_(diff)(real *z, const real *x, const real *y, const long n) {
  assert(THVector_(diff_DISPATCHPTR));
  THVector_(diff_DISPATCHPTR)(z, x, y, n);
}


static void (*THVector_(scale_DISPATCHPTR))(real *, const real, const long);
static FunctionDescription THVector_(scale_DISPATCHTABLE)[] = {
  FUNCTION_IMPL(THVector_(scale_SSE), SIMDExtension_SSE),
  FUNCTION_IMPL(THVector_(scale_SCALAR), SIMDExtension_SCALAR)
};
static TH_INLINE void THVector_(scale)(real *y, const real c, const long n) {
  assert(THVector_(scale_DISPATCHPTR));
  THVector_(scale_DISPATCHPTR)(y, c, n);
}


static void (*THVector_(mul_DISPATCHPTR))(real *, const real *, const long);
static FunctionDescription THVector_(mul_DISPATCHTABLE)[] = {
  FUNCTION_IMPL(THVector_(mul_SSE), SIMDExtension_SSE),
  FUNCTION_IMPL(THVector_(mul_SCALAR), SIMDExtension_SCALAR)
};
static TH_INLINE void THVector_(mul)(real *y, const real *x, const long n) {
  assert(THVector_(mul_DISPATCHPTR));
  THVector_(mul_DISPATCHPTR);
}

#endif

/* This needs to be called in order to initialize the dispatch pointers at runtime.
 * This function simply checks what SIMD extensions are available, and then walks the dispatch table
 * to choose the best function.
 * NOTE: As implemented, it will initialize the dispatch pointer to the first supported function.
 *       This means that in the dispatch tables, implementations supporting more recent extensions
 *       need to come first
 */
int THVector_(vectorDispatchInit)()
{
  uint32_t hostSimdExts = detectHostSIMDExtensions();
#if defined(TH_REAL_IS_DOUBLE) || defined(TH_REAL_IS_FLOAT)
  INIT_DISPATCH_PTR(fill);
  INIT_DISPATCH_PTR(add);
  INIT_DISPATCH_PTR(diff);
  INIT_DISPATCH_PTR(scale);
  INIT_DISPATCH_PTR(mul);
#endif
}

#endif
