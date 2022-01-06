/// Ensures independent compilation

#include "single_quick_select.h"

#include <vector>
#include <memory>

namespace {

  void build_test()
  {
    std::vector<int> v(20);
    auto b = v.begin();
    auto nth = b;

    nthexple::ranges::quick_select(v, nth);
    nthexple::ranges::quick_medmed_select(v, nth);;
    nthexple::ranges::intro_select(v, nth);

  }
}