module;

#include <cstdio>
#include <cerrno>
#include "lib2/strings/widen.h"

export module lib2.io:file;

import std;

import lib2.strings;

namespace lib2
{
    [[noreturn]] void throw_io();

    export
    class openmode
    {
    public:
        static const openmode in;
        static const openmode binary;
        static const openmode out;
        static const openmode trunc;
        static const openmode app;
        static const openmode noreplace;

        [[nodiscard]] constexpr const char* str() const noexcept
        {
            return do_str<char>(value_);
        }

        [[nodiscard]] constexpr const wchar_t* wstr() const noexcept
        {
            return do_str<wchar_t>(value_);
        }

        [[nodiscard]] constexpr int value() const noexcept
        {
            return value_;
        }

        constexpr openmode& operator|=(openmode other);
        friend constexpr openmode operator|(openmode lhs, openmode rhs);
        friend constexpr bool operator&(openmode lhs, openmode rhs) noexcept;

        [[nodiscard]] constexpr bool operator==(const openmode&) const noexcept = default;
        [[nodiscard]] constexpr bool operator!=(const openmode&) const noexcept = default;
    private:
        int value_;

        constexpr explicit openmode(const int value) noexcept
            : value_{value} {}

        template<class CharT>
        [[nodiscard]] static constexpr const CharT* do_str(const int value) noexcept
        {
            switch (value)
            {
            case std::ios_base::in:
                return STATICALLY_WIDEN(CharT, "r").data();
            case std::ios_base::binary | std::ios_base::in:
                return STATICALLY_WIDEN(CharT, "rb").data();
            case std::ios_base::in | std::ios_base::out:
                return STATICALLY_WIDEN(CharT, "r+").data();
            case std::ios_base::binary | std::ios_base::in | std::ios_base::out:
                return STATICALLY_WIDEN(CharT, "r+b").data();
            case std::ios_base::out:
            case std::ios_base::out | std::ios_base::trunc:
                return STATICALLY_WIDEN(CharT, "w").data();
            case std::ios_base::binary | std::ios_base::out:
            case std::ios_base::binary | std::ios_base::out | std::ios_base::trunc:
                return STATICALLY_WIDEN(CharT, "wb").data();
            case std::ios_base::in | std::ios_base::out | std::ios_base::trunc:
                return STATICALLY_WIDEN(CharT, "w+").data();
            case std::ios_base::binary | std::ios_base::in | std::ios_base::out  | std::ios_base::trunc:
                return STATICALLY_WIDEN(CharT, "w+b").data();
            case std::ios_base::out | std::ios_base::noreplace:
            case std::ios_base::out | std::ios_base::trunc | std::ios_base::noreplace:
                return STATICALLY_WIDEN(CharT, "wx").data();
            case std::ios_base::binary | std::ios_base::out | std::ios_base::noreplace:
            case std::ios_base::binary | std::ios_base::out | std::ios_base::trunc | std::ios_base::noreplace:
                return STATICALLY_WIDEN(CharT, "wbx").data();
            case std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::noreplace:
                return STATICALLY_WIDEN(CharT, "w+x").data();
            case std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::noreplace:
                return STATICALLY_WIDEN(CharT, "w+bx").data();
            case std::ios_base::out | std::ios_base::app:
            case std::ios_base::app:
                return STATICALLY_WIDEN(CharT, "a").data();
            case std::ios_base::binary | std::ios_base::out | std::ios_base::app:
            case std::ios_base::binary | std::ios_base::app:
                return STATICALLY_WIDEN(CharT, "ab").data();
            case std::ios_base::in | std::ios_base::out | std::ios_base::app:
            case std::ios_base::in | std::ios_base::app:
                return STATICALLY_WIDEN(CharT, "a+").data();
            case std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::app:
            case std::ios_base::binary | std::ios_base::in | std::ios_base::app:
                return STATICALLY_WIDEN(CharT, "a+b").data();
            }

            return nullptr;
        }
    };

    export
    inline constexpr openmode openmode::in {std::ios_base::in};
    
    export
    inline constexpr openmode openmode::binary {std::ios_base::binary};
    
    export
    inline constexpr openmode openmode::out {std::ios_base::out};
    
    export
    inline constexpr openmode openmode::trunc {std::ios_base::trunc};
    
    export
    inline constexpr openmode openmode::app {std::ios_base::app};
    
    export
    inline constexpr openmode openmode::noreplace {std::ios_base::noreplace};

    export
    constexpr openmode& openmode::operator|=(const openmode other)
    {
        if (do_str<char>(value_ | other.value_) == nullptr)
        {
            throw std::invalid_argument{"invalid openmode"};
        }

        value_ |= other.value_;

        return *this;
    }

    export
    constexpr openmode operator|(openmode lhs, const openmode rhs)
    {
        lhs |= rhs;
        return lhs;
    }

    export
    constexpr bool operator&(const openmode lhs, const openmode rhs) noexcept
    {
        return static_cast<bool>(lhs.value_ & rhs.value_);
    }

    export
    enum class buffer_mode : int
    {
        fully = _IOFBF,
        line  = _IOLBF,
        none  = _IONBF
    };

    export
    enum class seek_mode : int
    {
        set = SEEK_SET,
        cur = SEEK_CUR,
        end = SEEK_END
    };

    export
    enum class wide_orientation : int
    {
        narrow = -1,
        query  = 0,
        none   = query,
        wide   = 1
    };
    
    export
    class file_ref
    {
    public:
        using native_handle_type = std::FILE*;

        file_ref(std::FILE& handle) noexcept;

