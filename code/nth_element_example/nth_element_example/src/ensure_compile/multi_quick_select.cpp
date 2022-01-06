/// Ensures independent compilation
#include "multi_quick_select.h"
#include <vector>
#include <memory>
namespace {

  void build_test()
  {
    std::vector<int> v(20);
    auto b = v.begin();
    auto nths = std::vector{ b,b + 2,b + 5 };

    nthexple::ranges::multi_quick_select(v, nths);
    nthexple::ranges::multi_logintro_select(v, nths);
    nthexple::ranges::multi_intro_select(v, nths);

  }

}