<div id="Nth_element_p3275">

## Material on nth_element and P3275
</div>


This repo contains a simple implementation, a basics performance study, examples and
background material on
https://github.com/jmlundberg/p2375 :
 Generalisation of nth_element to a range of nths

#### Performance and complexity of nth_element vs std::sort

The single-nth
[`std::nth_element`](https://en.cppreference.com/w/cpp/algorithm/nth_element) has linear complexity: O(N), compared to O(N log N) of `std::sort`.

The range-of-nths overload proposed in P2375 has complexity: O(N log m) where m is the number of unique partitioning points.


#### Reference implementation

A simple implementation of the range-of-nths version of `nth_element` is found at
[code/nth_element_example/nth_element_example/src/P2375R0_nth_element.h](code/nth_element_example/nth_element_example/src/P2375R0_nth_element.h)

#### Measurement on a basic implementation

In practice, `nth-element` can be many times faster than a full `std::sort`.

More details at [performance.md](PERFORMANCE.md)


#### Example: Histogram / Data / Image Equalization

Found at [plotting/examples/partition_based_image_equalization.md](plotting/examples/partition_based_image_equalization.md)

Orgininal vs image equalization using `range-of-nths`, `m=5`

<img alt="alt_text" width="38%" src="plotting/examples/out_small/forsen_roundtrip.small.jpg?raw=true" /> <img alt="alt_text" width="38%" src="plotting/examples/out_small/forsen_partition5.small.jpg?raw=true" />
