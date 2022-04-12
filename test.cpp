#include <cstdint>

template <uintmax_t n>
static const uintmax_t fibonacci = fibonacci<n - 1> + fibonacci<n - 2>;
template <> static const uintmax_t fibonacci<1> = 1;
template <> static const uintmax_t fibonacci<0> = 0;

#include <iostream>
#include <string>
using namespace std;

constexpr bool isDigit(char c) { return c >= '0' && c <= '9'; }

template <typename T> constexpr T toNumber(const char *str, T accumulator = 0) {
  return *str == 0       ? accumulator
         : isDigit(*str) ? toNumber(str + 1, accumulator * 10 + (*str - '0'))
                         : throw "Not a digit";
}

template <char... str> consteval uintmax_t operator""_fibonacci() {
  constexpr char chars[] = {str..., 0};
  return fibonacci<toNumber<uintmax_t>(chars)>;
}

int main() {
  cout << 10_fibonacci << endl;
  return 0;
}
