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
--------------------------------------------
Benchmark           Time Iterations Multiple
--------------------------------------------
lib2_fstream          8s  137382251 8.11    
lib2_async_fstream    8s  117637319 6.95    
lib2_fstream_fmt      8s   64894826 3.83    
fstream               8s   21369867 1.26    
fprintf               8s   19823362 1.17    
fstream_print         8s   16937607 1.00
```
## fmt
```
Default formatting of std::uint32:
----------------------------------------
Benchmark       Time Iterations Multiple
----------------------------------------
std::to_string    5s   80722295 7.56
lib2::ostream     5s   63618137 5.96
std::format       5s   49360006 4.62
lib2::format      5s   43089415 4.03
std::ostream      5s   10683052 1.00
```
```
Default formatting of std::int32:
----------------------------------------
Benchmark       Time Iterations Multiple
----------------------------------------
std::to_string    5s   63858623 6.30
lib2::ostream     5s   55333765 5.46
lib2::format      5s   40380979 3.98
std::format       5s   39523016 3.90
std::ostream      5s   10138718 1.00
```