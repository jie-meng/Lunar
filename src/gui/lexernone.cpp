#include "lexernone.h"

namespace gui {

// The ctor.
LexerNone::LexerNone(QObject *parent)
    : QsciLexer(parent)
{
}


// The dtor.
LexerNone::~LexerNone()
{
}


// Returns the language name.
const char *LexerNone::language() const
{
    return "None";
}



QString LexerNone::description(int style) const
{
    return QString();
}

}
