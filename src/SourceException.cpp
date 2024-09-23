#include <tokenize/SourceException.h>
#include <stdarg.h>

namespace tokenize {
    SourceException::SourceException(Resource* src, const SourceLocation& loc, const String& msg) : Exception(msg), m_src(src), m_loc(loc) {}
    SourceException::SourceException(Resource* src, const SourceLocation& loc, const char* msgFmt, ...) : Exception(String()), m_src(src), m_loc(loc) {
        char buf[1024] = { 0 };

        va_list l;
        va_start(l, msgFmt);
        vsnprintf(buf, 1024, msgFmt, l);
        va_end(l);

        m_msg = buf;
    }

    Resource* SourceException::getSource() const {
        return m_src;
    }

    const SourceLocation& SourceException::getLocation() const {
        return m_loc;
    }
};