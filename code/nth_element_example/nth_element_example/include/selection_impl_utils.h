/*
 Johan Lundberg 2022-01-08
*/
#pragma once
#include <cassert>
#include <ranges>
#include <algorithm>
#include <cmath>

#include "selection_impl_utils_fwd.h"
#include "ms_stl_median_common.h"

namespace nthexple {

#ifdef _MSVC_STL_VERSION
  using NotQuiteObject = std::ranges::_Not_quite_object;
#else
  struct NotQuiteObject
  {
    struct _Construct_tag {};
    constexpr NotQuiteObject(_Construct_tag) {}
  }; // Dummy
#endif

  inline static void assertHere(bool v) noexcept
  {
    if (!v) {
      //std::cerr << "fail" << std::endl;
      assert(v);
    }
  }


  // Subtract one, down to zero
  static std::uint32_t noWrapSub1(const std::uint32_t allowed_depth)
  {
    return std::max(allowed_depth, 1U) - 1;
  }

  /// Protective assert. Also note, the test itself is ub in case nth is outside the underlying range
  template <std::ranges::random_access_range R, std::random_access_iterator Nth>
  static void assertNthValidForRange([[maybe_unused]] const R& r, [[maybe_unused]] const Nth& nth) noexcept
  {
#ifndef NDEBUG
    nthexple::assertHere(nth <= r.end());
    nthexple::assertHere(nth >= r.begin());
#endif
  }

  template <std::ranges::random_access_range R, std::ranges::random_access_range Nths>
  static void assertNthsValidForRange([[maybe_unused]] const R& r, [[maybe_unused]] const Nths& nths) noexcept
  {
#ifndef NDEBUG
    for (auto nth : nths) {
      nthexple::assertNthValidForRange(r, nth);
    }
#endif
  }

  /// Just the middle element 
  static auto uncheckedMidpointPivot(std::ranges::random_access_range auto&& r,
    [[maybe_unused]] auto comp, [[maybe_unused]] auto proj)
  {
    auto first = std::begin(r);
    auto last = std::end(r);
    assertHere(first != last);
    return *std::next(first, std::ranges::distance(r) / 2);
  }


  /// Median of 3 pivot is fast in the general case, and is good with benign data eg
  /// random data, previously sorted data (backwards/forwards). 
  static const auto& uncheckedMedianOf3Pivot(std::ranges::random_access_range auto&& r,
    auto comp, auto proj)
  {
    auto first = std::begin(r);
    auto last = std::end(r);
    const auto dist = std::ranges::distance(r);
    assertHere(dist > 0);
    if (dist < 2) {
      return *first;
    }
    const auto& a = *first;
    const auto& b = *std::next(first, std::ranges::distance(r) / 2);
    const auto& c = *std::prev(last);
    std::array arr{ std::ref(a), std::ref(b), std::ref(c) };
    std::ranges::sort(arr, comp, proj); // Easy way to get it right with comp, proj.
    return arr[1].get();
  }

  /// Ninther pivot
  static const auto& uncheckedNintherPivot(std::ranges::random_access_range auto&& r,
    auto comp, auto proj)
  {
    auto first = std::begin(r);
    auto last = std::end(r);
    const auto dist = std::ranges::distance(r);
    assertHere(dist>0);
    if (dist < 2) {
      return *first;
    }
    auto mid = first + dist / 2;
    Guess_median_common(first, mid, last-1, comp, proj);
    return *mid;
    
  }

  /// Move the median element of a range range to the first element.
  /// Is  O( n log n ), for typical use on a tiny number of elements. 
  static const void medianToFirst(std::random_access_iterator auto first,
    std::random_access_iterator auto end,
    auto comp, auto proj)
  {
    assertHere(first != end);
    std::ranges::sort(std::ranges::subrange(first, end), comp, proj);
    std::iter_swap(first, std::next(first,std::distance(first, end) / 2));
  }

  static const void mediansToFirstInGroupOf5(std::ranges::random_access_range auto && r,
    auto comp, auto proj)
  {
    auto first = std::begin(r);
    auto last = std::end(r);
    const auto dist = std::ranges::distance(r);
    assertHere(dist > 0); // For now, remove if 0 is useful
    auto it = first;
    for (auto grp = 0 * dist; grp < dist / 5; ++grp) {
      auto next = std::next(it, 5);
      medianToFirst(it, next, comp, proj);
      it = next;
    }
    if (it != last) {
      medianToFirst(it, last, comp, proj);
    }
  }

  // Gathers every 5 elements from range at onto, returns end of onto range
  static const auto gatherStride5ToIterator(std::ranges::random_access_range auto && r,
    std::random_access_iterator auto onto)
  {
    auto first = std::begin(r);
    auto last = std::end(r);

    const auto dist = std::ranges::distance(r);
    const auto groups = dist / 5 + (dist % 5 == 0 ? 0 : 1);
    auto into_it = onto;
    for (auto grp = 0 * dist; grp < groups; ++grp, ++into_it) {
      auto firstOfGroup = std::next(first, 5 * grp);
      std::iter_swap(firstOfGroup, into_it);
    }
    return std::ranges::subrange(onto,into_it);
  }

}