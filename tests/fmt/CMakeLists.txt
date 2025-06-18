target_sources(lib2_tests PUBLIC
    FILE_SET lib2_tests_fmt_modules TYPE CXX_MODULES FILES
        character.ixx
        integral.ixx
        floating.ixx
        string.ixx
        chrono.ixx
        fmt.ixx
)