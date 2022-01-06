/*
 Johan Lundberg 2022-01-08
*/

#pragma once
#include <cstdint>
#include <limits>
#include "selection_impl_utils_fwd.h"

namespace nthexple {

  struct PivotAndFallbackPolicies {

    /* Intro-select like policy for ensured worst case O(N log N) selection and sorting
      Selection is O(N) on average. Multi selection is O(N log m) on average, where
      m is the number of unique nth.

      Fast and simple median of 3 in the happy day case
      Falls back to deterministic median-of-median pivot to ensure worst case O(N log N) with
      no observable negative effect on th average case.

      The idea is simple. Ano ~2003: Musser http://www.cs.rpi.edu/~musser/gp/algorithms.html

      The cost of getting the guaranteed linear behaviour for kth element select or guaranteed
      nlog n for sort is small: you only need to keep count of how deep you go in quick
      sort or quick select: a single integer. And if you go too deep: ( Musser used: maxdepth = 2 logN )
      you from there switch over to a slower pivot finder (median of medians) that gives you
      guaranteed linear performance.

      In the paper by Musser the fall back for intro *sort* was to revert to heap sort since it's
      worst case O(N). For the (single/multi) *selection* problem the idea is the same, but instead
      of heap sort we use quicksort with medianOfMedian-pivoting.
      */
    struct NintherLogFallbackToMedMed
    {
      static uint32_t calcMaxRecursionDepth(const std::ranges::random_access_range auto& r)
      {
        const auto z = std::max(static_cast<double>(std::ranges::size(r)), 1024.0);
        auto allowed_depth = static_cast<std::uint32_t>(2 * std::log2(z));
        return allowed_depth;
      }

      static auto getPivoter(uint32_t remainingDepth)
      {
        return [remainingDepth](std::ranges::random_access_range auto&& r,
          auto comp, auto proj) -> decltype(auto) {
            if (remainingDepth == 0) {
              return nthexple::medianOfMediansRef(r, comp, proj);
            }
            return nthexple::uncheckedNintherPivot(r, comp, proj);
        };
      }
    };

    /* As NintherLogFallbackToMedMed, but gives guaranteed
     * worst case O(N) selection by allowing no more than MaxDepth
     * recursions before reverting to median of median
     *
     * A better guaranteed O(N) algorithm is Andrei Alexandrescu,
     * 10.4230/LIPIcs.SEA.2017.24
     * "median of ninthers, Fast Deterministic Selection"
     * https://erdani.com/research/sea2017.pdf
     * https://github.com/andralex/MedianOfNinthers
     */
    template<uint32_t MaxDepth = 24>
    struct NintherKFallbackToMedMed
    {
      static uint32_t calcMaxRecursionDepth(const std::ranges::random_access_range auto& r)
      {
        const auto maxd = NintherLogFallbackToMedMed::calcMaxRecursionDepth(r);
        return std::min(MaxDepth, maxd);
      }

      static auto getPivoter(uint32_t remainingDepth)
      {
        return NintherLogFallbackToMedMed::getPivoter(remainingDepth);
      }
    };

    /// Median of median pivoter. Ignores the depth. Guaranteed O(N) but slower than ninther
    struct MedMed
    {
      static uint32_t calcMaxRecursionDepth(const std::ranges::random_access_range auto&)
      {
        return std::numeric_limits<uint32_t>::max();
      }

      static auto getPivoter([[maybe_unused]] uint32_t remainingDepth)
      {
        return [](std::ranges::random_access_range auto&& r,
          auto comp, auto proj) -> decltype(auto) {
            return nthexple::medianOfMediansRef(r, comp, proj);
        };
      }
    };

    /// Ignores the depth. O(N) on average but worst-case O(N^2)
    struct Ninther
    {
      static uint32_t calcMaxRecursionDepth(const std::ranges::random_access_range auto&)
      {
        return std::numeric_limits<uint32_t>::max();
      }

      static auto getPivoter([[maybe_unused]] uint32_t remainingDepth)
      {
        return [](std::ranges::random_access_range auto&& r,
          auto comp, auto proj) -> decltype(auto) {
            return nthexple::uncheckedNintherPivot(r, comp, proj);
        };
      }
    };
  };
} 