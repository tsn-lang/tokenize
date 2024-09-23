#pragma once
#include <utils/types.h>

namespace tokenize {
    using namespace utils;

    enum class TokenType {
        Keyword,
        Symbol,
        Identifier,
        StringLiteral,
        NumberLiteral,
        Comment,
        Macro,
        EndOfStatement,
        EndOfInput
    };
};