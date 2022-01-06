/*
 Johan Lundberg 2022-01-08
*/
#pragma once
#include <ranges>
#include <cmath>

namespace nthexple {

  static const auto& uncheckedNintherPivot(std::ranges::random_access_range auto&& r, auto comp, auto proj);

  static const auto& medianOfMediansRef(std::ranges::random_access_range auto&& r, auto comp, auto proj);
}