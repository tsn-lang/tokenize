#pragma once
#include <tokenize/types.h>
#include <tokenize/SourceLocation.h>
#include <utils/Exception.h>

namespace tokenize {
    class Resource;
    
    class SourceException : public Exception {
        public:
            SourceException(Resource* src, const SourceLocation& loc, const String& msg);
            SourceException(Resource* src, const SourceLocation& loc, const char* msgFmt, ...);

            Resource* getSource() const;
            const SourceLocation& getLocation() const;
        
        protected:
            Resource* m_src;
            SourceLocation m_loc;
    };
};