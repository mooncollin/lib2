module;

#include <cstdio>

export module lib2.io:file;

import std;

import :sink;
import :source;

namespace lib2::io
{
    export
    enum class openmode : int
    {
        in          = 0x1,
        out         = 0x2,
        app         = 0x4,
        trunc       = 0x8,
        noreplace   = 0x10
    };

    export
    enum class seek_mode : int
    {
        set = SEEK_SET,
        cur = SEEK_CUR,
        end = SEEK_END
    };

    export
    constexpr bool operator&(const openmode lhs, const openmode rhs) noexcept
    {
        return std::to_underlying(lhs) & std::to_underlying(rhs);
    }

    export
    constexpr openmode operator|(const openmode lhs, const openmode rhs) noexcept
    {
        return static_cast<openmode>(std::to_underlying(lhs) | std::to_underlying(rhs));
    }

    export
    constexpr openmode& operator|=(openmode& lhs, const openmode rhs) noexcept
    {
        lhs = lhs | rhs;
        return lhs;
    }

    export
    class file_sink
    {
    public:
        constexpr file_sink() noexcept
            : handle{0} {}

        file_sink(const std::filesystem::path::string_type::value_type* const filename, const openmode mode = openmode::out)
            : handle{0}
        {
            open(filename, mode);
        }

        file_sink(const std::filesystem::path::string_type& filename, const openmode mode = openmode::out)
            : file_sink{filename.c_str(), mode} {}

        file_sink(const std::filesystem::path& filename, const openmode mode = openmode::out)
            : file_sink{filename.native(), mode} {}

        ~file_sink() noexcept
        {
            try
            {
                close();
            } catch (...) {}
        }

        void open(const std::filesystem::path::string_type::value_type* filename, openmode mode = openmode::out);

        void open(const std::filesystem::path::string_type& filename, const openmode mode = openmode::out)
        {
            open(filename.c_str(), mode);
        }

        void open(const std::filesystem::path& path, const openmode mode = openmode::out)
        {
            open(path.native(), mode);
        }

        void close();

        [[nodiscard]] bool is_open() const noexcept
        {
            return handle;
        }
        
        void put(const char c);
        void write(const char* buf, std::size_t count);
        void fill(const char c, std::size_t count);

        inline void write(const std::span<const char> buf)
        {
            write(buf.data(), buf.size());
        }

        inline void write(const char* const cstr)
        {
            write(cstr, std::char_traits<char>::length(cstr));
        }
    private:
        std::uintptr_t handle;
    };
    static_assert(sink<file_sink>);

    export
    class file_source
    {
    public:
        constexpr file_source() noexcept
            : handle{0} {}

        file_source(const std::filesystem::path::string_type::value_type* const filename, const openmode mode = openmode::in)
            : handle{0}
        {
            open(filename, mode);
        }

        file_source(const std::filesystem::path::string_type& filename, const openmode mode = openmode::in)
            : file_source{filename.c_str(), mode} {}

        file_source(const std::filesystem::path& filename, const openmode mode = openmode::in)
            : file_source{filename.native(), mode} {}

        ~file_source() noexcept
        {
            try
            {
                close();
            } catch (...) {}
        }

        void open(const std::filesystem::path::string_type::value_type* filename, openmode mode = openmode::in);

        void open(const std::filesystem::path::string_type& filename, const openmode mode = openmode::in)
        {
            open(filename.c_str(), mode);
        }

        void open(const std::filesystem::path& path, const openmode mode = openmode::in)
        {
            open(path.native(), mode);
        }

        void close();

        [[nodiscard]] bool is_open() const noexcept
        {
            return handle;
        }

        opt_char get();
        std::size_t read(char*, std::size_t);

        inline std::size_t read(const std::span<char> buf)
        {
            return read(buf.data(), buf.size());
        }
    private:
        std::uintptr_t handle;
    };
    static_assert(source<file_source>);

    dyn_sink init_cout() noexcept;
    dyn_sink init_cerr() noexcept;
    dyn_peekable_source init_cin() noexcept;
}

namespace lib2
{
    export
    inline io::dyn_sink cout {io::init_cout()};
    
    export
    inline io::dyn_sink cerr {io::init_cerr()};

    export
    inline io::dyn_peekable_source cin {io::init_cin()};
}