

#include "snaphakalgo_predef.hpp"
#include "snaphakalgo.hpp"


#define		IMPL_CODE_SEG		CS_CODE_SEG(".avx512_path")
#define		IMPL_VECTOR_WIDTH	64
#include "include_impls.hpp"


__INIT__IMPLS(avx512)