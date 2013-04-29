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

#ifndef ENCODING_BINARY_BUF_FWD_H_
#define ENCODING_BINARY_BUF_FWD_H_

/**
 * @file
 * @brief Forward declarations for binary buffers templates.
 */
namespace encoding { namespace binary {

template <
    typename ByteOrder,
    typename AccessTag
    >
class basic_buffer;

struct big_endian;
struct little_endian;
struct native_endian;

struct read_access_tag {
    static const bool readable = true;
    static const bool writable = false;
};
struct write_access_tag {
    static const bool readable = false;
    static const bool writable = true;
};
struct read_write_access_tag : read_access_tag, write_access_tag {
    using read_access_tag::readable;
    using write_access_tag::writable;
};

typedef big_endian default_byte_order;

typedef basic_buffer<default_byte_order, read_write_access_tag> buffer;
typedef basic_buffer<default_byte_order, read_access_tag> readonly_buffer;
typedef basic_buffer<default_byte_order, write_access_tag> writeonly_buffer;

typedef basic_buffer<little_endian, read_write_access_tag> le_buffer;
typedef basic_buffer<little_endian, read_access_tag> le_readonly_buffer;
typedef basic_buffer<little_endian, write_access_tag> le_writeonly_buffer;

} }

#endif /* ENCODING_BINARY_BUF_FWD_H_ */
