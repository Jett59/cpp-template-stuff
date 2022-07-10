#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

template <typename T>
extern const T declval; // Not defined anywhere.

template <typename F, typename... Args>
using ResultType = decltype(declval<F>(declval<Args>...));

template<typename F, typename... Args>
ResultType<F, Args...> invoke(F f, Args... args) {
  return f(args...);
}

class NotDefaultConstructable {
    public:
    NotDefaultConstructable(int a) {
        std::cout << "NotDefaultConstructable(int a)" << std::endl;
    }

    int operator()(int a) const {
        std::cout << "operator()(int a)" << std::endl;
        return a;
    }
};

double func(double a, int b, std::string c) {
    std::cout << a << " " << b << " " << c << std::endl;
    return static_cast<int>(a*b);
}

int main() {
  double result = invoke(func, 2, 5, "hi");
  std::cout << result << std::endl;
  return 0;
}
