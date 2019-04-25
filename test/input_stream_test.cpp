#include <gtest/gtest.h>
#include <net/base/stream.hpp>

TEST(INPUT_STREAM_TEST, BaseTest) 
{
    std::vector<char> data = {100, 0, 0, 0};
    pan::net::istream is(data.data(), data.size());
    EXPECT_EQ(is.size(), 0);
    EXPECT_EQ(is.capacity(), 4);
    EXPECT_EQ(is.data()[0], 100);
    EXPECT_EQ(is.data()[1], 0);
    EXPECT_EQ(is.data()[2], 0);
    EXPECT_EQ(is.data()[3], 0);
    pan::net::bit::istream bis(data.data(), data.size());
    EXPECT_EQ(bis.size(), 0);
    EXPECT_EQ(bis.capacity(), 4);
    EXPECT_EQ(bis.data()[0], 100);
    EXPECT_EQ(bis.data()[1], 0);
    EXPECT_EQ(bis.data()[2], 0);
    EXPECT_EQ(bis.data()[3], 0);
}

TEST(INPUT_STREAM_TEST, StreamTo1Test)
{
    std::uint8_t c1, c2, c3, c4;
    std::vector<char> data = { 100, 0, 0, 0 };
    pan::net::istream is(data.data(), data.size());
    is >> c1 >> c2 >> c3 >> c4;
    EXPECT_EQ(c1, 100);
    EXPECT_EQ(c2, 0);
    EXPECT_EQ(c3, 0);
    EXPECT_EQ(c4, 0);
    pan::net::bit::istream bis(data.data(), data.size());
    bis >> c1 >> c2 >> c3 >> c4;
    EXPECT_EQ(c1, 100);
    EXPECT_EQ(c2, 0);
    EXPECT_EQ(c3, 0);
    EXPECT_EQ(c4, 0);
}

TEST(INPUT_STREAM_TEST, StreamTo2Test)
{
    std::uint16_t c1, c2;
    std::vector<char> data = { 100, 0, 0, 0 };
    pan::net::istream is(data.data(), data.size());
    is >> c1 >> c2;
    EXPECT_EQ(c1, 100);
    EXPECT_EQ(c2, 0);
    pan::net::bit::istream bis(data.data(), data.size());
    bis >> c1 >> c2;
    EXPECT_EQ(c1, 100);
    EXPECT_EQ(c2, 0);
}

TEST(INPUT_STREAM_TEST, StreamTo4Test)
{
    std::uint32_t c1;
    std::vector<char> data = { 100, 0, 0, 0 };
    pan::net::istream is(data.data(), data.size());
    is >> c1;
    EXPECT_EQ(c1, 100);
    pan::net::bit::istream bis(data.data(), data.size());
    bis >> c1;
    EXPECT_EQ(c1, 100);
}

TEST(INPUT_STREAM_TEST, StreamTo8Test)
{
    std::uint64_t c1;
    std::vector<char> data = { 100, 0, 0, 0, 0, 0, 0, 0 };
    pan::net::istream is(data.data(), data.size());
    is >> c1;
    EXPECT_EQ(c1, 100);
    pan::net::bit::istream bis(data.data(), data.size());
    bis >> c1;
    EXPECT_EQ(c1, 100);
}

TEST(INPUT_STREAM_TEST, StreamToIntTest)
{
    int i1, i2;
    std::vector<char> data = { 100, 0, 0, 0, 20, 0, 0, 0 };
    pan::net::istream is(data.data(), data.size());
    is >> i1 >> i2;
    EXPECT_EQ(i1, 100);
    EXPECT_EQ(i2, 20);
    pan::net::bit::istream bis(data.data(), data.size());
    bis >> i1 >> i2;
    EXPECT_EQ(i1, 100);
    EXPECT_EQ(i2, 20);
}

