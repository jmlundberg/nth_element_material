/*
 Johan Lundberg 2022-01-08
*/
#pragma once
#include "single_quick_select_common.h"
#include "selection_policies.h"

namespace nthexple {

  namespace ranges {
    using ranges::single_quick_select_common_fn;

    /// intro_select. Select on all nths in range 
    /// Guaranteed O(N).
    inline constexpr single_quick_select_common_fn<PivotAndFallbackPolicies::NintherKFallbackToMedMed<>> intro_select{ NotQuiteObject::_Construct_tag{} };

    /// quick_select. Select on all nths in range 
    /// Expected O(N), worst case O(N^2)
    inline constexpr single_quick_select_common_fn<PivotAndFallbackPolicies::Ninther> quick_select{ NotQuiteObject::_Construct_tag{} };

    /// quick_select. Select on all nths in range 
    /// Guaranteed O(N).
    inline constexpr single_quick_select_common_fn<PivotAndFallbackPolicies::MedMed> quick_medmed_select{ NotQuiteObject::_Construct_tag{} };

  }

  /// intro_select. Select on all nths in range 
  template< class RandomAccessIterator, class RandomAccessIteratorNths, class Comp>
  constexpr void intro_select(RandomAccessIterator first, RandomAccessIteratorNths nth_first, RandomAccessIteratorNths nth_last, RandomAccessIterator last, Comp comp)
  {
    return nthexple::intro_select(std::ranges::subrange(first, last), std::ranges::subrange(nth_first, nth_last), comp);
  }

  /// intro_select. Select on all nths in range 
  template< class RandomAccessIterator, class RandomAccessIteratorNths >
  constexpr void intro_select(RandomAccessIterator first, RandomAccessIteratorNths nth_first, RandomAccessIteratorNths nth_last, RandomAccessIterator last)
  {
    return intro_select(first, nth_first, nth_last, last, std::less());
  }


  /// quick_select. Select on all nths in range 
  template< class RandomAccessIterator, class RandomAccessIteratorNths, class Comp>
  constexpr void quick_select(RandomAccessIterator first, RandomAccessIteratorNths nth_first, RandomAccessIteratorNths nth_last, RandomAccessIterator last, Comp comp)
  {
    return nthexple::quick_select(std::ranges::subrange(first, last), std::ranges::subrange(nth_first, nth_last), comp);
  }

  /// quick_select. Select on all nths in range 
  template< class RandomAccessIterator, class RandomAccessIteratorNths >
  constexpr void quick_select(RandomAccessIterator first, RandomAccessIteratorNths nth_first, RandomAccessIteratorNths nth_last, RandomAccessIterator last)
  {
    return quick_select(first, nth_first, nth_last, last, std::less());
  }

  /// quick_medmed_select. Select on all nths in range 
  template< class RandomAccessIterator, class RandomAccessIteratorNths, class Comp>
  constexpr void quick_medmed_select(RandomAccessIterator first, RandomAccessIteratorNths nth_first, RandomAccessIteratorNths nth_last, RandomAccessIterator last, Comp comp)
  {
    return nthexple::quick_medmed_select(std::ranges::subrange(first, last), std::ranges::subrange(nth_first, nth_last), comp);
  }

  /// quick_medmed_select. Select on all nths in range 
  template< class RandomAccessIterator, class RandomAccessIteratorNths >
  constexpr void quick_medmed_select(RandomAccessIterator first, RandomAccessIteratorNths nth_first, RandomAccessIteratorNths nth_last, RandomAccessIterator last)
  {
    return quick_medmed_select(first, nth_first, nth_last, last, std::less());
  }

}
