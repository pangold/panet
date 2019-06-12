#ifndef __MATH_HPP__
#define __MATH_HPP__

template <typename T>
class math {
public:
    T add(T t1, T t2) { return t1 + t2; }
    T minus(T t1, T t2) { return t1 - t2; }
    T multiply(T t1, T t2) { return t1 * t2; }
    T divide(T t1, T t2) { return t1 / t2; }
};

template <typename T>
T add(T t1, T t2) { return t1 + t2; }

template <typename T>
T minus(T t1, T t2) { return t1 - t2; }

template <typename T>
T multiply(T t1, T t2) { return t1 * t2; }

template <typename T>
T divide(T t1, T t2) { return t1 / t2; }

#endif // __MATH_HPP__