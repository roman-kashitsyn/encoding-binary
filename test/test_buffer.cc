#include "gtest/gtest.h"
#include "encoding/binary/buffer.h"
#include <cstring>
#include <stdexcept>

namespace bin = encoding::binary;
using bin::get;

TEST(Buffer, check_invariants) {
    uint8_t cbuf[13];
    bin::buffer buf(cbuf);
    ASSERT_EQ(sizeof(cbuf), buf.bytes_left());
    for (uint16_t i = 0; i < sizeof(cbuf) / sizeof(uint16_t); ++i) {
        ASSERT_TRUE(buf.begin() <= buf.pos() && buf.pos() <= buf.end());
        buf.put(i);
        ASSERT_EQ(buf.bytes_left(), buf.size() - sizeof(i) * (i + 1));
    }
    buf.reset();
    for (uint16_t i = 0; i < sizeof(cbuf) / sizeof(uint16_t); ++i) {
        ASSERT_TRUE(buf.begin() <= buf.pos() && buf.pos() <= buf.end());
        ASSERT_EQ(i, get<uint16_t>(buf));
        ASSERT_EQ(buf.bytes_left(), buf.size() - sizeof(i) * (i + 1));
    }
}

TEST(Buffer, writing_bytes_in_big_endian) {
    uint8_t cbuf[4];
    const uint8_t Expected[] = {0xa, 0xb, 0xc, 0xd};
    bin::buffer buf(cbuf);
    buf.put(uint32_t(0x0a0b0c0d));
    ASSERT_EQ(0, std::memcmp(Expected, cbuf, 4));
}

TEST(Buffer, writing_different_types_in_big_endian) {
    uint32_t head = 0x01020304;
    const uint8_t middle[] = {5, 6, 7, 8};
    uint16_t tail = 0x090a;

    const std::size_t Total = sizeof(head) + sizeof(middle) + sizeof(tail);
    uint8_t cbuf[Total];
    const uint8_t Expected[] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa};

    bin::buffer buf(cbuf);
    buf.put(head).put(middle, middle + sizeof(middle)).put(tail);

    ASSERT_EQ(buf.end(), buf.pos());
    ASSERT_EQ(0, std::memcmp(Expected, cbuf, Total));

    bin::readonly_buffer rd_buf(cbuf);
    ASSERT_EQ(head, get<uint32_t>(rd_buf));
    rd_buf.skip(sizeof(middle));
    ASSERT_EQ(tail, get<uint16_t>(rd_buf));
    ASSERT_EQ(0x0102030405060708u, get<uint64_t>(rd_buf.reset()));
}

TEST(Buffer, writing_different_types_in_little_endian) {
    uint32_t head = 0x01020304;
    const uint8_t middle[] = {5, 6, 7, 8};
    uint16_t tail = 0x090a;

    const std::size_t Total = sizeof(head) + sizeof(middle) + sizeof(tail);
    uint8_t cbuf[Total];
    const uint8_t Expected[] = {0x4, 0x3, 0x2, 0x1, 0x5, 0x6, 0x7, 0x8, 0xa, 0x9};

    bin::le_buffer buf(cbuf);
    buf.put(head).put(middle, middle + sizeof(middle)).put(tail);

    ASSERT_EQ(0, std::memcmp(Expected, cbuf, Total));

    bin::le_readonly_buffer rd_buf(cbuf);
    ASSERT_EQ(head, get<uint32_t>(rd_buf));
    rd_buf.skip(sizeof(middle));
    ASSERT_EQ(tail, get<uint16_t>(rd_buf));
    ASSERT_EQ(0x0807060501020304u, get<uint64_t>(rd_buf.reset()));
}

TEST(Buffer, overloaded_operators) {
    uint16_t x;
    uint32_t y;
    uint64_t z;
    uint8_t cbuf[sizeof(x) + sizeof(y) + sizeof(z)];
    bin::buffer buf(cbuf);
    buf << uint16_t(1) << uint32_t(2) << uint64_t(3);
    buf.reset();
    buf >> x >> y >> z;

    ASSERT_EQ(1u, x);
    ASSERT_EQ(2u, y);
    ASSERT_EQ(3u, z);
}

TEST(ReadOnlyBuffer, allows_to_traverse_array) {
    const uint8_t  bytes[] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6};
    const uint16_t nums[]  = {0x0102, 0x0304, 0x0506};
    bin::readonly_buffer rd_buf(bytes);

    for (std::size_t i = 0; i < sizeof(bytes); ++i) {
        ASSERT_EQ(bytes[i], get<uint8_t>(rd_buf));
    }

    rd_buf.reset();

    for (std::size_t i = 0; i < sizeof(nums) / sizeof(nums[0]); ++i) {
        ASSERT_EQ(nums[i], get<uint16_t>(rd_buf));
    }
}

TEST(ReadOnlyBuffer, throws_out_of_range_on_overflow) {
    const uint8_t bytes[] = {0x1, 0x2, 0x3, 0x4};
    bin::readonly_buffer buf(bytes);
    (void)get<uint32_t>(buf);
    ASSERT_THROW(get<uint8_t>(buf), std::out_of_range);
}
