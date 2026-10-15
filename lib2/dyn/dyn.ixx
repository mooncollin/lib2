export module lib2.dyn;

import std;

import lib2.type_traits;

namespace lib2
{
    struct destruct_vtable
    {
        void(*destruct)(void*) noexcept;
    };

    template<class T>
    inline constexpr destruct_vtable destruct_for {
        .destruct = [](void* const obj) noexcept {
            static_cast<T*>(obj)->~T();
        }
    };

    template<class... Facets>
    struct vtable_t : public destruct_vtable, public Facets::vtable_type... {};

    template<typename T, class... Facets>
    inline constexpr vtable_t<Facets...> vtables_for {
        destruct_for<T>,
        Facets::template get_vtable<T>()...
    };

    export
    template<class... Facets>
    class dyn : public Facets::mixin...
    {
    public:
        template<class T>
            requires(!std::same_as<T, dyn> && (Facets::template supports<T> && ...))
        constexpr dyn(T& obj) noexcept
            : obj_{std::addressof(obj)}
            , vptr{&vtables_for<T, Facets...>} {}

        template<class... Facets2>
            requires(!std::same_as<dyn<Facets2...>, dyn> && (is_any_of_v<Facets, Facets2...> && ...))
        constexpr dyn(const dyn<Facets2...> higher_mixin) noexcept
            : obj_{higher_mixin.obj_}
            , vptr{higher_mixin.vptr} {}

        constexpr dyn(const dyn&) noexcept = default;

        [[nodiscard]] void* obj() const noexcept
        {
            return obj_;
        }

        [[nodiscard]] const vtable_t<Facets...>* vtable() const noexcept
        {
            return vptr;
        }
    private:
        void* obj_;
        const vtable_t<Facets...>* vptr;

        template<class...>
        friend class dyn;
    };
}