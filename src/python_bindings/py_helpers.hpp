/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#pragma once

/**
 * \file src/basalt/py_helpers.hpp
 * Provides utility functions to Python/C++ object conversions
 */

#include <array>
#include <ostream>
#include <sstream>

#include <cereal/archives/binary.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include <pybind11/numpy.h>

#include <basalt/settings.hpp>

namespace basalt {

/**
 * \name Serialization helpers
 * \{
 */

struct membuf: std::streambuf {
    membuf(pybind11::array_t<char>& data);
    membuf(char* base, size_t size);
    virtual ~membuf();
};

struct imemstream: virtual membuf, std::istream {
    imemstream(char* base, size_t size);
    imemstream(pybind11::array_t<char>& data);
    virtual ~imemstream();
};

/**
 * Create a string with hexadecimal values of a py_array
 * \param array
 * \return hexadecimal string
static
std::string py_array_to_hexadecimal(pybind11::array_t<char> &array) {
    std::string output;
    static const char* const lut = "0123456789ABCDEF";
    auto request = array.request();
    auto input = reinterpret_cast<char*>(request.ptr);
    const auto len = request.size;

    output += std::to_string(len) + ' ';
    output.reserve(2 * len);
    for (auto i = 0; i < len; ++i) {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}
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
    std::memcpy(buffer.ptr, data.data(), data.size());
    return result;
}

/**
 * Write content of a string to a NumPy array
 * \param str string to copy
 * \return new NumPY array
 */

inline pybind11::array_t<char> to_py_array(const std::string& str) {
    auto array = pybind11::array_t<char>(str.size());
    auto buffer = array.request(true);
    std::memcpy(buffer.ptr, str.data(), str.size());
    return array;
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
    for (const auto idx: data) {
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
void deserialize_vector(std::istream& iss, std::vector<T>& data) {
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
    vector.reserve(static_cast<std::size_t>(buffer.size));
    auto ptr = reinterpret_cast<typename std::add_pointer<T>::type>(buffer.ptr);
    std::copy(ptr, ptr + buffer.size, std::back_inserter(vector));
}

/**
 * Fill a standard vector of \a std::array from a NumPy array
 */
template <typename T, std::size_t N>
void fill_vector(pybind11::buffer& buffer, std::vector<std::array<T, N>>& vector) {
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
    if (info.shape[1] != static_cast<ssize_t>(N)) {
        std::ostringstream oss;
        oss << "Invalid dimension 1 size. Expected " << N << " but got " << info.shape[1];
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

namespace cereal {

template <typename Payload>
pybind11::array_t<char> serialize(const Payload& p) {
    std::ostringstream oss(std::stringstream::out | std::stringstream::binary);
    {
        ::cereal::BinaryOutputArchive archive(oss);
        archive(p);
    }
    return to_py_array(oss);
}

template <typename Payload>
void deserialize(::pybind11::array_t<char>& data, Payload& p) {
    imemstream iss(data);
    ::cereal::BinaryInputArchive archive(iss);
    archive(p);
}

}  // namespace cereal

template <typename Payload>
pybind11::array_t<char> serialize_impl(const Payload& payload, int method) {
    if (method == BASALT_CEREAL_SERIALIZATION) {
        return cereal::serialize(payload);
    } else if (method == BASALT_SSTREAM_SERIALIZATION) {
        return payload.serialize_sstream();
    } else {
        throw std::runtime_error("Unknown serialization method");
    }
}

template <typename Payload>
pybind11::array_t<char> serialize(const Payload& payload) {
    return serialize_impl(payload, BASALT_SERIALIZATION);
}

template <typename Payload>
void deserialize_impl(Payload& payload, pybind11::array_t<char>& data, int method) {
    if (method == BASALT_CEREAL_SERIALIZATION) {
        cereal::deserialize(data, payload);
    } else if (method == BASALT_SSTREAM_SERIALIZATION) {
        payload.deserialize_sstream(data);
    } else {
        throw std::runtime_error("Unknown serialization method");
    }
}

template <typename Payload>
void deserialize(Payload& payload, pybind11::array_t<char>& data) {
    deserialize_impl(payload, data, BASALT_SERIALIZATION);
}

/**
 * \}
 */

}  // namespace basalt

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

}  // namespace std
