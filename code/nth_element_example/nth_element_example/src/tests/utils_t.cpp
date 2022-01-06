/*
 Johan Lundberg 2022-01-08
*/

#include <random>
#include <chrono>
#include <iostream>
#include <ranges>

#include <format>
#include <iostream>
#include <numeric>
#include <string>

#include "unit_test_helpers.h"

#include <gtest/gtest.h>
#include "median_of_medians.h"

namespace utils {

  struct medianOfMediansBasics : testing::Test {};
  TEST_F(medianOfMediansBasics, TimeAndCenterFraction) {
    for (const auto vecSizeNom : { 1,2,5,32,67,323,2221, 216'091 })
    {
      for (int round = 0; round < 6; ++round) {
        std::knuth_b knuth(0xfeefdedeU + 7321 * static_cast<int>(vecSizeNom)+99931321*round);
        const auto vecSize = vecSizeNom *(1 + round) + (round * 3'021'377) % 263;
        const auto delta = sqrt(vecSize +1 )/vecSize ; 
        std::vector<double> vOrg(vecSize);
        std::iota(vOrg.begin(), vOrg.end(), 0.0);
        std::ranges::shuffle(vOrg, knuth);
        auto v = vOrg;
        auto v2 = vOrg;

        auto m2 = nthexple::medianOfMedians(v2, std::less(), std::identity());
        const auto result = *m2;
        auto vMid = v.begin() + v.size() / 2;
        //(void)nthexple::ranges::intro_select(v, vMid);
        (void) std::ranges::nth_element(v, vMid);
        const auto truth = *vMid;

        const auto frac = truth * 1.0 / vecSize;
        EXPECT_GT(frac, 0.5 - delta);
        EXPECT_LT(frac, 0.5 + delta);
        unit_test_helpers::printSpaceLine(std::cout, truth, result, "frac:", frac);
      }
    }
  }

}

