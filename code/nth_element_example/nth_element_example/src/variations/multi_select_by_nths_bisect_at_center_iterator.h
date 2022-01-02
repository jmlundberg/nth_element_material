#pragma once

#include<algorithm>
#include<ranges>
#include<xutility>


/**
 *
 * This is a variation on nths_bisect found in multi_select_by_nth_bisect
 * but instead of bisecting nths, it's bisecting on the pointer value of nths
 *
 * The idea is that it could be better for skewed nths distributiuons, but in
 * my tests it's slower in all cases.
 *
 */


namespace multi_select_by_nths_bisect_at_center_iterator {

	namespace Current {


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

			class multi_select_fn : private NotQuiteObject {
			public:
				using NotQuiteObject::NotQuiteObject;

				//signature multi_select(R&& r, Nths&& nths, Comp comp = {}, Proj proj = {})
				template<std::ranges::random_access_range R,
					std::ranges::random_access_range Nths,
					class Comp = std::ranges::less, class Proj = std::identity>
					requires std::sortable< std::ranges::iterator_t<R>, Comp, Proj>
					&& std::convertible_to< std::iter_reference_t<std::ranges::iterator_t<Nths>>, std::ranges::iterator_t<R>>
					constexpr std::ranges::borrowed_iterator_t<R>
					operator()(R&& r, Nths&& nths, Comp comp = {}, Proj proj = {}) const
				{
					constexpr auto RevertToSortAt = 64;

					const auto first = std::begin(r);
					const auto last = std::end(r);
					const auto nth_first = std::begin(nths);
					const auto nth_last = std::end(nths);
					if (last - first <= RevertToSortAt) {
						return std::ranges::sort(r, comp, proj);
					}
					const auto nth_relevant_end = std::lower_bound(nth_first, nth_last, last);
					const auto nth_dist = nth_relevant_end - nth_first;
					if (nth_dist == 0 || *nth_first == last) return last;
					const auto vec_mid = *nth_first + (*(nth_relevant_end - 1) - *nth_first) / 2;
					const auto nth_mid = std::lower_bound(nth_first, nth_relevant_end, vec_mid);

					const auto at_nth_mid = *nth_mid;
					(void)std::ranges::nth_element(std::ranges::subrange(first, last), at_nth_mid, comp, proj);
					(void)multi_select_fn::operator()(std::ranges::subrange(first, at_nth_mid), std::ranges::subrange(nth_first, nth_mid), comp, proj);
					if (at_nth_mid != last) {
						// One could revert linear for: nth_last - nth_mid < some constant to get a
						// little less overhead, but in practice so much more work is done in the inner calls to nth_element.
						//   const auto nth_leftA = std::find_if_not(nth_mid, nth_last,
						// 	 [at_nth_mid](auto v) {return v == at_nth_mid; });
						const auto nth_left = std::upper_bound(nth_mid, nth_relevant_end, at_nth_mid);
						(void)multi_select_fn::operator()(std::ranges::subrange(at_nth_mid + 1, last), std::ranges::subrange(nth_left, nth_relevant_end), comp, proj);
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
					return multi_select_fn::operator()(std::ranges::subrange(first, last), nths, comp, proj);
				}
			};
			inline constexpr multi_select_fn multi_select{ NotQuiteObject::_Construct_tag{} };

		}

		template< class RandomAccessIterator, class RandomAccessIteratorNths, class Comp>
		constexpr void multi_select(RandomAccessIterator first, RandomAccessIteratorNths nth_first, RandomAccessIteratorNths nth_last, RandomAccessIterator last, Comp comp)
		{
			return ranges::multi_select(std::ranges::subrange(first, last), std::ranges::subrange(nth_first, nth_last), comp);
		}

		template< class RandomAccessIterator, class RandomAccessIteratorNths >
		constexpr void multi_select(RandomAccessIterator first, RandomAccessIteratorNths nth_first, RandomAccessIteratorNths nth_last, RandomAccessIterator last)
		{
			return multi_select(first, nth_first, nth_last, last, std::less());
		}


	}
}