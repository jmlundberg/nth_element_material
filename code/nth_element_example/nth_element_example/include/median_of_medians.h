/*
 Johan Lundberg 2022-01-08
*/
#pragma once

#include <cassert>
#include <ranges>
#include <algorithm>
#include <cmath>

#include "selection_impl_utils_fwd.h"
#include "single_quick_select.h"
namespace nthexple {

  /// Median of medians deterministic algorithm for finding a pivot with
  /// guaranteed fractional progress.
  /// Ref. eg https://www.csc.kth.se/~johanh/algnotes.pdf
  /// https://en.wikipedia.org/wiki/Johan_H%C3%A5stad
  /// The algorithm returns a good pivot that is guaranteed
  ///
  static const auto medianOfMedians(std::ranges::random_access_range auto&& r,
    auto comp, auto proj)
  {
    auto first = std::begin(r);
    auto last = std::end(r);
    const auto dist = std::ranges::distance(r);
    if (dist <= 15) {
      std::ranges::sort(r, comp, proj);
      return std::next(first, dist / 2);
    }

    mediansToFirstInGroupOf5(r, comp, proj);
    // We can gather median of fives anywhere, eg at the beginning of the range,
    // but the center does make more sense to me as we then on average move
    // in sorting order:
    auto into = std::next(first, 2 * (dist / 5));
    auto medianGroup = gatherStride5ToIterator(r, into);
    auto mid = medianGroup.begin() + std::ranges::distance(medianGroup) / 2;
    nthexple::ranges::quick_medmed_select(medianGroup, mid, comp, proj);
    return mid;
  }

  /// Note medianOfMedians. Range must not be empty. Returns a ref into the range
  static const auto& medianOfMediansRef(std::ranges::random_access_range auto&& r,
    auto comp, auto proj)
  {
    const auto dist = std::ranges::distance(r);
    assertHere(dist > 0);
    return *medianOfMedians(r, comp, proj);
  }

}