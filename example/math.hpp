#ifndef __MATH_HPP__
#define __MATH_HPP__


template <typename T>
T add(T x, T y)
{
    return x + y;
}

template <typename T>
struct math {
    T add(T x, T y) { return x + y; }
    T minus(T x, T y) { return x - y; }
    T multiply(T x, T y) { return x * y; }
    T divide(T x, T y) { return x / y; }
};

void voidf(int x, int y)
{
    std::cout << "x: " << x << ", y: " << y << std::endl;
}


#endif // __MATH_HPP__