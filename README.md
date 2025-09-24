# lib2
An extended C++ standard library.

This project is dedicated to adding useful features for C++ developers that utilize cutting edge C++ versions and techniques. Many useful features that the standard cannot have due to its constraints will be here. Upgrades to existing standard libraries will also be here, as a way to test out what a std2 might be like.

Any of the public or private APIs here can change at a moments notice. All libraries, even those deemed "Done" are all experimental and serve the purpose of furthering whatever I choose them to be.

# Current Libraries
| Library | Description | Code Status | Test Status | Doc Status |
| ------- | ----------- | ----------- | ----------- | ---------- |
| platform | Probe the type of architecture, operating system, compiler, and C++ version you are using at compile time | ✅ | ⚠️ | ❌ |
| type_traits | Additional type traits | ✅ | ⚠️ | ❌ |
| strings | More string abstractions and algorithms | ⚠️ | ⚠️ | ❌ |
| io | A reimagining of C++ streams and other io functionality | ✅ | ⚠️ | ❌ |
| fmt | A reimagining of fmt utilizing the io library | ✅ | ⚠️ | ❌ |
| test | A test case framework | ✅ | ⚠️ | ❌ |
| benchmarking | A benchmarking framework | ✅ | ⚠️ | ❌ |
| match | A pattern matching framework | ⚠️ | ⚠️ | ❌ |
| scan | The flip side to fmt. A modern approach to scanf | ⚠️ | ⚠️ | ❌ |

✅: "Done"

⚠️: Not quite there

❌: Haven't started

# Building
This project uses CMake with modern C++23 features. No outside dependencies other than a C++23 compiler and an up to date CMake. I export the targets, so all you need to do is add the build directory to your path and it should be available in `find_package`.

# Running Tests
My CMake tests are automatically generated and ran through a single test runner executable.

The best way to run all the tests at once is to go into the build/tests folder and run `.. -C Debug --output-on-failure`.

# Running Benchmarks
There is a benchmarks folder that holds various benchmarks to run. This is intended to check my implementations against the standard, or to prototype or demonstrate certain features. There is a cmake target for each file to create an executable that will print results.

# Benchmarks
## IO
### stringstream
```
Write characters:
---------------------------------------------
Benchmark            Time Iterations Multiple
---------------------------------------------
lib2::ostringstream    5s  180000511 2.00
std::stringbuf         5s  173673742 1.93
std::string            5s  169376743 1.88
std::ostringstream     5s   89890409 1.00
```
```
Write strings:
-------------------------------------------------
Benchmark                Time Iterations Multiple
-------------------------------------------------
std::stringbuf      1608.28ms   30000000 1.74
lib2::ostringstream 1695.59ms   30000000 1.65
std::string         2088.97ms   30000000 1.34
std::ostringstream  2794.28ms   30000000 1.00
```
### fstream
```
--------------------------------------------
Benchmark           Time Iterations Multiple
--------------------------------------------
lib2_fstream          8s  130342810 7.97
lib2_async_fstream    8s  114989771 7.03
lib2_fstream_fmt      8s   70692335 4.32
fstream               8s   21145601 1.29
fprintf               8s   17253510 1.06
fstream_print         8s   16351233 1.00
```
## fmt
```
Default formatting of std::uint32:
----------------------------------------
Benchmark       Time Iterations Multiple
----------------------------------------
lib2::ostream     5s   94082134 7.02
std::to_string    5s   76448604 5.70
lib2::format      5s   68478429 5.11
std::format       5s   48250966 3.60
std::ostream      5s   13401494 1.00
```
```
Default formatting of std::int32:
----------------------------------------
Benchmark       Time Iterations Multiple
----------------------------------------
lib2::ostream     5s   69888758 5.67
lib2::format      5s   54238168 4.40
std::to_string    5s   54205795 4.39
std::format       5s   38163354 3.09
std::ostream      5s   12333896 1.00
```