#pragma once

#include<algorithm>
#include<ranges>
#include<xutility>

namespace P2375_nth_element {
	namespace R0 {

		/// Implementation of P2375R0, range-of-nths overload for nth_element. Here called 'nth_elements'
		///	lacks "comp" which is part of the proposal. No range version.
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
	// 
	namespace Current{

		template< class RandomAccessIterator, class RandomAccessIterator2 , class Comp>
		constexpr void nth_elements(RandomAccessIterator first, RandomAccessIterator2 nth_first, RandomAccessIterator2 nth_last, RandomAccessIterator last, Comp comp)
		{
			if (last - first <= 64) {
				std::sort(first, last, comp);
				return;
			}
			const auto nth_dist = nth_last - nth_first;
			if (nth_dist == 0 || *nth_first == last) return;
			const auto nth_mid = nth_first + nth_dist / 2;
			const auto at_nth_mid = *nth_mid;
			std::nth_element(first, at_nth_mid, last, comp);
			nth_elements(first, nth_first, nth_mid, at_nth_mid, comp);
			if (at_nth_mid != last) {
				const auto nth_left = std::find_if_not(nth_mid, nth_last,
					[at_nth_mid](auto v) {return v == at_nth_mid; });
				nth_elements(at_nth_mid + 1, nth_left, nth_last, last, comp);
			}
		}

		template< class RandomAccessIterator, class RandomAccessIterator2 >
		constexpr void nth_elements(RandomAccessIterator first, RandomAccessIterator2 nth_first, RandomAccessIterator2 nth_last, RandomAccessIterator last)
		{
			return Current::nth_elements(first, nth_first, nth_last, last, std::less<>());
		}

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

				//signature nth_elements(R&& r, R2&& nths, Comp comp = {}, Proj proj = {})
				template<std::ranges::random_access_range R,
					std::ranges::random_access_range R2,
					class Comp = std::ranges::less, class Proj = std::identity>
					requires std::sortable< std::ranges::iterator_t<R>, Comp, Proj>
					constexpr std::ranges::borrowed_iterator_t<R>
					operator()(R&& r, R2&& nths, Comp comp = {}, Proj proj = {}) const
				{
					const auto first = std::begin(r);
					const auto last = std::end(r);
					const auto nth_first = std::begin(nths);
					const auto nth_last = std::end(nths);
					if (last - first <= 64) {
						return std::ranges::sort(r, comp, proj);
					}
					const auto nth_dist = nth_last - nth_first;
					if (nth_dist == 0 || *nth_first == last) return last;
					const auto nth_mid = nth_first + nth_dist / 2;
					const auto at_nth_mid = *nth_mid;
					(void) std::ranges::nth_element(std::ranges::subrange(first, last), at_nth_mid, comp, proj);
					(void) Nth_elements_fn::operator()(std::ranges::subrange(first, at_nth_mid), std::ranges::subrange(nth_first, nth_mid), comp, proj);
					if (at_nth_mid != last) {
						const auto nth_left = std::find_if_not(nth_mid, nth_last,
							[at_nth_mid](auto v) {return v == at_nth_mid; });
						(void) Nth_elements_fn::operator()(std::ranges::subrange(at_nth_mid + 1, last), std::ranges::subrange(nth_left, nth_last), comp, proj);
					}
					return last;
				}

				//Signature nth_elements(I first, R2&& nths, S last, Comp comp = {}, Proj proj = {})
				template<std::random_access_iterator I, std::sentinel_for<I> S,
					std::ranges::random_access_range R2, class Comp = std::ranges::less, class Proj = std::identity>
					requires std::sortable<I, Comp, Proj>
					constexpr I
					operator()(I first, R2&& nths, S last, Comp comp = {}, Proj proj = {}) const
				{
					return Nth_elements_fn::operator()(std::ranges::subrange(first, last), nths, comp, proj);
				}
			};
			inline constexpr Nth_elements_fn nth_elements{ NotQuiteObject::_Construct_tag{} };

		}



  }
}