        file_ref(const file_ref&) noexcept = default;
        file_ref& operator=(const file_ref&) noexcept = default;

        [[nodiscard]] operator native_handle_type() const noexcept
        {
            return handle_;
        }

        [[nodiscard]] native_handle_type native_handle() const noexcept
        {
            return handle_;
        }

        void flush();
        void setbuf(const std::span<char> buffer, const buffer_mode mode);
        void setbuf();

        template<class T>
        std::size_t read(T* const buffer, const std::size_t count)
        {
            errno = 0;
            std::size_t read_;
            if ((read_ = std::fread(buffer, sizeof(T), count, handle_)) != count)
            {
                if (std::ferror(handle_))
                {
                    throw_io();
                }
            }

            return read_;
        }

        template<std::ranges::contiguous_range R>
            requires(std::ranges::sized_range<R>)
        std::size_t read(R& r)
        {
            return read(std::ranges::data(r), std::ranges::size(r));
        }

        template<class T>
        void write(const T* const buffer, const std::size_t count)
        {
            errno = 0;
            if (std::fwrite(buffer, sizeof(T), count, handle_) != count)
            {
                throw_io();
            }
        }

        template<std::ranges::contiguous_range R>
            requires(std::ranges::sized_range<R>)
        void write(R&& r)
        {
            write(std::ranges::data(r), std::ranges::size(r));
        }

        bool get(char& c);
        bool get(wchar_t& c);
        void put(const char ch);
        void put(const wchar_t ch);
        void unget(const char ch);
        void unget(const wchar_t ch);
        long tell() const;
        std::fpos_t getpos() const;
        void setpos(const std::fpos_t& pos);
        void seek(const long offset, const seek_mode origin);
        wide_orientation wide(const wide_orientation mode);
        void rewind() noexcept;
        void clearerr() noexcept;
        bool eof() const noexcept;
    private:
        std::FILE* handle_;
    };
    
    export
    class file
    {
    public:
        using native_handle_type = std::FILE*;

        file() noexcept;
        file(native_handle_type&& handle) noexcept;
        
        file(const file&) = delete;
        file(file&& other) noexcept;
        
        file& operator=(const file&) = delete;
        file& operator=(file&& other) noexcept;

        ~file() noexcept;

        void swap(file& rhs) noexcept
        {
            std::swap(handle_, rhs.handle_);
        }

        [[nodiscard]] native_handle_type native_handle() const noexcept
        {
            return handle_;
        }

        [[nodiscard]] bool is_open() const noexcept
        {
            return handle_ != nullptr;
        }

        [[nodiscard]] native_handle_type release() noexcept
        {
            return std::exchange(handle_, nullptr);
        }

        std::expected<file_ref, std::error_code> ref() const noexcept;
        void reset(native_handle_type handle);
        std::expected<file_ref, std::error_code> open(const std::filesystem::path::string_type::value_type* p, const openmode mode);
        std::expected<file_ref, std::error_code> open(const std::filesystem::path::string_type& p, const openmode mode);
        std::expected<file_ref, std::error_code> open(const std::filesystem::path& p, const openmode mode);
        std::expected<file_ref, std::error_code> reopen(const std::filesystem::path::string_type::value_type* p, const openmode mode);
        std::expected<file_ref, std::error_code> reopen(const std::filesystem::path::string_type& p, const openmode mode);
        std::expected<file_ref, std::error_code> reopen(const std::filesystem::path& p, const openmode mode);
        void close();
    private:
        native_handle_type handle_;
    };

    export
    inline void swap(file& lhs, file& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    export
    class ifile_iterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = char;
        using difference_type   = long;
        using reference         = char;
        using pointer           = char*;

        ifile_iterator() noexcept
            : file_{nullptr} {}

        ifile_iterator(const std::default_sentinel_t) noexcept
            : ifile_iterator{} {}

        ifile_iterator(file_ref& file)
            : file_{std::addressof(file)}
        {
            ++*this;
        }

        ifile_iterator(const ifile_iterator&) noexcept = default;

        char operator*() const
        {
            return read_;
        }

        ifile_iterator& operator++()
        {
            if (!file_->get(read_))
            {
                file_ = nullptr;
            }

            return *this;
        }

        [[nodiscard]] bool operator==(const ifile_iterator& other) const noexcept
        {
            return file_ == other.file_;
        }

        [[nodiscard]] bool operator!=(const ifile_iterator& other) const noexcept
        {
            return file_ != other.file_;
        }
    private:
        file_ref* file_;
        char read_;
    };

    export
    class ofile_iterator
    {
    public:
        using iterator_category = std::output_iterator_tag;
        using value_type        = void;
        using difference_type   = std::ptrdiff_t;
        using reference         = void;
        using pointer           = void;

        ofile_iterator(file_ref& file) noexcept
            : file_{std::addressof(file)} {}

        ofile_iterator(const ofile_iterator&) noexcept = default;

        ofile_iterator& operator=(const char c)
        {
            file_->put(c);
            return *this;
        }

        ofile_iterator& operator=(const wchar_t c)
        {
            file_->put(c);
            return *this;
        }

        ofile_iterator& operator*()
        {
            return *this;
        }

        ofile_iterator& operator++()
        {
            return *this;
        }

        ofile_iterator& operator++(int)
        {
            return *this;
        }
    private:
        file_ref* file_;
    };

    export
    inline file_ref cstdin  {*stdin};
    
    export
    inline file_ref cstdout {*stdout};
    
    export
    inline file_ref cstderr {*stderr};
}