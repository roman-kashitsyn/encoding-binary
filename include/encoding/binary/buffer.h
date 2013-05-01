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
#include <cstring>
#include <stdexcept>
#include "encoding/binary/buf_fwd.h"

/**
 * @file
 * @brief Implementation of binary buffers and encoding strategies.
 * @author Roman Kashitsyn
 */
namespace encoding { namespace binary {

namespace {

const char * const OutOfRangeMessage = "Buffer out of bounds";
std::out_of_range Overflow(OutOfRangeMessage);

}

namespace details {

template <typename AccessTag>
void assert_access(AccessTag) {}

template<bool Cond, class T = void> struct enable_if {};
template<class T> struct enable_if<true, T> {
    typedef T type;
};

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
 *     begin               pos                   end
 *       V                  V                     V
 *       |------------------|--------------------|
 *
 * Invariant: `begin <= pos <= end`.
 *
 * @tparam ByteOrder byte order used for encoding
 * @tparam AccessTag type tag that restricts operations on buffer
 */
template <
    typename ByteOrder = default_byte_order,
    typename AccessTag = read_write_access_tag
    >
class basic_buffer
{
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

    basic_buffer(iterator begin, std::size_t length)
        : begin_(begin)
        , end_(begin + length)
        , pos_(begin)
    {}

    /**
     * @brief Returns beginning of a buffer.
     */
    const_iterator begin() const { return begin_; }
    iterator begin() { return begin_; }

    /**
     * @brief Returns end of a buffer.
     */
    const_iterator end() const { return end_; }

    /**
     * @brief Returns position of a buffer.
     */
    const_iterator pos() const { return pos_; }
    iterator pos() { return pos_; }

    /**
     * @brief Returns total size of buffer in bytes.
     */
    std::size_t size() const { return end() - begin(); }

    /**
     * @brief Returns number of not consumed/written bytes from input
     * sequence.
     */
    std::size_t bytes_left() const { return end() - pos(); }

    /**
     * @brief Resets buffer position to begin.
     * @return current buffer
     */
    basic_buffer & reset()
    {
        pos_ = begin();
        return *this;
    }

    /**
     * @brief Puts a value into a buffer.
     * @tparam T value type
     * @param value value to put
     * @return current buffer
     */
    template <typename T>
    basic_buffer & put(T value)
    {
        details::assert_access<write_access_tag>(access_tag());
        if (bytes_left() < sizeof(value)) throw Overflow;
        byte_order::encode(value, pos());
        pos_ += sizeof(value);
        return *this;
    }

    /**
     * @brief Copies sequence of bytes into a buffer.
     * @param from input sequence begin
     * @param length length of the input sequence
     * @return current buffer
     */
    basic_buffer & put(const value_type *from, std::size_t length)
    {
        details::assert_access<write_access_tag>(access_tag());
        const const_iterator to = from + length;
        for (; from != to && pos_ != end_; ++pos_, ++from) {
            *pos_ = *from;
        }
        if (from != to) throw Overflow;
        return *this;
    }

    /**
     * @brief Reads a value from a buffer.
     * @tparam T value type
     * @param value reference to value
     * @return current buffer
     */
    template <typename T>
    basic_buffer & get(T &value)
    {
        details::assert_access<read_access_tag>(access_tag());
        if (bytes_left() < sizeof(value)) throw Overflow;
        byte_order::decode(pos(), value);
        pos_ += sizeof(value);
        return *this;
    }

    /**
     * @brief Reads a sequence of bytes from a buffer.
     * @param dst destination byte sequence
     * @param length length of the output sequence
     * @return current buffer
     */
    basic_buffer & get(value_type *dst, std::size_t length)
    {
        details::assert_access<read_access_tag>(access_tag());
        if (bytes_left() < length) throw Overflow;
        std::memcpy(dst, pos_, length);
        pos_ += length;
        return *this;
    }

