#ifndef BASALT_WARNINGS_POP_HPP
#define BASALT_WARNINGS_POP_HPP

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic pop
#endif // defined(__clang__)

#endif //! BASALT_WARNINGS_POP_HPP
