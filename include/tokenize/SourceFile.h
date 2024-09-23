#pragma once
#include <tokenize/types.h>
#include <tokenize/SourceLocation.h>
#include <utils/String.h>

namespace tokenize {
    class SourceFile {
        public:
            SourceFile(const char* contents, u32 resourceId);
            ~SourceFile();

            u32 getResourceId() const;
            const char* getPointerToLocation(const SourceLocation& loc) const;
            String getStringAtLocation(const SourceLocation& loc) const;
        
        protected:
            u32 m_resourceId;
            u32 m_length;
            char* m_contents;
    };
};