
Johan Lundberg 2022-01-08

## Implementations of multi selection, single selection

### Purpose and status

These are simple C++ implementations of multi select, single select as
discussed and described in [../../../README.md](../../../README.md) and 
https://github.com/jmlundberg/p2375 : Generalisation of nth_element to a range of nths.

Most of this stuff is common knowledge since long, eg 
ninther by Tukey, 1978 (Check for example: 1978, Engineering a Sort Function. Bentley and McILROY) and Musser, David R. (1997). "Introspective Sorting and Selection Algorithms".

The implementations in this repo were made as reference implementation for *multiple* select, and as a learning exercise and history lesson on sorting and selection. You should likely not use them for anything else.

The implementations are bare and quite direct translations of the basic ideas in literature without any special tricks or improvement attempts. Notably, I'm also sticking with plain recursion for simplicity instead of loop based formulations. 
All algorithms including pivot-finding are completely in-place and the only required extra space is the call stack with a few range views, iterators, comp and proj. 

The interfaces are `ranges`-like, similar to https://en.cppreference.com/w/cpp/algorithm/ranges/nth_element, with wrappers for direct use with iterators. Follows the same [niebloid](https://stackoverflow.com/questions/62928396/what-is-a-niebloid) pattern as MS STL for ADL protection.

Some things I've not bothered to do: - To std::forward arguments where I don't know that it matters. - To use std::invoke instead of direct calls to comp and proj. To use constexpr. The code also mixes similar ways to do things (ex declaration syntax) a bit, eg declarations use
abbreviated concepts syntax sometimes. It does currently not support move-only types due to how I call `std::ranges::partition`.

The code should work with all random-access ranges. 

### Dependencies and Building

Header only and no external dependencies. The visual studio solution requires /std:c++latest for `<ranges>`.

### Content

#### [include/](include/)

The algorithms. Notably 

* selection_policies - Implements pivoting and fallback strategies.
* single_quick_select - Implements single nth nth_element (selection) using any of the strategies.
* multi_quick_select - Implements multi nth nth_element (multi selection) using any of the strategies. 

#### [src/tests/](src/tests/)

Some unit test and timing test. Tests use bundled googletest, built with the vs project.

#### [src/ensure_compile/](src/ensure_compile/)

Some additional .cpp files that only include a single header to ensure independent compilation.

