#pragma once
#include <utils/types.h>

namespace tokenize {
    using namespace utils;

    enum class TokenType {
        Keyword,
        Symbol,
        Identifier,
        Literal,
        Comment,
        Macro,
        EndOfStatement,
        EndOfInput
    };
};