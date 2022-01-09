<div id="multi_nth_element_p3275">

#### Performance of reference implementation of `multi_nth_element`


The following is a study of an implementation of `multi_nth_element` of p2375 using the algorithm `multi_quick_select` found in this repository.
More at [code/nth_element_example/nth_element_example/CODE_README.md](code/nth_element_example/nth_element_example/CODE_README.md)

 Links:

 * Back to [github material_on_multi_nth_element/README.md](README.md).
 * Or to the proposal at https://github.com/jmlundberg/p2375

## Time vs `std::sort` for equidistant partitioning points

<img width="74%" src="plotting/images/multiselect_equi_speed_for_n_vs_sort.png?raw=1" />


The above image shows the execution speed of a `multi_quick_select` implementation compared to `std::sort`} for various vector sizes `N`.
Each data point is the mean of many repetitions, excluding outliers. Error bars show the spread of all repetitions and is influenced by the randomness of the test data. Individual data points do fluctuate between initial random seeds of the example, but the overall trends are very stable.

The alternative `bisect_nths` algorithm (similar to Alsuwaiyel2001) is about 10% slower for m>5, and about 20% slower for m>50 but has the same asymptotic complexity. The data to sort here consists of randomly shuffled doubles, and `multi_nth_element` was given different numbers `m` of evenly spaced partitioning points in the vector. Dotted lines show lines `k_i  × log(N)` fitted to pass though each curve at `N=3e6`.

The image shows that the speed curves approximately follow the expected `log(N)` shape, with different factors for different `m`.

The following table shows a few speedup factors for a number of unique partitioning points m and vector sizes N.

| m | speedup factor | at N |
|:----------------|:--------|-----:|
| 1 | 11 |  N=3e7  |
| 5 | 5.7 |  N=3e7  |
| 10 | 4.7 |  N=3e7  |
| 500 | 2.3 |  N=3e7  |
| 1000 | 2.2 |  N=3e7  |

| m | speedup factor | at N |
|:----------------|:--------|-----:|
| 10 | 3.6 |  N=3e5  |
|50 | 2.5 |  N=3e5  |

| m | speedup factor | at N |
|:----------------|:--------|-----:|
5 | 2.6 |  N=3e3  |

Clearly, the benefit compared to std::sort is the greatest for smaller m. In these tests of  `multi_quick_select`, the performance are still not worse than `std::sort` even for `m ~ N`.
The benefit over `sort` grows with `N` as `log N`.

<img width="74%" src="plotting/images/multiselect_equi_time_for_n.png?raw=1" />

The above image shows a different view of the same data. The execution times for `std::sort` for various vector sizes `N` and for `multi_nth_element` with different numbers `m` of evenly spaced partitioning points in the vector. Arbitrary normalization.

The complexity of `multi_nth_element` is `O(N log m)`. Therefore a thin line `k_i × N` is also drawn for each curve `i` where `k_i` is set to match at `N=3e6`. For `std::sort`, a dotted line `s × N log N` is drawn instead. (Note how similar `N log N` and `N` looks in log-log.)

<img width="74%" src="plotting/images/multiselect_equi_time_for_n_div.png?raw=1" />

This is the same data as shown in the previous figure except all data points are divided by the `s × N log N` curve described above.
In other words, the image represents the execution time compared to the expected `N log N` of `std::sort` at `N=3e6`. As expected, the `std::sort` line approaches `1.0` for large `N`, while the relative execution time for `multi_nth_element` decreases as `1/log N` for large N.

<img width="74%" src="plotting/images/multiselect_equi_speed_for_m_vs_sort.png?raw=1" />

Another view of the same data, but shows the speedup as function of `m` for different vector sizes `N`.
It's interesting that even for quite small `N=3e4` there are speedups of `2` to `6` times for `m=1` to `m=20`.

## Time vs `std::sort` for randomized partitioning points

The case where the partitioning points are uniformly random (unique) instead of the equidistant points above we investigated. The conclusions are very similar, but the performance is slightly better, as expected since some of the partition points may be close to each other or identical.

## Data generation and scripts

The cpp code (Visual Studio project) for generating the data is found
at [code](code), and the python scripts are at [plotting](plotting)

Each shown data points is the mean of 12 repeated measurements "trials", (excluding outliers, using `numpy.partition`, the python incarnation of the proposed range-of-nths algoritm). Error bars show the sample standard deviation of the 12 data points.
Each trial in turns repeats the execution of `multi_nth_element` (and `std::sort`) several times. More times for smaller vectors. Times are measured using `chrono::high_resolution_clock`.

The above plots were generated using
```
Visual Studio 16.11.7, Standard release build.
Windows 10, 19044.1348.
Processor	11th Gen Intel(R) Core(TM) i7-11700K @ 3.60GHz   3.60 GHz
Installed RAM	32.0 GB (31.8 GB usable)
```
The execution takes about 20 minutes for equidistant or random partitioning. A vast majority of the execution time is the repeated `std::sort` on large vectors; the reference time of `std::sort` is unnecessarily repeated for each `m`.

Similar results were seen on an older CPU: AMD Phenom II X6 1090T.
