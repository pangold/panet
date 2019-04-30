#include <cstdint>
#include <vector>
#include <iostream>
#include <pan/net.hpp>
#include "example/math.hpp"

#include "example/rpc.cpp"
#include "example/echo.cpp"

int main(int argc, char** argv)
{
#if defined(AS_SERVER)
    rpc_server();
#else
    rpc_client();
#endif
    return 0;
}
