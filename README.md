# lib2
An extended C++ standard library.

This project is dedicated to adding useful features for C++ developers that utilize cutting edge C++ versions and techniques. Many useful features that the standard cannot have due to its constraints will be here. Upgrades to existing standard libraries will also be here, as a way to test out what a std2 might be like.

Any of the public or private APIs here can change at a moments notice. All libraries, even those deemed "Done" are all experimental and serve the purpose of furthering whatever I choose them to be.

# Current Libraries
| Library | Description | Code Status | Test Status | Doc Status |
| ------- | ----------- | ----------- | ----------- | ---------- |
| platform | Probe the type of architecture, operating system, compiler, and C++ version you are using at compile time | ✅ | ⚠️ | ❌ |
| type_traits | Additional type traits | ✅ | ⚠️ | ❌ |
| strings | More string abstractions and algorithms | ✅ | ⚠️ | ❌ |
| io | A reimagining of C++ streams and other io functionality | ⚠️ | ⚠️ | ❌ |
| fmt | A reimagining of std::format utilizing the io library | ✅ | ⚠️ | ❌ |
| test | A test case framework | ✅ | ✅ | ❌ |
| benchmarking | A benchmarking framework | ✅ | ✅ | ❌ |
| scan | The flip side to std::format. A modern approach to scanf | ⚠️ | ⚠️ | ❌ |

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
lib2::ostringstream    5s  178242011 1.84
std::string            5s  173816111 1.80
std::stringbuf         5s  173559612 1.80
std::ostringstream     5s   96631776 1.00
```
```
Write strings:
-------------------------------------------------
Benchmark                Time Iterations Multiple
-------------------------------------------------
lib2::ostringstream 1653.71ms   30000000 1.50
std::stringbuf      1688.29ms   30000000 1.47
std::string         2075.81ms   30000000 1.19
std::ostringstream  2480.17ms   30000000 1.00
```
### fstream
```
---------------------------------------------
Benchmark            Time Iterations Multiple
---------------------------------------------
lib2_fstream_write     8s  120557283 6.79
lib2_fstream_fmt       8s  103227193 5.81
lib2_fstream_rt_fmt    8s   65908364 3.71
fstream                8s   25579747 1.44
fprintf                8s   21392484 1.20
fstream_print          8s   17761981 1.00
```
## fmt
```
Default formatting of std::uint32:
----------------------------------------
Benchmark       Time Iterations Multiple
----------------------------------------
std::to_string    5s   78459581 7.39
lib2::format      5s   55179142 5.20
std::format       5s   41335222 3.89
std::ostream      5s   10620343 1.00
```
```
Default formatting of std::int32:
----------------------------------------
Benchmark       Time Iterations Multiple
----------------------------------------
std::to_string    5s   70156031 6.88
lib2::format      5s   49372213 4.84
std::format       5s   34091229 3.34
std::ostream      5s   10204504 1.00
```