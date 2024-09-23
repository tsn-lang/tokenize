#pragma once
#include <tokenize/types.h>
#include <tokenize/SourceLocation.h>
#include <utils/String.h>
#include <utils/Array.h>

namespace tokenize {
    class TokenizedSource;
    class TokenSet;

    class Resource {
        public:
            Resource(const char* contents, u32 resourceId);
            ~Resource();

            u32 getResourceId() const;
            const char* getContents() const;
            u32 getLength() const;
            const char* getPointerToLocation(const SourceLocation& loc) const;
            String getStringAtLocation(const SourceLocation& loc) const;
            SourceLocation calculateSourceLocationFromRange(u32 beginOffset, u32 endOffset) const;
            String getLine(u32 index) const;
            TokenizedSource* tokenize(TokenSet* tokenSet) const;
        
        protected:
            struct LineOffset {
                u32 begin;
                u32 end;
            };

            u32 m_resourceId;
            u32 m_length;
            char* m_contents;
            Array<LineOffset> m_lineOffsets;
    };
};