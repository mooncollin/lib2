export module lib2.benchmarking:do_not_optimize;

namespace lib2::benchmarking
{
    void use_char_pointer(const volatile char*) noexcept {}

    export
    void do_not_optimize(const auto& value) noexcept
    {
        use_char_pointer(&reinterpret_cast<const volatile char&>(value));
    }
}