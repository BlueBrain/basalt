#ifndef BASALT_GSL_HPP
#define BASALT_GSL_HPP
// https://clang.llvm.org/extra/clang-tidy/checks/cppcoreguidelines-owning-memory.html
namespace gsl {
template <typename T>
using owner = T;
}
#endif  // BASALT_GSL_HPP
