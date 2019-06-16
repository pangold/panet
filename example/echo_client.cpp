#include <pan/net/echo.hpp>

int main(int argc, char** argv)
{
    pan::net::echo::client client("localhost", 8887);
    client.write("Hello");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}