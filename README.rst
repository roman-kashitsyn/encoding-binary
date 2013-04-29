Encoding.Binary
===============

A simple cross-platform header-only library for binary
encoding/decoding.

Basic Usage
-----------

The main abstraction of the library is the ``basic_buffer`` class
template that allows to specify required encoding byte order
(big-endian is used by default). There are several kinds of buffers
(they are all typedefed from a single ``basic_buffer`` class template)
for fine-grained access control:

* ``readonly_buffer`` allows `get` modificators (it works with both
  mutable and immutable input sequences);
* ``writeonly_buffer`` allows `put` modificators (requires mutable
  output sequence);
* ``buffer`` allows both modifications (requires mutable input/output
  sequence).

Examples
--------

* Writing binary values and reading them back:

  .. code:: c++

    #include <encoding/binary/buffer.h>

    namespace bin = encoding::binary;

    struct file_header {
      uint16_t magic_number;
      uint8_t  major_version;
      uint8_t  minor_version;
      uint32_t num_entries;
    };

    const std::size_t HeaderSizeInBytes = 8;

    // Reading header from a buffer
    void encode(const file_header &header, uint8_t *dst) {
      bin::writeonly_buffer buf(dst, dst + HeaderSizeInBytes);
      buf << header.magic_number
          << header.major_version
	  << header.minor_version
	  << header.num_entries;
      // or
      // buf.put(header.magic_number)
      //    .put(header.major_version)
      //    .put(header.minor_version)
      //    .put(header.num_entries);
    }

    void decode(const uint8_t *src, const file_header &header) {
      // input sequence is const so we have to use readonly_buffer here
      bin::readonly_buffer buf(src, src + HeaderSizeInBytes);
      buf >> header.magic_number
          >> header.major_version
	  >> header.minor_version
	  >> header.num_entries;
      // or
      // buf.get(header.magic_number)
      //    .get(header.major_version)
      //    .get(header.minor_version)
      //    .get(header.num_entries);
    }


* Using forward declaration for shorter compile time:

  .. code:: c++

    #include <encoding/binary/buf_fwd.h>
    namespace bin = encoding::binary;

    // Your function prototypes which don't require full buffer definition
    bin::buffer & my_encode(bin::buffer &buf);
    bin::readonly_buffer & my_decode(bin::readonly_buffer &buf);


Please find more examples at ``test`` folder.

Bugs
----
Please report found bugs to roman.kashitsyn at gmail.com.
