export module lib2.shared;

import std;

namespace lib2
{
    export
    class shared_library
    {
    public:
        shared_library() noexcept
            : handle{nullptr} {}

        explicit shared_library(const std::filesystem::path& path)
            : shared_library{}
        {
            load(path);
        }

        ~shared_library() noexcept
        {
            try
            {
                unload();
            }
            catch (...) {}
        }

        shared_library(const shared_library&) = delete;
        shared_library(shared_library&& other) noexcept
            : handle{std::exchange(other.handle, nullptr)} {}

        shared_library& operator=(const shared_library&) = delete;
        shared_library& operator=(shared_library&& other)
        {
            if (this != std::addressof(other))
            {
                unload();
                handle = std::exchange(other.handle, nullptr);
            }
            return *this;
        }

        void load(const std::filesystem::path& path);
        void unload();
        void* get_func(const char* name) const;
    private:
        void* handle;
    };
}