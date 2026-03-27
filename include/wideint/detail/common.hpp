#ifndef WIDEINT_DETAIL_COMMON_HPP
#define WIDEINT_DETAIL_COMMON_HPP

#pragma once

#if defined(__CUDACC__)
#define INTX_HD __device__
#else
#define INTX_HD
#endif

#endif // WIDEINT_DETAIL_COMMON_HPP
