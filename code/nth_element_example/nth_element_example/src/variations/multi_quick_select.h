#pragma once

#include<algorithm>
#include <cassert>
#include<ranges>
#include<cmath>
/**
 *
 * Multi select implemented using multi quick select
 *
 * The algorithm has expected O(N log m) complexity
 *
 */

namespace quickselect_nth_element {


  namespace ranges {

#ifdef _MSVC_STL_VERSION
    using NotQuiteObject = std::ranges::_Not_quite_object;
#else
    struct NotQuiteObject
    {
      struct _Construct_tag {};
      constexpr NotQuiteObject(_Construct_tag) {}
    }; // Dummy
#endif

    class multi_quick_select_fn : private NotQuiteObject {
    public:
      using NotQuiteObject::NotQuiteObject;

      static auto uncheckedMidpointPivot(const std::random_access_iterator auto first, std::random_access_iterator auto last,
        [[maybe_unused]] auto comp = std::less(), [[maybe_unused]] auto proj = std::identity())
      {
        return *std::next(first, std::distance(first, last) / 2);
      }

      static const auto& uncheckedMedianOf3Pivot(const std::random_access_iterator auto first, std::random_access_iterator auto last,
        auto comp, auto proj)
      {
        if (std::distance(first, last) < 2) {
          return *first;
        }
        const auto& a = *first;
        const auto& b = *std::next(first, std::distance(first, last) / 2);
        const auto& c = *std::prev(last);
        std::array arr{ std::ref(a),std::ref(b),std::ref(c) };
        std::ranges::sort(arr, comp, proj);
        return arr[1].get();
      }

      static const auto& uncheckedMedianOf5Pivot(const std::random_access_iterator auto first, std::random_access_iterator auto last,
        auto comp = std::less(), auto proj = std::identity())
      {
        const auto& a = *first;
        const auto& b0 = *std::next(first, std::distance(first, last) / 4);
        const auto& b1 = *std::next(first, std::distance(first, last) / 2);
        const auto& b2 = *std::next(first, 3 * (std::distance(first, last) / 4));
        const auto& e = *std::prev(last);
        std::array arr{ std::ref(a),std::ref(b0),std::ref(b1), std::ref(b2), std::ref(e) };
        std::ranges::sort(arr, comp, proj);
        return arr[2].get();
      }

      static void assertHere(bool v) noexcept
      {
        if (!v) {
          //std::cerr << "fail" << std::endl;
          assert(v);
        }
      }

      template <std::ranges::random_access_range R, std::ranges::random_access_range Nths>
      static void assertNthsInRange([[maybe_unused]] R&& r, [[maybe_unused]] Nths&& nths) noexcept
      {
#ifndef NDEBUG
        for (auto nth : nths) {
          assertHere(nth <= r.end());
          assertHere(nth >= r.begin());
        }
#endif
      }

