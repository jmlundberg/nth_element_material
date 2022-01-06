/*
 Johan Lundberg 2022-01-08
*/
#pragma once

#include<algorithm>
#include<ranges>
#include<cmath>

#include "selection_impl_utils.h"

namespace nthexple {

  namespace ranges {

    using nthexple::NotQuiteObject;

    /**
     * Common implementation of multi_quick_select, multi_intro_select and generally
     * any quicksort-like multi-selection algorithm. Depending on the pivoting strategy
     * you get guaranteed O(N) of N = size(R)
     */

    template<typename PivotSelectFactory>
    class multi_quick_select_common_fn : private NotQuiteObject {
    public:
      using NotQuiteObject::NotQuiteObject;
      
      //signature multi_quick_select(R&& r, Nths&& nths, Comp comp = {}, Proj proj = {})
      template<
        std::ranges::random_access_range R,
        std::ranges::random_access_range Nths,
        class Comp = std::ranges::less, class Proj = std::identity>
        requires std::sortable< std::ranges::iterator_t<R>, Comp, Proj>
        && std::convertible_to< std::iter_reference_t<std::ranges::iterator_t<Nths>>, std::ranges::iterator_t<R>>
        static constexpr std::ranges::borrowed_iterator_t<R>
        basicMultiQuickSelect(R&& r, Nths&& nths, Comp comp, Proj proj, std::uint32_t allowed_depth)
      {
        nthexple::assertNthsValidForRange(r, nths);
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

        const auto distance = std::distance(first, end);
        if (distance < 2) {
          // Base case: Out of range
          return end;
        }

        constexpr auto RevertToSortAt = 32;
        if (std::distance(first, end) <= RevertToSortAt) {
          // This is an optional optimization that helps significantly for the
          //  edge case of short vectors with a very large number of nths
          //  points. Possibly a little bit of overall worse performance in the general case
          //  but not enough to matter.
          return std::ranges::sort(r, comp, proj);
        }

        const auto& pivot0 = PivotSelectFactory::getPivoter(allowed_depth)(r, comp, proj);

        const auto projPivot = proj(pivot0); // If error here, read comment:
        // For now, only support copyable types, because std::partition does not work with pivot elements
        // that are references into the partitioned range

        const auto firstPivotToEnd = ranges::partition(r, [&projPivot, comp](const auto& elem) { return comp(elem, projPivot); }, proj);
        const auto lowerNths = subrange(nths_first, ranges::lower_bound(nths, firstPivotToEnd.begin()));
        {
          const auto lastPivotToEnd = ranges::partition(firstPivotToEnd, [&projPivot, comp](const auto& elem) { return !comp(projPivot, elem); }, proj);
          const auto afterPivot = lastPivotToEnd.begin();
          const auto upperNths = subrange(ranges::lower_bound(subrange(lowerNths.end(), nths_end), afterPivot), nths_end);
          basicMultiQuickSelect(lastPivotToEnd, upperNths, comp, proj, nthexple::noWrapSub1(allowed_depth));
        }
        const auto leftRange = subrange(first, firstPivotToEnd.begin());
        basicMultiQuickSelect(leftRange, lowerNths, comp, proj, nthexple::noWrapSub1(allowed_depth));

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
        const auto allowed_depth = PivotSelectFactory::calcMaxRecursionDepth(std::as_const(r));
        return basicMultiQuickSelect(r, nths, comp, proj, allowed_depth);
      }

      //Signature multi_quick_select(I first, Nths&& nths, S last, Comp comp = {}, Proj proj = {})
      template<std::random_access_iterator I, std::sentinel_for<I> S,
        std::ranges::random_access_range Nths, class Comp = std::ranges::less, class Proj = std::identity>
        requires std::sortable<I, Comp, Proj>
        && std::convertible_to<std::iter_reference_t<std::ranges::iterator_t<Nths>>, I>
        constexpr I
        operator()(I first, Nths&& nths, S last, Comp comp = {}, Proj proj = {}) const
      {
        return multi_quick_select_common_fn::operator()(std::ranges::subrange(first, last), nths, comp, proj);
      }
    };

  }

}