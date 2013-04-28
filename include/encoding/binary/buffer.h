// -*- c++ -*-

// Copyright (c) 2013, Roman Kashitsyn
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of the copyright holder nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef ENCODING_BINARY_BUFFER_H_
#define ENCODING_BINARY_BUFFER_H_

#include <stdint.h>
#include <stdexcept>
#include "encoding/binary/buf_fwd.h"

/**
 * @file
 * @brief Implementation of binary buffers and encoding strategies.
 */
namespace encoding { namespace binary {

namespace {

const char * const OutOfRangeMessage = "Buffer out of bounds";
std::out_of_range Overflow(OutOfRangeMessage);

template <
    typename ByteOrder,
    typename ValueType
>
std::size_t get_from(const uint8_t *src, ValueType &dst, const read_access_tag&) {
    ByteOrder::decode(src, dst);
    return sizeof(dst);
}


template <
    typename ByteOrder,
    typename ValueType
>
std::size_t put_into(ValueType src, uint8_t *dst, const write_access_tag&) {
    ByteOrder::encode(src, dst);
    return sizeof(src);
}

}

/**
 * @defgroup ByteOrder encoding strategies implementations.
 * @{
 */

/**
 * @brief Base for endian implementations. Contains no-op conversions
 * which are useful in templates.
 */
struct endian_base {
    static void encode(uint8_t val, uint8_t *buf) {
        *buf = val;
    }
    static void decode(const uint8_t *buf, uint8_t &val) {
        val = *buf;
    }
};

/**
 * @brief Implementation of little-endian encoding routines.
 */
struct little_endian : public endian_base {
    using endian_base::encode;
    using endian_base::decode;

    static void encode(uint16_t val, uint8_t *buf) {
        buf[0] = uint8_t(val);
        buf[1] = uint8_t(val >> 8);
    }
    static void decode(const uint8_t *buf, uint16_t &val) {
        val = uint16_t(buf[1] << 8) | buf[0];
    }
    static void encode(uint32_t val, uint8_t *buf) {
        buf[0] = uint8_t(val);
        buf[1] = uint8_t(val >> 8);
        buf[2] = uint8_t(val >> 16);
        buf[3] = uint8_t(val >> 24);
    }
    static void decode(const uint8_t *buf, uint32_t &val) {
        val = uint32_t(buf[3]) << 24 |
            uint32_t(buf[2]) << 16 |
            uint32_t(buf[1]) <<  8 |
            buf[0];
    }
    static void encode(uint64_t val, uint8_t *buf) {
        buf[0] = uint8_t(val);
        buf[1] = uint8_t(val >>  8);
        buf[2] = uint8_t(val >> 16);
        buf[3] = uint8_t(val >> 24);
        buf[4] = uint8_t(val >> 32);
        buf[5] = uint8_t(val >> 40);
        buf[6] = uint8_t(val >> 48);
        buf[7] = uint8_t(val >> 56);
    }
    static void decode(const uint8_t *buf, uint64_t &val) {
        val = uint64_t(buf[7]) << 56 |
            uint64_t(buf[6]) << 48 |
            uint64_t(buf[5]) << 40 |
            uint64_t(buf[4]) << 32 |
            uint64_t(buf[3]) << 24 |
            uint64_t(buf[2]) << 16 |
            uint64_t(buf[1]) <<  8 |
            buf[0];
    }
};

/**
 * @brief Implementation of big-endian encoding routines.
 */
struct big_endian : public endian_base {
    using endian_base::encode;
    using endian_base::decode;

    static void encode(uint16_t val, uint8_t *buf) {
        buf[0] = uint8_t(val >> 8);
        buf[1] = uint8_t(val);
    }
    static void decode(const uint8_t *buf, uint16_t &val) {
        val = uint16_t(buf[0] << 8) | buf[1];
    }
    static void encode(uint32_t val, uint8_t *buf) {
        buf[0] = uint8_t(val >> 24);
        buf[1] = uint8_t(val >> 16);
        buf[2] = uint8_t(val >> 8);
        buf[3] = uint8_t(val);
    }
    static void decode(const uint8_t *buf, uint32_t &val) {
        val = uint32_t(buf[0]) << 24 |
            uint32_t(buf[1]) << 16 |
            uint32_t(buf[2]) << 8 |
            buf[3];
    }
    static void encode(uint64_t val, uint8_t *buf) {
        buf[0] = uint8_t(val >> 56);
        buf[1] = uint8_t(val >> 48);
        buf[2] = uint8_t(val >> 40);
        buf[3] = uint8_t(val >> 32);
        buf[4] = uint8_t(val >> 24);
        buf[5] = uint8_t(val >> 16);
        buf[6] = uint8_t(val >> 8);
        buf[7] = uint8_t(val);
    }
    static void decode(const uint8_t *buf, uint64_t &val) {
        val = uint64_t(buf[0]) << 56 |
            uint64_t(buf[1]) << 48 |
            uint64_t(buf[2]) << 40 |
            uint64_t(buf[3]) << 32 |
            uint64_t(buf[4]) << 24 |
            uint64_t(buf[5]) << 16 |
            uint64_t(buf[6]) << 8 |
            buf[7];
    }
};

/**
 * @brief Implementation of native encoding routines. This
 * implementation usually the fastest one, but it's not portable.
 */
struct native_endian : public endian_base {
    using endian_base::encode;
    using endian_base::decode;

