

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

#include <gtest/gtest.h>

namespace multi_select_timing {

	struct NthElementsTest : testing::Test {};
	TEST_F(NthElementsTest, time_nths_test) {
		constexpr auto quickRelativeTest = false;
		constexpr auto quickRelativeTestRounds = 2;
		constexpr auto doRandomIters = false;

#define multi_select_under_test   quickselect_nth_element::ranges::multi_quick_select

		constexpr auto fakeSortTime = quickRelativeTest;
		for (const auto vecSize : { 30,300,3000,30'000,300'000,3'000'000,30'000'000 }) {
			const auto nominalOuterRounds = 12; // vecSize < 10'000'000U ? 12U : 3U;
			const auto outerRounds = quickRelativeTest ? quickRelativeTestRounds : nominalOuterRounds;

			for (unsigned int outerRound = 0; outerRound < outerRounds; ++outerRound) {

				std::knuth_b knuth(0xfeefdedeU + outerRound + outerRounds * 7321 * static_cast<int>(vecSize));
				std::vector<double> vOrg(vecSize);
				std::iota(vOrg.begin(), vOrg.end(), 0.0);
				std::ranges::shuffle(vOrg, knuth);
				auto v = vOrg;
				const auto nCases = 13;
				double avgT = 0.0;
				for (const auto constantNumQ_i : std::views::iota(0, nCases)) {

					int constantNumQ = 1 + constantNumQ_i;
					if (constantNumQ_i > 4) {
						constantNumQ = 10 + 10 * unit_test_helpers::square(constantNumQ_i - 4 - 1);
					}
					if (constantNumQ_i > 7) {
						constantNumQ = 100 + 100 * unit_test_helpers::square(constantNumQ_i - 7 - 1);
					}
					if (constantNumQ_i == nCases - 1) {
						constantNumQ = vecSize;
					}

					unit_test_helpers::Timer timer;
					auto totT = 0.0;
					auto totTS = 0.0;
					const auto numSizeTrialsNominal = 2;
					size_t innerTrials = 2;
					innerTrials *= (1 + (vecSize < 1e3)) * (1 + (vecSize < 1e4)) * (1 + (vecSize < 1e5)) * (1 + (vecSize < 1e6));
					double thisTrialMean = 0.0;
					double thisTrialMeanS = 0.0;
					size_t rounds = 0;
					size_t roundsS = 0;

					const auto numNs = constantNumQ;
					std::vector<decltype(v)::iterator> ns;

					ns.reserve(numNs);
					if (doRandomIters) {
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

					std::sort(ns.begin(), ns.end());
					auto newEnd = std::ranges::unique(ns).begin();
					ns.resize(newEnd - ns.begin());
					const auto nths_size = ns.size();
					const auto numSizeTrials = 2; // numSizeTrialsNominal* (nths_size < 3 ? 4 : 1);

					for (auto sizeTrials = 0; sizeTrials < numSizeTrials; ++sizeTrials) {
						auto  thisTrial = 0.0;
						auto  thisTrialS = 0.0;
						auto  thisTrialS_N = 0;

						for (auto trials = 0ULL; trials < innerTrials; ++trials) {
							std::ranges::copy(vOrg, v.begin());
							if (trials % 2 == outerRound % 2)
							{
								const auto t0 = timer.tock_ns();
								multi_select_under_test(v, ns);
								const auto t1 = timer.tock_ns();
								thisTrial += (t1 - t0) * 1.0;
								++rounds;
							}
							else
							{
								if constexpr (fakeSortTime) {
									const auto faketime = (vecSize * log(vecSize * 1.0) * (2458.85 / 516501238)) / 1e-6;
									thisTrialS += faketime;
								}
								else {
									//if (thisTrialS_N <1) {
									const auto t0 = timer.tock_ns();
									std::sort(v.begin(), v.end());
									const auto t1 = timer.tock_ns();
									const auto tsort = (t1 - t0) * 1.0;
									thisTrialS += tsort;
									// }else{
									 //  thisTrialS += thisTrialS / thisTrialS_N;
									// }
								}
								++roundsS;
								++thisTrialS_N;
							}

						}
						thisTrialMean += thisTrial;
						thisTrialMeanS += thisTrialS;

					}
					if (rounds * 2 != numSizeTrials * innerTrials || rounds != roundsS) {
						exit(2);
					}
					thisTrialMean *= 1.0 / rounds;
					totT += thisTrialMean;
					thisTrialMeanS *= 1.0 / roundsS;
					totTS += thisTrialMeanS;
					avgT += totT * 1e-9;
					unit_test_helpers::printJsonLine(std::cout, outerRound, vecSize, constantNumQ, nths_size, totTS / totT, log(vecSize) / log(nths_size * 1.0), totT * 1e-9, avgT / (1.0 + constantNumQ_i));

				}
			}
		}
	}
}


