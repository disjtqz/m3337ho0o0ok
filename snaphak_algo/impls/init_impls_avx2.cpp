

#include "snaphakalgo_predef.hpp"
#include "snaphakalgo.hpp"

#ifndef SHALGO_ONE_SEGMENT
#define		IMPL_CODE_SEG		CS_CODE_SEG(".avx2_path")
#else
#define		IMPL_CODE_SEG
#endif

#define		IMPL_VECTOR_WIDTH	32
#include "include_impls.hpp"


__INIT__IMPLS(avx2)
