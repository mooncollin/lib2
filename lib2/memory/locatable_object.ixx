export module lib2.memory:locatable_object;

import std;

namespace lib2
{
    export
    template<class T>
    class locatable_object
    {
    public:
        constexpr locatable_object() noexcept = default;

        template<class... Args>
        [[nodiscard]] constexpr T& start_lifetime(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        {
            return *std::construct_at(&storage.value, std::forward<Args>(args)...);
        }
    private:
        union alignas(T) Storage {
            T value;
            char dummy{};

            constexpr ~Storage() noexcept {}
        } storage;
    };
}