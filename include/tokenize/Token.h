#pragma once
#include <tokenize/types.h>
#include <tokenize/SourceLocation.h>
#include <utils/String.h>

namespace tokenize {
    class Resource;

    struct Token {
        TokenType type;
        i32 subType;
        SourceLocation location;
        i32 contentBeginOffset;
        i32 contentLength;
        Resource* source;

        String toString() const;
        String getContentString() const;
    };
};