TEST(INPUT_STREAM_TEST, StreamToFloatTest)
{
    union 
    {
        float f1 = 0.11f;
        std::uint32_t i1;
    };
    union 
    {
        float f2 = 100.3f;
        std::uint32_t i2;
    };
    std::vector<std::uint32_t> data = { i1, i2 };
    pan::net::istream is(data.data(), data.size() * sizeof(std::uint32_t));
    EXPECT_EQ(is.capacity(), 8);
    is >> f1 >> f2;
    EXPECT_TRUE(std::abs(f1 - 0.11f) < 1e-6);
    EXPECT_TRUE(std::abs(f2 - 100.3f) < 1e-6);
    pan::net::bit::istream bis(data.data(), data.size() * sizeof(std::uint32_t));
    EXPECT_EQ(bis.capacity(), 8);
    bis >> f1 >> f2;
    EXPECT_TRUE(std::abs(f1 - 0.11f) < 1e-6);
    EXPECT_TRUE(std::abs(f2 - 100.3f) < 1e-6);
}

TEST(INPUT_STREAM_TEST, StreamToDoubleTest)
{
    union
    {
        double f1 = 0.11;
        std::uint64_t i1;
    };
    union
    {
        double f2 = 100.3;
        std::uint64_t i2;
    };
    std::vector<std::uint64_t> data = { i1, i2 };
    pan::net::istream is(data.data(), data.size() * sizeof(std::uint64_t));
    EXPECT_EQ(is.capacity(), 16);
    is >> f1 >> f2;
    EXPECT_TRUE(std::abs(f1 - 0.11) < 1e-6);
    EXPECT_TRUE(std::abs(f2 - 100.3) < 1e-6);
    pan::net::bit::istream bis(data.data(), data.size() * sizeof(std::uint64_t));
    EXPECT_EQ(bis.capacity(), 16);
    bis >> f1 >> f2;
    EXPECT_TRUE(std::abs(f1 - 0.11) < 1e-6);
    EXPECT_TRUE(std::abs(f2 - 100.3) < 1e-6);
}

TEST(INPUT_STREAM_TEST, ByteStreamToComplexTypeAndTupleTest)
{
    union
    {
        float f1 = 0.11f;
        std::uint32_t i1;
    };
    std::vector<std::uint8_t> data = { 100, 0, 0xff, 1, 0, 20, 0, 0, 0 };
    auto p = reinterpret_cast<std::uint8_t*>(&i1);
    data.insert(data.end(), p, p + 4);
    pan::net::istream is(data.data(), data.size());
    EXPECT_EQ(is.capacity(), 13);
    unsigned short v1;
    unsigned char v2;
    bool v3;
    bool v4;
    int v5;
    float v6;
    is >> v1 >> v2 >> v3 >> v4 >> v5 >> v6;
    EXPECT_EQ(v1, 100);
    EXPECT_EQ(v2, 255);
    EXPECT_EQ(v3, true);
    EXPECT_EQ(v4, false);
    EXPECT_EQ(v5, 20);
    EXPECT_TRUE(std::abs(v6 - 0.11f) < 1e-6);
    // tuple
    pan::net::istream is2(data.data(), data.size());
    auto tp = pan::net::istream_to_tuple<std::tuple<unsigned short, unsigned char, bool, bool, int, float>>(is2);
    EXPECT_EQ(std::get<0>(tp), 100);
    EXPECT_EQ(std::get<1>(tp), 0xff);
    EXPECT_EQ(std::get<2>(tp), true);
    EXPECT_EQ(std::get<3>(tp), false);
    EXPECT_EQ(std::get<4>(tp), 20);
    EXPECT_TRUE(std::abs(std::get<5>(tp) - 0.11f) < 1e-6);
}

