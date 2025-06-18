#pragma once
#define STATICALLY_WIDEN(CharT, str) lib2::s_widen<CharT>(str, L##str, u8##str, u##str, U##str)