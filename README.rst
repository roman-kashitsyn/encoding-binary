Encoding.Binary
===============

A simple cross-platform header-only library for binary
encoding/decoding.

Concepts
--------

The main abstraction of the library is the ``buffer`` concept. It
allows to specify required encoding byte order (big-endian is used by
default) and maximum length. There are several kinds of buffers (some
of them are typedefed from a single ``basic_buffer`` class template)
for fine-grained access control:

* ``readonly_buffer`` allows `get` modificators (it works with both
  mutable and immutable input sequences);
* ``writeonly_buffer`` allows `put` modificators (requires mutable
  output sequence);
* ``buffer`` allows both modifications (requires mutable input/output
  sequence).

Static Buffers
--------------

Exact lengths of a buffer and it's components are often known at
compile time. Such knowledge allows us to find overflow errors *at
compile time* and to provide *strong exception
guarantee*. ``basic_static_buffer`` designed specially for such
cases. It's also requeres some additional template instantiation and
slightly increases compile-time.

Examples
--------

* Writing fixed sequence of binary values and reading it back:

  .. code:: c++

    #include <encoding/binary/buffer.h>

    namespace bin = encoding::binary;

    struct file_header
    {
      uint16_t magic_number;
      uint8_t  major_version;
      uint8_t  minor_version;
      uint32_t num_entries;
    };

    const std::size_t HeaderSizeInBytes = 8;

    // Writing header into a buffer
    void encode(const file_header &header, uint8_t *dst)
    {
      bin::writeonly_static_buffer<HeaderSizeInBytes> buf(dst);
      buf.put(header.magic_number)
         .put(header.major_version)
         .put(header.minor_version)
         .put(header.num_entries);  // adding one extra `put` won't compile
    }

    // Reading header from a buffer
    void decode(const uint8_t *src, const file_header &header)
    {
      // input sequence is const so we have to use readonly_buffer here
      bin::readonly_static_buffer<HeaderSizeInBytes> buf(src);
      buf.get(header.magic_number)
         .get(header.major_version)
         .get(header.minor_version)
         .get(header.num_entries);  // adding one extra `get` won't compile
    }

* Writing sequece of bytes depending on number specified at runtime:

  .. code:: c++

    #include <encoding/binary/buffer.h>

    namespace bin = encoding::binary;
    const std::size_t MaxChunkLength = 1 << 30;

    struct chunk
    {
      uint32_t length;
      uint8_t  *data;
      uint32_t check_sum;
    };

    std::size_t bytes_for_chunk(chunk const &c)
    {
      return c.length + sizeof(c.length) + sizeof(c.check_sum);
    }   

    // Writing data chunk into buffer
    void write_chunk(chunk const &c, uint8_t *dst)
    {
      bin::writeonly_buffer buf(dst, bytes_for_chunk(c));
      buf.put(c.length)
         .put(c.data, c.length)
	 .put(c.check_sum)
    }

    void read_chunk(const uint8_t *src, chunk &dst)
    {
      try {
      bin::readonly_buffer buf(src, MaxChunkLength);
      buf.get(dst.length)
         .get(dst.data, dst.length)
	 .get(dst.check_sum);
      } catch (std::out_of_range &e) {
        // handle bug here
      }
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
