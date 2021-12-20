
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

namespace nth_elements_timing {
  
static void nthElementsTests_time_nths() {
  const auto outerRounds = 12;
  for (const auto vecSize : { 30,300,3000,30000,300000,3000000,30000000 }) {
    for (unsigned int outerRound = 0 ; outerRound < outerRounds ; ++outerRound) {

      std::knuth_b knuth(0xfeefdedeU + outerRound + outerRounds* 7321 * static_cast< int >( vecSize ));
      std::vector<double> vOrg(vecSize);
      std::iota(vOrg.begin(),vOrg.end(),0.0);
      std::ranges::shuffle( vOrg, knuth);
      auto v = vOrg;
      const auto nCases = 13;
      double avgT = 0.0;
      for ( const auto constantNumQ_i : std::views::iota(0,nCases)) {
 
        int constantNumQ = 1 + constantNumQ_i;
        if (constantNumQ_i > 4) {
          constantNumQ = 10 + 10 * NthElementsUnitTestHelpers::square(constantNumQ_i - 4 - 1) ;
        }
        if (constantNumQ_i > 7) {
          constantNumQ = 100 + 100 * NthElementsUnitTestHelpers::square(constantNumQ_i - 7 - 1);
        }
        if (constantNumQ_i == nCases - 1) {
          constantNumQ = vecSize;
        }

        NthElementsUnitTestHelpers::Timer timer;
        auto totT = 0.0;
        auto totTS = 0.0;
        const auto numSizeTrials = 2;
        size_t innerTrials = 2;
        innerTrials*=(1+(vecSize<1e3))*(1+(vecSize<1e4))*(1+(vecSize<1e5))*(1+(vecSize<1e6));
        double thisTrialMean = 0.0;
        double thisTrialMeanS = 0.0;
        size_t rounds=0;
        size_t roundsS=0;

        const auto numNs = constantNumQ; 
        std::vector<decltype(v)::iterator> ns;

        ns.reserve(numNs);
        if (false) {
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
        auto newEnd = std::ranges::unique( ns ).begin();
        ns.resize(newEnd - ns.begin());
        const auto nths_size = ns.size();

        for (auto sizeTrials = 0; sizeTrials < numSizeTrials; ++sizeTrials) {
          auto  thisTrial = 0.0;
          auto  thisTrialS = 0.0;

          for (auto trials = 0ULL; trials < innerTrials; ++trials) {
            std::ranges::copy( vOrg,v.begin());

            if (trials % 2 == outerRound % 2)
            {
              const auto t0 = timer.tock_ns();
              //P2375_nth_element::R0::nth_elements(v.begin(), ns.begin(), ns.end(), v.end());
              P2375_nth_element::Current::ranges::nth_elements(v, ns);
              //P2375_nth_element::Current::ranges::nth_elements(v.begin(), ns, v.end());
              const auto t1 = timer.tock_ns();
              thisTrial += (t1 - t0) * 1.0;
              ++rounds;
            }
            else
            {
              const auto t0 = timer.tock_ns();
              std::sort(v.begin(), v.end());
              const auto t1 = timer.tock_ns();
              thisTrialS += (t1 - t0) * 1.0;
              ++roundsS;
            }

          }
          thisTrialMean += thisTrial;
          thisTrialMeanS += thisTrialS;

        }
        if(rounds*2!=numSizeTrials*innerTrials || rounds != roundsS){
          std::cerr << "err" <<std::endl;
          exit(2);
        }
        thisTrialMean *= 1.0 / rounds;
        totT += thisTrialMean;
        thisTrialMeanS *= 1.0 / roundsS;
        totTS += thisTrialMeanS;
        avgT += totT * 1e-9;
        NthElementsUnitTestHelpers::printJsonLine(std::cout, outerRound, vecSize, constantNumQ, nths_size, totTS / totT, log(vecSize) / log(nths_size * 1.0), totT * 1e-9, avgT / (1.0 + constantNumQ_i));
        
      }
    }
  }
}
}

#if __has_include(<gtest/gtest.h>)
  #include <gtest/gtest.h>
  struct NthElementsTest : testing::Test {};
  TEST_F(NthElementsTest,time_nths_test){
    nth_elements_timing::nthElementsTests_time_nths();
  }
#else
int main()
{
  nth_elements_timing::nthElementsTests_time_nths();
}
#endif


