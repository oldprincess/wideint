#ifndef WIDEINT_DETAIL_COMMON_HPP
#define WIDEINT_DETAIL_COMMON_HPP

#pragma once

#if defined(__CUDACC__)
#define WIDEINT_HD __device__
#else
#define WIDEINT_HD
#endif

#endif // WIDEINT_DETAIL_COMMON_HPP
