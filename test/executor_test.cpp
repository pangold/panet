#include <gtest/gtest.h>
#include <net/base/stream.hpp>
#include <net/rpc/executor.hpp>

static int add(int x, int y) 
{ 
    return x + y; 
}

struct math 
{
    int add(int x, int y) { return x + y; }
    int minus(int x, int y) { return x - y; }
    int multiply(int x, int y) { return x * y; }
    int divide(int x, int y) { return x / y; }
    void print(int x, int y) { std::cout << "print -> x: " << x << ", y: " << y << std::endl; }
};

TEST(EXECUTOR_TEST, BaseTest)
{
    std::vector<int> data = { 100, 100 };
    pan::net::bit::ostream os;
    pan::net::rpc::executor exe;
    math math;

    exe.bind("add", add);
    exe.bind("math.add", &math::add, &math);
    exe.bind("math.minus", &math::minus, &math);
    exe.bind("math.multiply", &math::multiply, &math);
    exe.bind("math.divide", &math::divide, &math);
    exe.bind("math.print", &math::print, &math);

    exe.invoke("add", os, pan::net::bit::istream(data.data(), data.size() * sizeof(int)));
    exe.invoke("math.add", os, pan::net::bit::istream(data.data(), data.size() * sizeof(int)));
    exe.invoke("math.minus", os, pan::net::bit::istream(data.data(), data.size() * sizeof(int)));
    exe.invoke("math.multiply", os, pan::net::bit::istream(data.data(), data.size() * sizeof(int)));
    exe.invoke("math.divide", os, pan::net::bit::istream(data.data(), data.size() * sizeof(int)));
    exe.invoke("math.print", os, pan::net::bit::istream(data.data(), data.size() * sizeof(int)));

    try {
        exe.invoke("math.not.exist", os, pan::net::bit::istream(data.data(), data.size() * sizeof(int)));
        EXPECT_TRUE(false);
    }
    catch (pan::net::rpc::executor::handler_not_found_error& /*e*/) {
        EXPECT_TRUE(true);
    }

    int r1, r2, r3, r4, r5 = 0;
    pan::net::bit::istream ris(os.data(), os.size());
    ris >> r1 >> r2 >> r3 >> r4 >> r5;
    EXPECT_EQ(r1, 200);
    EXPECT_EQ(r2, 200);
    EXPECT_EQ(r3, 0);
    EXPECT_EQ(r4, 10000);
    EXPECT_EQ(r5, 1);
}