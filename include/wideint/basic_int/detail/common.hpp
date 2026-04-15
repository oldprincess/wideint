#ifndef WIDEINT_BASIC_INT_DETAIL_COMMON_HPP
#define WIDEINT_BASIC_INT_DETAIL_COMMON_HPP

#pragma once

#if defined(__CUDACC__)
#define WIDEINT_HD __host__ __device__
#else
#define WIDEINT_HD
#endif

#endif // WIDEINT_BASIC_INT_DETAIL_COMMON_HPP
