/*
 Johan Lundberg 2022-01-08
*/

/**
 *
 * Multi select implemented using multi quick select
 *
 * The algorithm has expected O(N log m) complexity
 *
 */

#pragma once
#include "multi_quick_select_common.h"
#include "selection_policies.h"
#include "median_of_medians.h"

namespace nthexple {
  
  namespace ranges {
    using ranges::multi_quick_select_common_fn;

    /// multi_quick_select. Select on all nths in range 
    /// Guaranteed O(N log m)
    inline constexpr multi_quick_select_common_fn<PivotAndFallbackPolicies::Ninther> multi_quick_select{ NotQuiteObject::_Construct_tag{} };

    /// multi_intro_select. Select on all nths in range
    /// Expected O(N log m), worst case O(N log N). Read comments in NintherLogFallbackToMedMed
    inline constexpr multi_quick_select_common_fn<PivotAndFallbackPolicies::NintherLogFallbackToMedMed> multi_logintro_select{ NotQuiteObject::_Construct_tag{} };
   
    /// multi_quick_select. Select on all nths in range 
    /// Guaranteed O(N log m). Read comments in NintherKFallbackToMedMed
    inline constexpr multi_quick_select_common_fn<PivotAndFallbackPolicies::NintherKFallbackToMedMed<>> multi_intro_select{ NotQuiteObject::_Construct_tag{} };

  }

  /// multi_intro_select. Select on all nths in range 
  template< class RandomAccessIterator, class RandomAccessIteratorNths, class Comp>
  constexpr void multi_intro_select(RandomAccessIterator first, RandomAccessIteratorNths nth_first, RandomAccessIteratorNths nth_last, RandomAccessIterator last, Comp comp)
  {
    return nthexple::multi_intro_select(std::ranges::subrange(first, last), std::ranges::subrange(nth_first, nth_last), comp);
  }

  /// multi_intro_select. Select on all nths in range 
  template< class RandomAccessIterator, class RandomAccessIteratorNths >
  constexpr void multi_intro_select(RandomAccessIterator first, RandomAccessIteratorNths nth_first, RandomAccessIteratorNths nth_last, RandomAccessIterator last)
  {
    return multi_intro_select(first, nth_first, nth_last, last, std::less());
  }


  /// multi_quick_select. Select on all nths in range 
  template< class RandomAccessIterator, class RandomAccessIteratorNths, class Comp>
  constexpr void multi_quick_select(RandomAccessIterator first, RandomAccessIteratorNths nth_first, RandomAccessIteratorNths nth_last, RandomAccessIterator last, Comp comp)
  {
    return nthexple::multi_quick_select(std::ranges::subrange(first, last), std::ranges::subrange(nth_first, nth_last), comp);
  }

  /// multi_quick_select. Select on all nths in range 
  template< class RandomAccessIterator, class RandomAccessIteratorNths >
  constexpr void multi_quick_select(RandomAccessIterator first, RandomAccessIteratorNths nth_first, RandomAccessIteratorNths nth_last, RandomAccessIterator last)
  {
    return multi_quick_select(first, nth_first, nth_last, last, std::less());
  }

}