    /**
     * @brief Skips `count` bytes from input sequence by moving buffer
     * position forward.
     *
     * @param count number of bytes to skip
     * @return current buffer
     */
    basic_buffer & skip(std::size_t count)
    {
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
 * @brief Implementation of binary buffer with compile-time bounds
 * checking.
 *
 * The buffer itself is immutable since all position state is held in
 * numeric template parameters. It means that each modifier returns
 * newly constructed buffer of different type (concrete type will be
 * computed during the compilation). The most convenient way of work
 * is just to chain modifiers. That allows the compiler to compute
 * offsets and to do all the boring plumbing. Buffer copy is very
 * cheap since almost all the state is held in the buffer type.
 *
 * @note Synopsis is close to synopsis of `basic_buffer` but sometimes
 * it *has to be* different to allow compile-time offset computation.
 * For example, `skip` and `put(const_iterator)` became templates with
 * numeric parameter.
 *
 * @note Since all the checks are performed at compile time, all
 * `basic_static_buffer` methods provide *strong exception guarantee*.
 */
template <
    class ByteOrder,
    typename AccessTag,
    std::size_t Size,
    std::size_t Offset
>
class basic_static_buffer
{
public:
    typedef ByteOrder byte_order;
    typedef AccessTag access_tag;
    typedef basic_static_buffer<byte_order, access_tag, 0u, Size> buffer_beginning;
    typedef typename access_traits<access_tag>::value_type value_type;
    typedef typename access_traits<access_tag>::iterator iterator;
    typedef typename access_traits<access_tag>::const_iterator const_iterator;

    basic_static_buffer(iterator begin)
        : begin_(begin)
    {}

    /**
     * @brief Returns pointer to first element of the buffer.
     */
    const_iterator begin() const { return begin_; }
    iterator begin() { return begin_; }

    /**
     * @brief Returns end of a buffer (pointer to element after the
     * last one).
     */
    const_iterator end() const { return begin_ + Size; }

    /**
     * @brief Returns current position in a buffer.
     */
    const_iterator pos() const { return begin_ + Offset; }
    iterator pos() { return begin_ + Offset; }

    /**
     * @brief Returns size of a buffer.
     */
    std::size_t size() const { return Size; }

    /**
     * @brief Returns number bytes left to the end, synonym to `end() - pos()`.
     */
    std::size_t bytes_left() const { return Size - Offset; }

    /**
     * @brief Reads a value from a buffer.
     * @tparam T value type
     * @return new buffer with updated offset
     */
    template <typename T>
    typename details::enable_if<
        sizeof(T) <= (Size - Offset),
        basic_static_buffer<byte_order, access_tag, Size, Offset + sizeof(T)>
        >::type get(T &value)
    {
        details::assert_access<read_access_tag>(access_tag());
        byte_order::decode(begin_ + Offset, value);
        return basic_static_buffer<byte_order, access_tag, Size, Offset + sizeof(T)>(begin_);
    }

     /**
     * @brief Reads a sequence of bytes from a buffer.
     * @tparam Length length of the output sequence
     * @return new buffer with updated offset
     */
    template <std::size_t Length>
    typename details::enable_if<
        Length <= (Size - Offset),
        basic_static_buffer<byte_order, access_tag, Size, Offset + Length>
        >::type get(value_type *dst)
    {
        details::assert_access<read_access_tag>(access_tag());
        std::memcpy(dst, pos(), Length);
        return basic_static_buffer<byte_order, access_tag, Size, Offset + Length>(begin());
    }

    /**
     * @brief Writes a value into a buffer.
     * @tparam T value type
     * @param value value to put
     * @return new buffer with updated offset
     */
    template <typename T>
    typename details::enable_if<
        sizeof(T) <= (Size - Offset),
        basic_static_buffer<byte_order, access_tag, Size, Offset + sizeof(T)>
        >::type put(T value)
    {
        details::assert_access<write_access_tag>(access_tag());
        byte_order::encode(value, pos());
        return basic_static_buffer<byte_order, access_tag, Size, Offset + sizeof(T)>(begin());
    }

    /**
     * @brief Writes bytes into a buffer.
     * @tparam Length length of the byte sequence
     * @return new buffer with updated offset
     */
    template <std::size_t Length>
    typename details::enable_if<
        Length <= (Size - Offset),
        basic_static_buffer<byte_order, access_tag, Size, Offset + Length>
        >::type put(const_iterator bytes)
    {
        details::assert_access<write_access_tag>(access_tag());
        std::memcpy(pos(), bytes, Length);
        return basic_static_buffer<byte_order, access_tag, Size, Offset + Length>(begin());
    }

    /**
     * @brief Skips given bytes count from input sequence.
     * @tparam SkipBytes number of bytes to skip
     * @return new buffer with updated offset
     */
    template <std::size_t SkipBytes>
    typename details::enable_if<
        (Offset + SkipBytes <= Size),
        basic_static_buffer<byte_order, access_tag, Size, Offset + SkipBytes>
        >::type skip() const
    {
        return basic_static_buffer<byte_order, access_tag, Size, Offset + SkipBytes>(begin());
    }

    /**
     * @brief Returns buffer with zero offset.
     * @return new buffer with zero offset
     */
    buffer_beginning reset() const {
        return buffer_beginning(begin());
    }

private:
    const iterator begin_;
};

/**
 * @brief Readable and writable static buffer template with big-endian
 * encoding.
 */
template <std::size_t Size, std::size_t Offset = 0>
class static_buffer :
    public basic_static_buffer<default_byte_order, read_write_access_tag, Size, Offset>
{
public:
    typedef basic_static_buffer<default_byte_order, read_write_access_tag, Size, Offset> base;
    typedef typename base::iterator iterator;

    static_buffer(base const &b): base(b)
    {}

    static_buffer(iterator begin): base(begin)
    {}
};

/**
 * @brief Read-only static buffer template.
 */
template <std::size_t Size, std::size_t Offset = 0>
class readonly_static_buffer :
    public basic_static_buffer<default_byte_order, read_access_tag, Size, Offset>
{
public:
    typedef basic_static_buffer<default_byte_order, read_access_tag, Size, Offset> base;
    typedef typename base::iterator iterator;

    readonly_static_buffer(base const &b): base(b)
    {}

    readonly_static_buffer(iterator begin): base(begin)
    {}
};

/**
 * @brief Write-only static buffer template.
 */
template <std::size_t Size, std::size_t Offset = 0>
class writeonly_static_buffer :
    public basic_static_buffer<default_byte_order, write_access_tag, Size, Offset>
{
public:
    typedef basic_static_buffer<default_byte_order, write_access_tag, Size, Offset> base;
    typedef typename base::iterator iterator;

    writeonly_static_buffer(base const &b): base(b)
    {}

    writeonly_static_buffer(iterator begin): base(begin)
    {}
};

/**
 * @defgroup OverloadedOperators Handy overloaded operators.
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
 * @defgroup Aux Various auxiliary functions and templates.
 * @{
 */

template <
    typename T,
    class BufferType
>
T get(BufferType &buf)
{
    T tmp;
    buf.get(tmp);
    return tmp;
}

/**
 * @brief Compile-time function to check if it possible to read from a
 * buffer of specified type.
 */
template <class BufferType>
struct is_readable
{
    typedef typename BufferType::access_tag access_tag;
    static const bool value = access_tag::readable;
};

/**
 * @brief Compile-time function to check if it possible to write into a
 * buffer of specified type.
 */
template <class BufferType>
struct is_writable
{
    typedef typename BufferType::access_tag access_tag;
    static const bool value = access_tag::writable;
};

/** @} */

} }

#endif /* ENCODING_BINARY_BUFFER_H_ */
