/**
 * \file src/basalt/py_helpers.hpp
 * Provides utility functions to Python/C++ object conversions
 */

#ifndef BASALT_PY_HELPERS_HPP
#define BASALT_PY_HELPERS_HPP

#include <array>
#include <ostream>
#include <pybind11/numpy.h>

namespace basalt {
/**
 * \name Serialization helpers
 * \{
 */

/**
 * Write content of a string stream to a NumPy array
 * \param oss string stream
 * \return new NumPY array
 */
inline pybind11::array_t<char> to_py_array(const std::ostringstream& oss) {
    const std::string& data = oss.str();
    auto result = pybind11::array_t<char>(data.size());
    auto buffer = result.request();
    strncpy(reinterpret_cast<char*>(buffer.ptr), data.data(), data.size());
    return result;
}

/**
 * Read a NumPy and fill a string stream
 * \param data NumPy array to read
 * \param iss string stream to fill
 */
inline void from_py_array(pybind11::array_t<char>& data,
                          std::istringstream& iss) {
    auto request = data.request();
    iss.rdbuf()->pubsetbuf(reinterpret_cast<char*>(request.ptr), request.size);
}

/**
 * Write a standard vector to a string stream
 * \tparam T vector value type
 * \param oss string stream to write to
 * \param data vector to read data from
 */
template <typename T>
void serialize_vector(std::ostringstream& oss, const std::vector<T>& data) {
    oss << data.size() << ' ';
    for (const auto idx : data) {
        oss << idx << ' ';
    }
}

/**
 * Fill a standard vector from a string stream
 * \tparam T vector value type
 * \param iss string stream to read
 * \param data vector to fill
 */
template <typename T>
void deserialize_vector(std::istringstream& iss, std::vector<T>& data) {
    std::size_t count;
    iss >> count;
    data.reserve(count);
    const auto begin = data.begin();
    for (auto i = 0u; i < count; ++i) {
        T value;
        iss >> value;
        data.insert(begin + i, value);
    }
}

/**
 * \}
 */

/**
 * \name Conversion helper
 * \{
 */

/**
 * Fill a standard vector from a NumPy array
 * \tparam T vector data type
 */
template <typename T>
void fill_vector(pybind11::array_t<T>& array, std::vector<T>& vector) {
    auto buffer = array.request(false);
    vector.reserve(buffer.size);
    auto ptr = reinterpret_cast<typename std::add_pointer<T>::type>(buffer.ptr);
    std::copy(ptr, ptr + buffer.size, std::back_inserter(vector));
}

/**
 * Fill a standard vector of \a std::array from a NumPy array
 */
template <typename T, std::size_t N>
void fill_vector(pybind11::buffer& buffer,
                 std::vector<std::array<T, N>>& vector) {
    auto info = buffer.request(false);
    if (info.size == 0) {
        return;
    }
    if (info.format != pybind11::format_descriptor<T>::format()) {
        throw std::runtime_error("Incompatible buffer data type");
    }
    if (info.ndim != 2) {
        throw std::runtime_error("Incompatible buffer dimension");
    }
    if (info.shape[1] != N) {
        std::ostringstream oss;
        oss << "Invalid dimension 1 size. Expected " << N << " but got "
            << info.shape[1];
        throw std::runtime_error(oss.str());
    }
    auto ptr = reinterpret_cast<typename std::add_pointer<T>::type>(info.ptr);
    auto ptr_end = ptr + info.size;
    while (ptr != ptr_end) {
        std::array<T, N> data{};
        for (auto i = 0u; i < N; ++i) {
            data[i] = ptr[i];
        }
        vector.push_back(data);
        std::advance(ptr, N);
    }
}

/**
 * \}
 */

} // namespace basalt

namespace std {

/**
 * New function overload to write a standard vector to a stream
 * \tparam T array value type
 * \tparam N array size
 * \param ostr stream to write data to
 * \param array array to read data from
 * \return stream
 */
template <typename T, size_t N>
ostream& operator<<(ostream& ostr, const array<T, N>& array) {
    for (auto i = 0u; i < N; ++i) {
        ostr << array[i] << ' ';
    }
    return ostr;
}

/**
 * New function overload to fill a standard array from a stream
 * \tparam T array value type
 * \tparam N array size
 * \param istr stream to read from
 * \param array array
 * \return stream
 */
template <typename T, std::size_t N>
std::istream& operator>>(std::istream& istr, std::array<T, N>& array) {
    for (auto i = 0u; i < N; ++i) {
        istr >> array[i];
    }
    return istr;
}

} // namespace std

#endif // BASALT_PY_HELPERS_HPP
