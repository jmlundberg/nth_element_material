
#include "P2375_nth_element.h"

#include <random>
#include <chrono>
#include <iostream>
#include <ranges>

#include <format>
#include <iostream>
#include <numeric>
#include <string>

#include "unit_test_helpers.h"

namespace nth_elements_basics {



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
			ASSERT_TRUE(okHere);
			if (!okHere) {
				std::terminate();
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
					ASSERT_TRUE(okHere);
					if (!okHere) {
						std::terminate();
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
					ASSERT_TRUE(okHere);
					if (!okHere) {
						std::terminate();
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
	static void nthElementsTests_correct(bool uniformNths) {

		constexpr auto thisProj = [](auto v) {
			return abs(741.22 - abs(v - 3.31));
		};
		constexpr auto thisCompare = [](auto v, auto other) {
			return abs(other - 15.31) < abs(v - 15.31);
		};
		const auto checkAllUntil = 11137 / 10;
		const auto maxNcheck = 11137 * 100;
		const auto outerRounds = 1;

		std::vector<size_t> vecSizes;
		auto vz = size_t(0);
		for (; vz < checkAllUntil; ++vz) {
			vecSizes.push_back(vz);
		}
		for (; vz < maxNcheck; vz += vz / 4 + 2) {
			vecSizes.push_back(vz);
		}
		for (const auto vecSize : vecSizes) {
			std::cout << vecSize << std::endl;
			size_t extraReruns = 1;
			extraReruns *= 1; // (1 + (vecSize < 1e3))* (1 + (vecSize < 1e4))* (1 + (vecSize < 1e5))* (1 + (vecSize < 1e6));

			for (unsigned int outerRound = 0; outerRound < outerRounds * extraReruns; ++outerRound) {

				std::knuth_b knuth(0xfeefdedeU + outerRound + outerRounds * 7321 * static_cast<int>(vecSize));
				std::vector<double> vOrg(vecSize);
				std::iota(vOrg.begin(), vOrg.end(), 0.0);
				std::ranges::shuffle(vOrg, knuth);
				auto v = vOrg;
				auto vOrgSorted = vOrg;
				std::ranges::sort(vOrgSorted, thisCompare, thisProj);
				const auto nCases = 13;
				for (const auto constantNumQ_i : std::views::iota(0, nCases)) {

					size_t constantNumQ = 1 + constantNumQ_i;
					if (constantNumQ_i > 4) {
						constantNumQ = 10 + 10 * NthElementsUnitTestHelpers::square(constantNumQ_i - 4 - 1);
					}
					if (constantNumQ_i > 7) {
						constantNumQ = 100 + 100 * NthElementsUnitTestHelpers::square(constantNumQ_i - 7 - 1);
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
					const auto nths_size = ns.size();

					{
						std::ranges::copy(vOrg, v.begin());
						//P2375_nth_element::R0::nth_elements(v.begin(), ns.begin(), ns.end(), v.end());
						P2375_nth_element::Current::ranges::nth_elements(v, ns, thisCompare, thisProj);
						//P2375_nth_element::Current::ranges::nth_elements(v.begin(), ns, v.end());

						checkNthElements(v, vOrgSorted, ns, thisCompare, thisProj);

					}
				}
			}
		}
	}
}

#if __has_include(<gtest/gtest.h>)
#include <gtest/gtest.h>
struct NthElementsTestBasics : testing::Test {};
TEST_F(NthElementsTestBasics, basics_uniform) {
	nth_elements_basics::nthElementsTests_correct(true);
}
TEST_F(NthElementsTestBasics, basics_random) {
	nth_elements_basics::nthElementsTests_correct(false);
}
#else
int main()
{
	nth_elements_timing::nthElementsTests_time_nths();
}
#endif


