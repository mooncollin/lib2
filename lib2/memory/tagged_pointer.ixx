export module lib2.memory:tagged_pointer;

import std;

namespace lib2
{
    export
    template<class T, std::size_t Bits>
    class tagged_pointer
    {
        static_assert(Bits <= (alignof(std::max_align_t) & ~(alignof(T) - 1)), "Too many bits requested");
    public:
        tagged_pointer(T* ptr_ = nullptr) noexcept
            : ptr_{reinterpret_cast<std::uintptr_t>(ptr_)} {}
        
        tagged_pointer(const tagged_pointer&) noexcept = default;
        tagged_pointer& operator=(const tagged_pointer&) noexcept = default;

        tagged_pointer& operator=(T* const ptr_) noexcept
        {
            ptr(ptr_);
            return *this;
        }

        void ptr(T* const ptr_) noexcept
        {
            const auto tag {this->tag()};
            this->ptr_ = reinterpret_cast<std::uintptr_t>(ptr_) | tag;
        }

        T* ptr() const noexcept
        {
            return reinterpret_cast<T*>(ptr_ & ~((std::uintptr_t{1} << Bits) - 1));
        }

        void tag(const std::uintptr_t tag)
        {
            if (tag >= (std::uintptr_t{1} << Bits))
            {
                throw std::out_of_range{"Tag value out of range"};
            }

            const auto ptr {this->ptr()};
            this->ptr_ = reinterpret_cast<std::uintptr_t>(ptr) | tag;
        }

        std::uintptr_t tag() const noexcept
        {
            return ptr_ & ((std::uintptr_t{1} << Bits) - 1);
        }

        explicit operator bool() const noexcept
        {
            return static_cast<bool>(ptr());
        }

        void swap(tagged_pointer& other) noexcept
        {
            using std::swap;
            swap(ptr_, other.ptr_);
        }
    private:
        std::uintptr_t ptr_;
    };

    export
    template<class T, std::size_t Bits>
    void swap(tagged_pointer<T, Bits>& a, tagged_pointer<T, Bits>& b) noexcept
    {
        a.swap(b);
    }
}