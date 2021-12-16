<div id="Nth_element_p3275">

#### Performance of reference implementation of range-of-nths version of `nth_element`

Back to [github material_on_nth_element/README.md](README.md)

Or to proposal at https://github.com/jmlundberg/p2375

## Time vs `std::sort` for equidistant partitioning points

[<img alt="alt_text" width="68%" src="plotting/images/partition_points_equidistant_speed_for_n_vs_sort.png" />](plotting/images/partition_points_equidistant_speed_for_n_vs_sort.png)

The above image shows the execution speed of `nth_element` compared to `std::sort` for various vector sizes `N`.
The data to sort here consists of randomly shuffled doubles, and the range-of-nths version of `nth_element` was given different numbers `m` of evenly spaced partitioning points in the vector. Dotted lines show lines `k_i × log(N)` fitted to pass though each curve at `N=3e6`.

The image shows that the speed curves approximately follow the expected `log(N)` shape, with different factors for different `m`.

Note: Some care on interpreting the values at low N: For example at the curve `m=1000`. `N=300`, the actual number of partitioning points are saturated to `N`, that is `300` unique partitioning points.

Still, one can say, that as `N` decreases and approaches `m` the performance is worse than `std::sort`, but the speed in these tests are still about `85%` of `std::sort` even for `m ~ N`. The benefit of `nth_element` over `sort` grows with `N` as `log N`.

With `m = 1` partitioning points the speed of `nth_element` is about `10` times that of `std::sort` at `N=3e7`.

With `m = 5` partitioning points the speed of `nth_element` is about `5` times that of `std::sort` at `N=3e7`.

With `m = 500` partitioning points the speed of `nth_element` is about `2` times that of `std::sort` at `N=3e7`.

With `m = 50` partitioning points the speed of `nth_element` is about `2` times that of `std::sort` at `N=3e5`.

With `m = 5` partitioning points the speed of `nth_element` is about `2` times that of `std::sort` at `N=3e3`.

[<img alt="alt_text" width="68%" src="plotting/images/partition_points_equidistant_time_for_n.png" />](plotting/images/partition_points_equidistant_time_for_n_div.png?raw=1)

The above image shows a different view of the same data. The execution times for `std::sort` for various vector sizes `N` and for `nth_element` with different numbers `m` of evenly spaced partitioning points in the vector. Arbitrary normalization.

The complexity of `nth_element` is `O(N log m)`. Therefore a thin line `k_i × N` is also drawn for each curve `i` where `k_i` is set to match at `N=3e6`. For `std::sort`, a dotted line `s × N log N` is drawn instead. (Note how similar `N log N` and `N` looks in log-log.)

[<img alt="alt_text" width="68%" src="plotting/images/partition_points_equidistant_time_for_n_div.png" />](plotting/images/partition_points_equidistant_time_for_n_div.png?raw=1)

This is the same data as shown in the previous figure except all data points are divided by the `s × N log N` curve described above.
In other words, the image represents the execution time compared to the expected `N log N` of `std::sort` at `N=3e6`. As expected, the `std::sort` line approaches `1.0` for large `N`, while the relative execution time for `nth_element` decreases as `1/log N` for large N.

[<img alt="alt_text" width="68%" src="plotting/images/partition_points_equidistant_speed_for_m_vs_sort.png" />](plotting/images/partition_points_equidistant_speed_for_m_vs_sort.png?raw=1)

Another view of the same data, but shows the speedup as function of `m` for different vector sizes `N`.
It's interesting that even for quite small `N=3e4` there are speedups of `2` to `6` times for `m=1` to `m=20`.

## Time vs `std::sort` for randomized partitioning points

The following plots are similar to the ones above except that the partitioning points are uniformly random (unique) instead of the equidistant points above. The conclusions are very similar, but the performance is slightly better.

[<img alt="alt_text" width="68%" src="plotting/images/partition_points_random_speed_for_n_vs_sort.png" />](plotting/images/partition_points_random_speed_for_n_vs_sort.png)

[<img alt="alt_text" width="68%" src="plotting/images/partition_points_random_time_for_n.png" />](plotting/images/partition_points_random_time_for_n_div.png?raw=1)

[<img alt="alt_text" width="68%" src="plotting/images/partition_points_random_time_for_n_div.png" />](plotting/images/partition_points_random_time_for_n_div.png?raw=1)

[<img alt="alt_text" width="68%" src="plotting/images/partition_points_random_speed_for_m_vs_sort.png" />](plotting/images/partition_points_random_speed_for_m_vs_sort.png?raw=1)

## Data generation and scripts

The cpp code (Visual Studio project) for generating the data is found
at [code](code), and the python scripts are at [plotting](plotting)

Each shown data points is the mean of 12 repeated measurements "trials", (excluding outliers, using `numpy.partition`, the python incarnation of the proposed range-of-nths algoritm). Error bars show the sample standard deviation of the 12 data points.
Each trial in turns repeats the execution of `nth_element` (and `std::sort`) several times. More times for smaller vectors. Times are measured using `chrono::high_resolution_clock`.

The above plots were generated using
```
Visual Studio 16.11.7, Standard release build.
Windows 10, 19044.1348.
Processor	11th Gen Intel(R) Core(TM) i7-11700K @ 3.60GHz   3.60 GHz
Installed RAM	32.0 GB (31.8 GB usable)
```
The execution takes about 21 minutes for equidistant or random partitioning. A vast majority of the execution time is the repeated `std::sort` on large vectors; the reference time of `std::sort` is unnecessarily repeated for each `m`.

Similar results were seen on an older CPU: AMD Phenom II X6 1090T.
