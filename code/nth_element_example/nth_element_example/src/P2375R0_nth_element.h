#pragma once

#include<algorithm>

namespace P2375_nth_element {
  namespace R0 {

		/// Implementation of P2375R0, range-of-nths overload for nth_element. Here called 'nth_elements'
		template< class RandomAccessIterator, class RandomAccessIterator2 >
		constexpr void nth_elements(RandomAccessIterator first, RandomAccessIterator2 nth_first, RandomAccessIterator2 nth_last, RandomAccessIterator last)
		{
			if (last - first <= 64) {
				std::sort(first, last);
				return;
			}
			const auto nth_dist = nth_last - nth_first;
			if (nth_dist == 0 || *nth_first == last) return;
			const auto nth_mid = nth_first + nth_dist / 2;
			const auto at_nth_mid = *nth_mid;
			std::nth_element(first, at_nth_mid, last);
			nth_elements(first, nth_first, nth_mid, at_nth_mid);
			if (at_nth_mid != last) {
				const auto nth_left = std::find_if_not(nth_mid, nth_last,
					[at_nth_mid](auto v) {return v == at_nth_mid; });
				nth_elements(at_nth_mid + 1, nth_left, nth_last, last);
			}
		}

  }
}