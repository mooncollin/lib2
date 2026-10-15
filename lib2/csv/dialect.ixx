export module lib2.csv:dialect;

namespace lib2::csv
{
    export
    enum class quoting_style
    {
        minimal,
        all,
        nonnumeric,
        none
    };

    export
    struct dialect
    {
        char delimiter {','};
        char quotechar {'"'};
        char escapechar {'\\'};
        quoting_style quoting {quoting_style::minimal};
    };
}