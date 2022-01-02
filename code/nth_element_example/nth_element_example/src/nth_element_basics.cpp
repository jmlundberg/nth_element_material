
#include <random>
#include <chrono>
#include <iostream>
#include <ranges>

#include <format>
#include <iostream>
#include <numeric>
#include <string>

#include "unit_test_helpers.h"
#include "variations/multi_quick_select.h"
#include "variations/multi_select_by_nths_bisect.h"
#include <gtest/gtest.h>

namespace multi_select_basics {


#define multi_select_under_test   quickselect_nth_element::ranges::multi_quick_select

	template<class Ns, class Comp = std::ranges::less, class Proj = std::identity>
	static void checkNthElements(const std::vector<double>& v, const std::vector<double>& vOrgSorted, Ns& ns, Comp comp = {}, Proj proj = {})
	{
		{ // basic corruption check: no lost elements
			auto vCopy = v;
			std::ranges::sort(vCopy, comp, proj);
			auto compEqual = [comp](auto a, auto b) {
				return !comp(a, b) && !comp(b, a);
			};
			auto okHere = std::ranges::equal(vCopy, vOrgSorted, compEqual, proj, proj);
			//ASSERT_TRUE(okHere);
			if (!okHere) {
				throw std::runtime_error("blarg0");
			  //std::terminate();
			}
		}
		{ // basic nth_element check: equality at nths
			const auto vBegin = v.begin();
			for (const auto nth : ns) {
				if (nth != v.end()) {
					const auto projNth = proj(*nth);
					const auto projFacit = proj(vOrgSorted[static_cast<size_t>(nth - vBegin)]);
					//ASSERT_EQ(projNth, projFacit);
					const auto okHere = !comp(projNth, projFacit) && !comp(projFacit, projNth);
					//ASSERT_TRUE(okHere);
					if (!okHere) {
						throw std::runtime_error("blarg1");
					}
				}
			}
		}
		{ // elements after nth must not be less
			for (auto nth_i = size_t(0); nth_i < ns.size(); ++nth_i) {
				const auto nth = ns[nth_i];
				const auto until = nth_i + 1 != ns.size() ? ns[nth_i + 1] : v.end();
				for (auto at = nth; at != until; ++at) {
					const auto okHere = !(comp(proj(*at), proj(*nth)));
					//ASSERT_TRUE(okHere);
					if (!okHere) {
						throw std::runtime_error("blarg2");
					}
				}
			}
			/* same but slower
			for (const auto nth : ns) {
				for (auto after = nth; after != v.end(); ++after) {
					ASSERT_TRUE(!(*after < *nth));
				}
			}
			*/
		}

	}
	constexpr auto singleNthTest = false;