TEST(INPUT_STREAM_TEST, BitStreamToComplexTypeAndTupleTest)
{
    union
    {
        float f1 = 0.11f;
        std::uint32_t i1;
    };
    std::vector<std::uint8_t> data = { 100, 0, 0xff, 1, 20, 0, 0, 0 };
    auto p = reinterpret_cast<std::uint8_t*>(&i1);
    data.insert(data.end(), p, p + 4);
    pan::net::bit::istream is(data.data(), data.size());
    EXPECT_EQ(is.capacity(), 12);
    unsigned short v1;
    unsigned char v2;
    bool v3;
    bool v4;
    bool v5;
    bool v6;
    bool v7;
    bool v8;
    bool v9;
    bool v10;
    int v11;
    float v12;
    is >> v1 >> v2 >> v3 >> v4 >> v5 >> v6 >> v7 >> v8 >> v9 >> v10 >> v11 >> v12;
    EXPECT_EQ(v1, 100);
    EXPECT_EQ(v2, 255);
    EXPECT_EQ(v3, true);
    EXPECT_EQ(v4, false);
    EXPECT_EQ(v5, false);
    EXPECT_EQ(v6, false);
    EXPECT_EQ(v7, false);
    EXPECT_EQ(v8, false);
    EXPECT_EQ(v9, false);
    EXPECT_EQ(v10, false);
    EXPECT_EQ(v11, 20);
    EXPECT_TRUE(std::abs(v12 - 0.11f) < 1e-6);
    // tuple
    pan::net::bit::istream is2(data.data(), data.size());
    auto tp = pan::net::bit::istream_to_tuple<std::tuple<unsigned short, unsigned char, bool, bool, bool, bool, bool, bool, bool, bool, int, float>>(is2);
    EXPECT_EQ(std::get<0>(tp), 100);
    EXPECT_EQ(std::get<1>(tp), 0xff);
    EXPECT_EQ(std::get<2>(tp), true);
    EXPECT_EQ(std::get<3>(tp), false);
    EXPECT_EQ(std::get<4>(tp), false);
    EXPECT_EQ(std::get<5>(tp), false);
    EXPECT_EQ(std::get<6>(tp), false);
    EXPECT_EQ(std::get<7>(tp), false);
    EXPECT_EQ(std::get<8>(tp), false);
    EXPECT_EQ(std::get<9>(tp), false);
    EXPECT_EQ(std::get<10>(tp), 20);
    EXPECT_TRUE(std::abs(std::get<11>(tp) - 0.11f) < 1e-6);
}

TEST(INPUT_STREAM_TEST, BitStreamToComplex2Test)
{
    union
    {
        float f1 = 0.11f;
        std::uint32_t i1;
    };
    std::vector<std::uint8_t> data = { 100, 0, 0xff, 0xf9, 20, 0, 0, 0 }; // f9 = 1111 1001
    auto p = reinterpret_cast<std::uint8_t*>(&i1);
    data.insert(data.end(), p, p + 4);
    pan::net::bit::istream is(data.data(), data.size());
    EXPECT_EQ(is.capacity(), 12);
    unsigned short v1;
    unsigned char v2;
    bool v3;
    bool v4;
    bool v5;
    unsigned char v6; // 5 bits
    int v7;
    float v8;
    is >> v1 >> v2 >> v3 >> v4 >> v5;
    is.read(v6, 5);
    is >> v7 >> v8;
    EXPECT_EQ(v1, 100);
    EXPECT_EQ(v2, 255);
    EXPECT_EQ(v3, true);
    EXPECT_EQ(v4, false);
    EXPECT_EQ(v5, false);
    EXPECT_EQ(v6, 0b00011111);
    EXPECT_EQ(v7, 20);
    EXPECT_TRUE(std::abs(v8 - 0.11f) < 1e-6);
}

TEST(INPUT_STREAM_TEST, BitStreamToComplex3Test)
{
    std::vector<std::uint8_t> data = { 0b11000110, 0b10000000, 
        0b11100001, 0b11111001, 0b01010101, 0b10010010, 
        0b00100101, 0b00000001, 0b11111111, 0b11111111 }; // 80 bits
    pan::net::bit::istream is(data.data(), data.size());
    unsigned char v1, v2, v3, v4, v5;
    unsigned short v6 = 0, v7 = 0;
    unsigned int v8 = 0, v9 = 0;
    is.read(v1, 5);
    is.read(v2, 4);
    is.read(v3, 6); // 15
    is.read(v4, 3); // 18 bits
    is.read(v5, 8); // 26
    is.read(v6, 15);

    is.read(v7, 11); // 52
    is.read(v8, 20); // 72
    is.read(v9, 8); // 80
    EXPECT_EQ(v1, 0b110);
    EXPECT_EQ(v2, 0b110);
    EXPECT_EQ(v3, 0);
    EXPECT_EQ(v4, 0b011);
    EXPECT_EQ(v5, 0b01111000);
    EXPECT_EQ(v6, 0b001010101111110);

    EXPECT_EQ(v7, 0b01011001001);
    EXPECT_EQ(v8, 0b11111111000000010010);
    EXPECT_EQ(v9, 0b11111111);
}
