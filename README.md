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
This project uses CMake with modern C++23 features. No outside dependencies other than a C++23 compiler and an up to date CMake.

# Installing
I haven't gotten installing down yet, as C++ modules with CMake are experimental and still have its issues. I believe once I find a separate project that will utilize these set of libraries, I'll figure that out.

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
lib2::ostringstream    5s  175426872 2.29
std::stringbuf         5s  174088972 2.27
std::string            5s  172292547 2.25
std::ostringstream     5s   76576314 1.00
```
```
Write strings:
-------------------------------------------------
Benchmark                Time Iterations Multiple
-------------------------------------------------
lib2::ostringstream 1548.01ms   30000000 1.78
std::stringbuf      1622.10ms   30000000 1.70
std::string         2068.64ms   30000000 1.33
std::ostringstream  2753.06ms   30000000 1.00
```
### fstream
```
-------------------------------------------
Benchmark          Time Iterations Multiple
-------------------------------------------
lib2_fstream         8s   39889658 2.85    
lib2_fstream_fmt     8s   29245940 2.09    
fprintf              8s   20410975 1.46    
fstream_insertion    8s   19481705 1.39    
fstream_print        8s   13991613 1.00
```
## fmt
```
Default formatting of std::uint32:
----------------------------------------
Benchmark       Time Iterations Multiple
----------------------------------------
lib2::ostream     5s   74674434 5.65
std::to_string    5s   59321166 4.49
lib2::format      5s   50436970 3.81
std::format       5s   32938206 2.49
std::ostream      5s   13221026 1.00
```
```
Default formatting of std::int32:
----------------------------------------
Benchmark       Time Iterations Multiple
----------------------------------------
lib2::ostream     5s   53488184 4.58
std::to_string    5s   43796982 3.75
lib2::format      5s   42656134 3.65
std::format       5s   27686977 2.37
std::ostream      5s   11671625 1.00
```