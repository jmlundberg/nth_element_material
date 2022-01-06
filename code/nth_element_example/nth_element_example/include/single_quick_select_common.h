/*
 Johan Lundberg 2022-01-08
*/
#pragma once

#include <ranges>
#include <algorithm>

#include "selection_impl_utils.h"
namespace nthexple {

  namespace ranges {

    using nthexple::NotQuiteObject;

    /**
     * Common implementation of quick_select, intro_select and generally
     * any quicksort-like single-selection algorithm. Depending on the pivoting strategy
     * you get guaranteed O(N) of N = size(R)
     */

    template<typename PivotSelectFactory>
    class single_quick_select_common_fn : private NotQuiteObject {
    public:
      using NotQuiteObject::NotQuiteObject;

      template<std::ranges::random_access_range R,
        class Comp = std::ranges::less, class Proj = std::identity>
        requires std::sortable< std::ranges::iterator_t<R>, Comp, Proj>
        static constexpr std::ranges::borrowed_iterator_t<R>
        basicQuickSelect(R&& r, std::ranges::iterator_t<R> nth, Comp comp , Proj proj, uint32_t allowed_depth ) {
        nthexple::assertNthValidForRange(r, nth);
        const auto first = std::begin(r);
        const auto end = std::end(r);
        if (std::distance(first, end) < 2 || nth == end) {
          // Base case: Out of range
          return end;
        }

        constexpr auto RevertToSortAt = 32;
        if (std::distance(first, end) <= RevertToSortAt) {
          return std::ranges::sort(r, comp, proj);
        }

        const auto& pivot0 = PivotSelectFactory::getPivoter(allowed_depth)(r, comp, proj);

        auto projPivot = proj(pivot0); // If error here, read comment:
        // For now, only support copyable types, because std::partition does not work with pivot elements
        // that are references into the partitioned range

        const auto rightPartRange = std::ranges::partition(r, [&projPivot, comp](const auto& elem) { return comp(elem, projPivot); }, proj);
        if (nth < rightPartRange.begin()) {
          basicQuickSelect(std::ranges::subrange(first, rightPartRange.begin()), nth, comp, proj, nthexple::noWrapSub1(allowed_depth ));
        }
        else if (rightPartRange.begin() <= nth) {
          const auto rightPartRange2 = std::ranges::partition(rightPartRange, [&projPivot, comp](const auto& elem) { return !comp(projPivot, elem); }, proj);
          if (rightPartRange2.begin() <= nth) {
            basicQuickSelect(rightPartRange2, nth, comp, proj, nthexple::noWrapSub1(allowed_depth));
          }
        }
        return end;
      }


      template<std::ranges::random_access_range R,
        class Comp = std::ranges::less, class Proj = std::identity>
        requires std::sortable< std::ranges::iterator_t<R>, Comp, Proj>
        constexpr std::ranges::borrowed_iterator_t<R>
        operator()(R&& r, std::ranges::iterator_t<R> nth, Comp comp=std::ranges::less(), Proj proj=std::identity()) const
      {
        const auto allowed_depth = PivotSelectFactory::calcMaxRecursionDepth(std::as_const(r));
        return basicQuickSelect(r, nth, comp, proj, allowed_depth);
      }
    };
  }
}


