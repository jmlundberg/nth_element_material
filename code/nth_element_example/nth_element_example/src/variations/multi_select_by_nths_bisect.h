#pragma once

#pragma once

#include<algorithm>
#include<ranges>
#include<xutility>

/**
 *
 * Multi select implemented as bisection of nths
 *
 * The algorithm has expected O(N log m) complexity, m is the number of unique elements in nths
 *
 * Basic outline and analysis of the algorithm (but with no repeated nths) is found at
 *
 * Alsuwaiyel2001] Muhammad H. Alsuwaiyel: An optimal parallel algorithm
 * for the multiselection problem. Parallel Computing Volume 27, Issue 6, May 2001, Pages 861-865
 * https://doi.org/10.1016/S0167-8191(00)00095-8
 *
 * This implementation is a slight extension to allow repeated nth.
 *
 */

namespace multi_select_by_nth_bisect {

  
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

    class Nth_elements_fn : private NotQuiteObject {
    public:
      using NotQuiteObject::NotQuiteObject;

      //signature multi_select(R&& r, Nths&& nths, Comp comp = {}, Proj proj = {})
      template<std::ranges::random_access_range R,
        std::ranges::random_access_range Nths,
        class Comp = std::ranges::less, class Proj = std::identity>
        requires std::sortable< std::ranges::iterator_t<R>, Comp, Proj>
        && std::convertible_to< std::iter_reference_t<std::ranges::iterator_t<Nths>>, std::ranges::iterator_t<R>>
        constexpr std::ranges::borrowed_iterator_t<R>
        operator()(R&& r, Nths&& nths, Comp comp, Proj proj) const
      {
        constexpr auto RevertToSortAt = 64;

        const auto first = std::begin(r);
        const auto last = std::end(r);
        const auto nth_first = std::begin(nths);
        const auto nth_last = std::end(nths); 
        if (last - first <= RevertToSortAt) {
          // A similar case is found in existing single nth  nth_element, 
          //but here it also prevents
          // unnecessary bisections of equal and near equal nths.
          return std::ranges::sort(r, comp, proj);
        }
        const auto nth_dist = nth_last - nth_first;
        if (nth_dist == 0 || *nth_first == last) return last;
        const auto nth_mid = nth_first + nth_dist / 2;
        const auto at_nth_mid = *nth_mid;
        (void)std::ranges::nth_element(r, at_nth_mid, comp, proj);
        (void)Nth_elements_fn::operator()(std::ranges::subrange(first, at_nth_mid), std::ranges::subrange(nth_first, nth_mid), comp, proj);
        if (at_nth_mid != last) {
          // One could revert linear for: nth_last - nth_mid < some constant to get a
          // little less overhead, but in practice so much more work is done in the inner calls to nth_element.
          //   const auto nth_leftA = std::find_if_not(nth_mid, nth_last,
          //    [at_nth_mid](auto v) {return v == at_nth_mid; });
          const auto nth_left = std::upper_bound(nth_mid, nth_last, at_nth_mid);
          (void)Nth_elements_fn::operator()(std::ranges::subrange(at_nth_mid + 1, last), std::ranges::subrange(nth_left, nth_last), comp, proj);
        }
        return last;
      }

      //Signature multi_select(I first, Nths&& nths, S last, Comp comp = {}, Proj proj = {})
      template<std::random_access_iterator I, std::sentinel_for<I> S,
        std::ranges::random_access_range Nths, class Comp = std::ranges::less, class Proj = std::identity>
        requires std::sortable<I, Comp, Proj>
        && std::convertible_to<std::iter_reference_t<std::ranges::iterator_t<Nths>>, I>
        constexpr I
        operator()(I first, Nths&& nths, S last, Comp comp = {}, Proj proj = {}) const
      {
        return Nth_elements_fn::operator()(std::ranges::subrange(first, last), nths, comp, proj);
      }
    };
    inline constexpr multi_select_by_nth_bisect::ranges::Nth_elements_fn multi_select{ NotQuiteObject::_Construct_tag{} };

  }

  template< class RandomAccessIterator, class RandomAccessIteratorNths, class Comp>
  constexpr void multi_select(RandomAccessIterator first, RandomAccessIteratorNths nth_first, RandomAccessIteratorNths nth_last, RandomAccessIterator last, Comp comp)
  {
    return ranges::multi_select(std::ranges::subrange(first, last), std::ranges::subrange(nth_first, nth_last), comp);
  }

  template< class RandomAccessIterator, class RandomAccessIteratorNths >
  constexpr void multi_select(RandomAccessIterator first, RandomAccessIteratorNths nth_first, RandomAccessIteratorNths nth_last, RandomAccessIterator last)
  {
    return multi_select(first, nth_first, nth_last, last, std::less<>());
  }


}