    static void encode(uint16_t val, uint8_t *buf) {
        *reinterpret_cast<uint16_t*>(buf) = val;
    }
    static void decode(const uint8_t *buf, uint16_t &val) {
        val = *reinterpret_cast<const uint16_t*>(buf);
    }
    static void encode(uint32_t val, uint8_t *buf) {
        *reinterpret_cast<uint32_t*>(buf) = val;
    }
    static void decode(const uint8_t *buf, uint32_t &val) {
        val = *reinterpret_cast<const uint32_t*>(buf);
    }
    static void encode(uint64_t val, uint8_t *buf) {
        *reinterpret_cast<uint64_t*>(buf) = val;
    }
    static void decode(const uint8_t *buf, uint64_t &val) {
        val = *reinterpret_cast<const uint64_t*>(buf);
    }
};

/** @} */

/**
 * @defgroup ACTraits Access Control Traits
 * @{
 */

template <typename AccessTag>
struct access_traits {
    typedef uint8_t value_type;
    typedef uint8_t strict_type;
    typedef uint8_t *iterator;
    typedef const uint8_t *const_iterator;
};

template <>
struct access_traits<read_access_tag> {
    typedef uint8_t value_type;
    typedef const uint8_t strict_type;
    typedef const uint8_t *iterator;
    typedef const uint8_t *const_iterator;
};

/** @} */

/**
 * @brief Implementation of bound binary buffer.
 *
 * Invariant: `begin <= pos <= end`.
 *
 * @tparam ByteOrder byte order used for encoding
 * @tparam AccessTag type tag that restricts operationr on buffer
 */
template <
    typename ByteOrder = default_byte_order,
    typename AccessTag = read_write_access_tag
    >
class basic_buffer {
public:
    typedef ByteOrder byte_order;
    typedef AccessTag access_tag;
    typedef typename access_traits<access_tag>::value_type value_type;
    typedef typename access_traits<access_tag>::strict_type strict_type;
    typedef typename access_traits<access_tag>::iterator iterator;
    typedef typename access_traits<access_tag>::const_iterator const_iterator;

    basic_buffer(iterator begin, iterator end)
        : begin_(begin)
        , end_(end)
        , pos_(begin)
    {}

    template <std::size_t Size>
    basic_buffer(strict_type (&buf)[Size])
        : begin_(buf)
        , end_(buf + Size)
        , pos_(buf)
    {}

    /**
     * @brief Returns biginnig of a buffer.
     */
    const_iterator begin() const { return begin_; }

    /**
     * @brief Returns end of a buffer.
     */
    const_iterator end() const { return end_; }

    /**
     * @brief Returns position of a buffer.
     */
    const_iterator pos() const { return pos_; }

    /**
     * @brief Returns total size of buffer in bytes.
     */
    std::size_t size() const { return end_ - begin_; }

    /**
     * @brief Returns number of not consumed/written bytes from input
     * requence.
     */
    std::size_t bytes_left() const { return end_ - pos_; }

    /**
     * @brief Resets buffer position to begin.
     * @return current buffer
     */
    basic_buffer & reset() {
        pos_ = begin_;
        return *this;
    }

    /**
     * @brief Puts a value into a buffer.
     * @tparam T value type
     * @param value value to put
     * @return current buffer
     */
    template <typename T>
    basic_buffer & put(T value) {
        if (bytes_left() < sizeof(value)) throw Overflow;
        pos_ += put_into<byte_order>(value, pos_, access_tag());
        return *this;
    }

    /**
     * @brief Copies sequence of bytes into a buffer.
     * @param from input sequence begin
     * @param to input sequence end
     * @retun current buffer
     */
    basic_buffer & put(const_iterator from, const const_iterator to) {
        for (; from != to && pos_ != end_; ++pos_, ++from) {
            *pos_ = *from;
        }
        if (from != to) throw Overflow;
        return *this;
    }

    /**
     * @brief Retrieves a value from a buffer.
     * @tparam T value type
     * @param value reference to value
     * @return current buffer
     */
    template <typename T>
    basic_buffer & get(T &value) {
        if (bytes_left() < sizeof(value)) throw Overflow;
        pos_ += get_from<byte_order>(pos_, value, access_tag());
        return *this;
    }

    /**
     * @brief Skips `count` bytes from input sequence by moving buffer
     * position forward.
     *
     * @param count number of bytes to skip
     * @return current buffer
     */
    basic_buffer & skip(std::size_t count) {
        if (bytes_left() < count) throw Overflow;
        pos_ += count;
        return *this;
    }

private:
    const iterator begin_;     // not const_iterator to allow assignment `pos_ = begin_`
    const const_iterator end_;
    iterator pos_;
};

/**
 * @defgroup OverloadedOperators Handy overloaded operators
 * @{
 */

template <
    typename ByteOrder,
    typename AccessTag,
    typename T>
basic_buffer<ByteOrder, AccessTag> & operator>>(basic_buffer<ByteOrder, AccessTag> &buf,
                                                T &value) {
    return buf.get(value);
}

template <
    typename ByteOrder,
    typename AccessTag,
    typename T
    >
basic_buffer<ByteOrder, AccessTag> & operator<<(basic_buffer<ByteOrder, AccessTag> &buf,
                                                const T &value) {
    return buf.put(value);
}

/** @} */

/**
 * @defgroup AuxFuncs Various axiliary functions.
 * @{
 */

template <
    typename T,
    typename BufferType
>
T get(BufferType &buf) {
    T tmp;
    buf.get(tmp);
    return tmp;
}

/** @} */

} }

#endif /* ENCODING_BINARY_BUFFER_H_ */
