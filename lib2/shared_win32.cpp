module;

#include <windows.h>

module lib2.shared;

import std;

import lib2.err;

namespace lib2
{
    void shared_library::load(const std::filesystem::path& path)
    {
        unload();
        handle = ::LoadLibraryW(path.c_str());
        if (!handle)
        {
            throw os_error{"Failed to load shared library"};
        }
    }

    void shared_library::unload()
    {
        if (handle)
        {
            if (!::FreeLibrary(static_cast<HMODULE>(handle)))
            {
                throw os_error{"Failed to unload shared library"};
            }
            handle = nullptr;
        }
    }

    void* shared_library::get_func(const char* const name) const
    {
        if (!handle)
        {
            throw std::runtime_error{"Shared library is not loaded"};
        }

        const auto func {::GetProcAddress(static_cast<HMODULE>(handle), name)};
        if (!func)
        {
            throw os_error{"Failed to get function address"};
        }
        return func;
    }
}