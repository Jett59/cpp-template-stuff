#include <cstdint>

constexpr bool checkIsPrime(uintmax_t n) {
  if (n % 2 == 0) {
    return false;
  } else {
    for (uintmax_t i = 3; i < n; i += 2) {
      if (n % i == 0) {
        return false;
      }
    }
    return true;
  }
}
template <uintmax_t n> struct isPrime {
  enum : bool { value = checkIsPrime(n); };
};

constexpr bool isDigit(char c) { return c >= '0' && c <= '9'; }

template <typename T> constexpr T toNumber(const char *str, T accumulator = 0) {
  return *str == 0 ? accumulator
                   : isDigit(*str)
                         ? toNumber(str + 1, accumulator * 10 + (*str - '0'))
                         : throw "Not a digit";
}

template <char... str> consteval uintmax_t operator""_isPrime() {
  constexpr char chars[] = {str..., 0};
  return isPrime<toNumber<uintmax_t>(chars)>::value;
}

#include <iostream>

using std::cout;
using std::endl;

int main() {
  cout << 31_isPrime << endl;
  return 0;
}
