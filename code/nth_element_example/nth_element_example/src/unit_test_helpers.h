#pragma once

#ifndef UNIT_TEST_HELPERS_NTHELEMENTS_H_
#define UNIT_TEST_HELPERS_NTHELEMENTS_H_
#include <chrono>
#include <string>

namespace unit_test_helpers {

  template <typename... Args>
  std::string dyna_print(std::string_view rt_fmt_str, Args&&... args) {
    return std::vformat(rt_fmt_str, std::make_format_args(args...));
  }

  struct Timer {
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    static std::chrono::time_point<std::chrono::high_resolution_clock> now() {
      return std::chrono::high_resolution_clock::now();
    }
    Timer() noexcept : start(now()) { }
    auto reset() noexcept { return start = now(); }
    [[nodiscard]] auto tock() const noexcept {
      return now() - start;
    }

    [[nodiscard]] long long tock_ms() const {
      return std::chrono::duration_cast<std::chrono::milliseconds>(now() - start).count();
    }
    [[nodiscard]] long long tock_ns() const {
      return std::chrono::duration_cast<std::chrono::nanoseconds>(now() - start).count();
    }
    [[nodiscard]] long long tock_us() const {
      return std::chrono::duration_cast<std::chrono::microseconds>(now() - start).count();
    }
  };

  template <typename Arg, typename... Args>
  void doPrint(std::ostream& out, Arg&& arg, Args&&... args)
  {
    out << std::forward<Arg>(arg);
    ((out << ',' << std::forward<Args>(args)), ...);
  }

  template <typename Arg, typename... Args>
  void printJsonLine(std::ostream& out, Arg&& arg, Args&&... args)
  {
    out << "[" << std::forward<Arg>(arg);
    ((out << ',' << std::forward<Args>(args)), ...);
    out << "],\n" << std::flush;
  }

  template<typename T>
  constexpr auto square(const T& v)
  {
    return v * v;
  }
}

#endif
