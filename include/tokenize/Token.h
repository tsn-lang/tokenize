#pragma once
#include <tokenize/types.h>
#include <tokenize/SourceLocation.h>

namespace tokenize {
    class SourceFile;

    struct Token {
        TokenType type;
        i32 subType;
        SourceLocation location;
        SourceFile* source;
    };
};