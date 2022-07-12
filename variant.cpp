#include <cstddef>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

template <typename T, T a, T... others>
static constexpr T max = a > max<T, others...> ? a : max<T, others...>;
template <typename T, T a> static constexpr T max<T, a> = a;

template <typename... types>
static constexpr size_t largest = max<size_t, sizeof(types)...>;

template <typename A, typename B> static constexpr bool isSame = false;
template <typename T> static constexpr bool isSame<T, T> = true;

template <typename... types> static constexpr bool containedIn = false;

template <typename T> static constexpr bool containedIn<T> = false;
template <typename T, typename A, typename... others>
static constexpr bool containedIn<T, A, others...> =
    isSame<T, A> ? true : containedIn<T, others...>;

template <int count = 0, typename... types> static constexpr int indexOf = -1;

template <int count, typename T> static constexpr int indexOf<count, T> = -1;
template <int count, typename T, typename A, typename... others>
static constexpr int indexOf<count, T, A, others...> =
    isSame<T, A> ? count : indexOf<count + 1, T, others...>;

template <typename... types> class Variant {
public:
  template <typename T>
  requires(containedIn<T, types...>) Variant(T value) {
    new (bytes) T(std::forward<T>(value));
    valueDestructor = destructor<T>;
    valueCopyConstructor = copyConstructor<T>;
    valueMoveConstructor = moveConstructor<T>;
    type = indexOf<0, T, types...>;
  }
  template <typename T>
  requires(containedIn<T, types...>) Variant<types...>
  &operator=(T value) {
    valueDestructor(bytes);
    new (bytes) T(std::forward<T>(value));
    valueDestructor = destructor<T>;
    valueCopyConstructor = copyConstructor<T>;
    valueMoveConstructor = moveConstructor<T>;
    type = indexOf<0, T, types...>;
    return *this;
  }
  Variant<types...> &operator=(const Variant<types...> &other) {
    valueDestructor(bytes);
    other.valueCopyConstructor(bytes, other.bytes);
    valueDestructor = other.valueDestructor;
    valueCopyConstructor = other.valueCopyConstructor;
    valueMoveConstructor = other.valueMoveConstructor;
    type = other.type;
    return *this;
  }
  Variant<types...> &operator=(Variant<types...> &&other) {
    valueDestructor(bytes);
    other.valueMoveConstructor(bytes, other.bytes);
    valueDestructor = other.valueDestructor;
    valueCopyConstructor = other.valueCopyConstructor;
    valueMoveConstructor = other.valueMoveConstructor;
    type = other.type;
    return *this;
  }
  Variant(const Variant<types...> &other) {
    // To avoid code duplication we try to call the assignment operator
    // whereever possible. This is a bit of a hack, but it works.
    valueDestructor = [](std::byte *) {};
    *this = std::forward<const Variant<types...> &>(other);
  }
  Variant(Variant<types...> &&other) {
    // See above.
    valueDestructor = [](std::byte *) {};
    *this = std::forward<Variant<types...> &&>(other);
  }
  ~Variant() { valueDestructor(bytes); }

  template <typename T>
  requires(containedIn<T, types...>) const T &get() {
    if (type == indexOf<0, T, types...>) {
      return *(const T *)bytes;
    } else {
      throw std::invalid_argument(
          "Variant does not contain the requested type");
    }
  }

  template <typename T>
  requires(containedIn<T, types...>) bool is() {
    return type == indexOf<0, T, types...>;
  }

private:
  std::byte bytes[largest<types...>];
  void (*valueDestructor)(std::byte *);
  void (*valueCopyConstructor)(std::byte *to, const std::byte *from);
  void (*valueMoveConstructor)(std::byte *to, std::byte *from);
  int type;

  template <typename T> static void destructor(std::byte *bytes) {
    ((T *)bytes)->~T();
  }
  template <typename T>
  static void copyConstructor(std::byte *to, const std::byte *from) {
    new (to) T(*(const T *)from);
  }
  template <typename T>
  static void moveConstructor(std::byte *to, std::byte *from) {
    new (to) T(std::move(*(T *)from));
  }
};

class T1 {
public:
  T1() { std::cout << "Constructing t1" << std::endl; }
  T1(const T1 &other) {
    *this = std::forward<const T1 &>(other);
    std::cout << "(Copy constructor)" << std::endl;
  }
  T1(T1 &&other) {
    *this = std::forward<T1 &&>(other);
    std::cout << "(Move constructor)" << std::endl;
  }
  T1 &operator=(const T1 &other) {
    a = other.a;
    std::cout << "Copying t1" << std::endl;
    return *this;
  }
  T1 &operator=(T1 &&other) {
    a = other.a;
    other.a = 0;
    std::cout << "Moving t1" << std::endl;
    return *this;
  }

  ~T1() { std::cout << "Destroying t1" << std::endl; }

  int a;
};
class T2 {
public:
  T2() { std::cout << "Constructing t2" << std::endl; }
  T2(const T2 &other) {
    *this = std::forward<const T2 &>(other);
    std::cout << "(Copy constructor)" << std::endl;
  }
  T2(T2 &&other) {
    *this = std::forward<T2 &&>(other);
    std::cout << "(Move constructor)" << std::endl;
  }
  T2 &operator=(const T2 &other) {
    a = other.a;
    std::cout << "Copying t2" << std::endl;
    return *this;
  }
  T2 &operator=(T2 &&other) {
    a = other.a;
    other.a = 0;
    std::cout << "Moving t2" << std::endl;
    return *this;
  }
  ~T2() { std::cout << "Destroying t2" << std::endl; }

  double a;
};

int main() {
  Variant<T1, T2, double> ts(T1{});
  ts = T2{};
  ts = T1{};
  ts = T1{};
  std::cout << "Copying the variant" << std::endl;
  Variant<T1, T2, double> ts2 = ts;
   ts = 2.5;
  if (ts.is<double>()) {
    std::cout << ts.get < double>() << std::endl;
  }else if (ts.is<T1>()) {
    std::cout << ts.get < T1>().a << std::endl;
  }else {
    std::cout << ts.get < T2>().a << std::endl;
  }
  return 0;
}
