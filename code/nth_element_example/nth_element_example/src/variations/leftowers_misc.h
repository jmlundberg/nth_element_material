#error("nah")

/*
 * misc text. has errors.
 
        template<std::ranges::random_access_range R,
          class Comp = std::ranges::less, class Proj = std::identity>
          requires std::sortable< std::ranges::iterator_t<R>, Comp, Proj>
          static constexpr std::ranges::borrowed_iterator_t<R>
          basicQuickSort(R&& r, Comp comp = {}, Proj proj = {}) {

          const auto first = std::begin(r);
          const auto last = std::end(r);
          if (std::distance(first, last) < 2) {
            return last;
          }
          const auto pivot0 = uncheckedMidpointPivot(first, last, comp,proj);
          const auto projPivot = proj(pivot0);
          const auto rightPartRange = std::ranges::partition(r, [projPivot, comp](const auto& elem) { return comp(elem, projPivot); }, proj);
          basicQuickSort(std::ranges::subrange(first, rightPartRange.begin()), comp, proj);
          const auto rightPartRange2 = std::ranges::partition(rightPartRange, [projPivot, comp](const auto& elem) { return !comp(projPivot, elem); }, proj);
          basicQuickSort(rightPartRange2, comp, proj);
          return last;
        }

        template<std::ranges::random_access_range R,
          class Comp = std::ranges::less, class Proj = std::identity>
          requires std::sortable< std::ranges::iterator_t<R>, Comp, Proj>
          static constexpr std::ranges::borrowed_iterator_t<R>
          basicQuickSelect(R&& r, std::ranges::iterator_t<R> nth, Comp comp = {}, Proj proj = {}) {

          const auto first = std::begin(r);
          const auto last = std::end(r);
          if (std::distance(first, last) < 2) {
            return last;
          }
          const auto pivot0 = uncheckedMidpointPivot(first, last, comp, proj);
          const auto projPivot = proj(pivot0);
          const auto rightPartRange = std::ranges::partition(r, [projPivot, comp](const auto& elem) { return comp(elem, projPivot); }, proj);
          if (nth < rightPartRange.begin()) {
            basicQuickSelect(std::ranges::subrange(first, rightPartRange.begin()), nth, comp, proj);
          }
          else if (rightPartRange.begin() <= nth) { .. . . . .
            const auto rightPartRange2 = std::ranges::partition(rightPartRange, [projPivot, comp](const auto& elem) { return !comp(projPivot, elem); }, proj);
            if (rightPartRange2.begin() <= nth) {
              basicQuickSelect(rightPartRange2, nth, comp, proj);
            }
          }
          return last;
        }
 
 */