	static void nthElementsTests_correct(bool uniformNths, bool justShuffle) {


    #ifndef SKIP_PROJ_TEST
		constexpr auto thisProj = [](auto v) {
			return abs(741.22 - abs(v - 3.31));
		};
		constexpr auto thisCompare = [](auto v, auto other) {
			return abs(other - 15.31) < abs(v - 15.31);
		};
    #else
		constexpr auto thisProj = std::identity();
		constexpr auto thisCompare = std::less();
    #endif

	  const auto checkAllUntil = 11137 / 10;
    #ifndef NDEBUG
		const auto maxNcheck = 11137 * 10;
    #else
		const auto maxNcheck = 11137 * 100;
    #endif
		const auto outerRounds = 1;

		std::vector<size_t> vecSizes;
		auto vz = size_t(0);
		for (; vz < checkAllUntil; ++vz) {
			vecSizes.push_back(vz);
		}
		for (; vz < maxNcheck; vz += vz / 2 + 2) {
			vecSizes.push_back(vz);
		}
		for (const auto vecSize : vecSizes) {
			std::cout << vecSize << std::endl;
			size_t extraReruns = 1;
			extraReruns *= 1; // (1 + (vecSize < 1e3))* (1 + (vecSize < 1e4))* (1 + (vecSize < 1e5))* (1 + (vecSize < 1e6));

			for (unsigned int outerRound = 0; outerRound < outerRounds * extraReruns; ++outerRound) {

				std::knuth_b knuth(0xfeefdedeU + outerRound + outerRounds * 7321 * static_cast<int>(vecSize));
				std::knuth_b knuth2(0xfeefdedeU + outerRound + outerRounds * 7321 * static_cast<int>(vecSize));
				std::vector<double> vOrg(vecSize);
				if (justShuffle) {
					std::iota(vOrg.begin(), vOrg.end(), 0.0);
					std::ranges::shuffle(vOrg, knuth);
				}else{
					std::exponential_distribution<double> ex(1/(vecSize*1.0+0.1));
					for (auto& item : vOrg) {
						double v = -1;
						while (v<0 || v>vecSize){
							v = static_cast< int >( ex( knuth ) )*1.0;
						}
						item = v;
					}
				}
				auto v = vOrg;
				auto vOrgSorted = vOrg;
				std::ranges::sort(vOrgSorted, thisCompare, thisProj);
				//std::ranges::sort(vOrgSorted, thisCompare, thisProj);
				const auto nCases = 13;
				for (const auto constantNumQ_i : std::views::iota(0, nCases)) {

					size_t constantNumQ = 1 + constantNumQ_i;
					if (constantNumQ_i > 4) {
						constantNumQ = 10 + 10 * unit_test_helpers::square(constantNumQ_i - 4 - 1);
					}
					if (constantNumQ_i > 7) {
						constantNumQ = 100 + 100 * unit_test_helpers::square(constantNumQ_i - 7 - 1);
					}
					if (constantNumQ_i == nCases - 1) {
						constantNumQ = vecSize;
					}

					const auto numNs = constantNumQ;
					std::vector<decltype(v)::iterator> ns;

					ns.reserve(numNs);
					if (uniformNths) {
						const auto uniformInV = std::uniform_int<size_t>(0, v.size());
						for (auto n = 0; n < numNs; ++n) {

							auto vi = uniformInV(knuth);
							ns.push_back(v.begin() + vi);
						}
					}
					else {
						for (auto n = 0; n < numNs; ++n) {
							const auto vi = (v.size() * (n + 1)) / (numNs + 1);
							ns.push_back(v.begin() + vi);
						}
					}

					std::ranges::sort(ns);
					auto newEnd = std::ranges::unique(ns).begin();
					ns.resize(newEnd - ns.begin());
					if(singleNthTest){
						if (ns.size() < 1) continue;
						std::ranges::shuffle(ns, knuth2);
						ns.resize(1);
						//std::cout << ns[0]-v.begin() << std::endl;
					}
					const auto nths_size = ns.size();

					{
						std::ranges::copy(vOrg, v.begin());
						multi_select_under_test(v, ns, thisCompare, thisProj);

						checkNthElements(v, vOrgSorted, ns, thisCompare, thisProj);

					}
				}
			}
		}
	}

	static void nthElementsTests_dangle() {
		auto v = std::vector<int>(10, 10);
		const std::vector nths{ v.begin() + 1, v.begin() + 2 };
		auto e = multi_select_under_test(v, nths, std::less(), std::identity());
		static_assert(!std::is_same_v<decltype(e), std::ranges::dangling>);
		auto dangle = multi_select_under_test(std::move(v), nths, std::less(), std::identity());
		static_assert(std::is_same_v<decltype(dangle),std::ranges::dangling>);
	}

	struct NthElementsTestBasics : testing::Test {};
	TEST_F(NthElementsTestBasics, basics_uniform_shuf) {
		multi_select_basics::nthElementsTests_correct(true, true);
	}
	TEST_F(NthElementsTestBasics, basics_uniform) {
		multi_select_basics::nthElementsTests_correct(true, false);
	}
	TEST_F(NthElementsTestBasics, basics_random_shuf) {
		multi_select_basics::nthElementsTests_correct(false, true);
	}
	TEST_F(NthElementsTestBasics, basics_random) {
		multi_select_basics::nthElementsTests_correct(true, false);
	}
	TEST_F(NthElementsTestBasics, dangle) {
		multi_select_basics::nthElementsTests_dangle();
	}

}