      //signature multi_quick_select(R&& r, Nths&& nths, Comp comp = {}, Proj proj = {})
      template<std::ranges::random_access_range R,
        std::ranges::random_access_range Nths,
        class Comp = std::ranges::less, class Proj = std::identity>
        requires std::sortable< std::ranges::iterator_t<R>, Comp, Proj>
        && std::convertible_to< std::iter_reference_t<std::ranges::iterator_t<Nths>>, std::ranges::iterator_t<R>>
        static constexpr std::ranges::borrowed_iterator_t<R>
        basicMultiQuickSelect(R&& r, Nths&& nths, Comp comp, Proj proj, std::uint32_t allowed_depth)
      {
        assertNthsInRange(r, nths);
        namespace ranges = std::ranges;
        using ranges::subrange;
        const auto first = std::begin(r);
        const auto end = std::end(r);
        const auto nths_first = std::begin(nths);
        const auto nths_end = std::end(nths);

        if (nths_first == nths_end || *nths_first == end) {
          // Base case: Out of Nths
          return end;
        }

        /*
        if (*nths_first == *std::prev(nths_end)) {
          // For the limited tests I have this is actually about 10% degradation.
          return std::ranges::nth_element(r, *nths_first, comp, proj);
        }
        */
        if (std::distance(first, end) < 2) {
          // Base case: Out of range
          return end;
        }

        const auto RevertToSortAt = 32;
        if (std::distance(first, end) <= RevertToSortAt) {
          // This is an optional optimization that helps significantly for the
          //  edge case of short vectors with a very large number of nths
          //  points. Possibly a little bit of overall worse performance in the general case
          //  but not enough to matter.
          return std::ranges::sort(r, comp, proj);
        }
        /*
        if(allowed_depth==0){
          // This is an sketch on guaranteed instead of average O(N) lookup, similar to
          // intro sort:
          // This means we are not doing enough progress, eg due to median-of-3 hostile
          // input data. Then revert back to slightly slower algorithm with guaranteed
          // worst case performance.
          return ...:  implement multi_quick_select with median-of-median pivot;
        }
        */

        const auto& pivot0 = uncheckedMedianOf3Pivot(first, end, comp, proj);

        const auto projPivot = proj(pivot0);
        const auto firstPivotToEnd = ranges::partition(r, [&projPivot, comp](const auto& elem) { return comp(elem, projPivot); }, proj);
        const auto lowerNths = subrange(nths_first, ranges::lower_bound(nths, firstPivotToEnd.begin()));
        {
          const auto lastPivotToEnd = ranges::partition(firstPivotToEnd, [&projPivot, comp](const auto& elem) { return !comp(projPivot, elem); }, proj);
          const auto afterPivot = lastPivotToEnd.begin();
          const auto upperNths = subrange(ranges::lower_bound(subrange(lowerNths.end(), nths_end), afterPivot), nths_end);
          basicMultiQuickSelect(lastPivotToEnd, upperNths, comp, proj, allowed_depth - 1);
        }
        const auto leftRange = subrange(first, firstPivotToEnd.begin());
        basicMultiQuickSelect(leftRange, lowerNths, comp, proj, allowed_depth - 1);

        return end;
      }

      //signature multi_quick_select(R&& r, Nths&& nths, Comp comp = {}, Proj proj = {})
      template<std::ranges::random_access_range R,
        std::ranges::random_access_range Nths,
        class Comp = std::ranges::less, class Proj = std::identity>
        requires std::sortable< std::ranges::iterator_t<R>, Comp, Proj>
        && std::convertible_to< std::iter_reference_t<std::ranges::iterator_t<Nths>>, std::ranges::iterator_t<R>>
        constexpr std::ranges::borrowed_iterator_t<R>
        operator()(R&& r, Nths&& nths, Comp comp = {}, Proj proj = {}) const
      {
        const auto z = std::max(static_cast<double>(std::ranges::size(nths)), 1024.0);
        auto allowedDepth = static_cast<std::uint32_t>(2 * std::log2(z) + 1);
        return basicMultiQuickSelect(r, nths, comp, proj, allowedDepth);
      }

      //Signature multi_quick_select(I first, Nths&& nths, S last, Comp comp = {}, Proj proj = {})
      template<std::random_access_iterator I, std::sentinel_for<I> S,
        std::ranges::random_access_range Nths, class Comp = std::ranges::less, class Proj = std::identity>
        requires std::sortable<I, Comp, Proj>
        && std::convertible_to<std::iter_reference_t<std::ranges::iterator_t<Nths>>, I>
        constexpr I
        operator()(I first, Nths&& nths, S last, Comp comp = {}, Proj proj = {}) const
      {
        return multi_quick_select_fn::operator()(std::ranges::subrange(first, last), nths, comp, proj);
      }
    };
    inline constexpr multi_quick_select_fn multi_quick_select{ NotQuiteObject::_Construct_tag{} };

  }

  template< class RandomAccessIterator, class RandomAccessIteratorNths, class Comp>
  constexpr void multi_quick_select(RandomAccessIterator first, RandomAccessIteratorNths nth_first, RandomAccessIteratorNths nth_last, RandomAccessIterator last, Comp comp)
  {
    return quickselect_nth_element::multi_quick_select(std::ranges::subrange(first, last), std::ranges::subrange(nth_first, nth_last), comp);
  }


  template< class RandomAccessIterator, class RandomAccessIteratorNths >
  constexpr void multi_quick_select(RandomAccessIterator first, RandomAccessIteratorNths nth_first, RandomAccessIteratorNths nth_last, RandomAccessIterator last)
  {
    return multi_quick_select(first, nth_first, nth_last, last, std::less());
  }

}