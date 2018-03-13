#ifndef QSCILEXERNONE_H
#define QSCILEXERNONE_H

#include <QObject>

#include <Qsci/qsciglobal.h>
#include <Qsci/qscilexer.h>

namespace gui {

class LexerNone : public QsciLexer
{
    Q_OBJECT
public:
    LexerNone(QObject *parent = 0);

    virtual ~LexerNone();    

    virtual const char *language() const;
    virtual QString description(int style) const;
private:
    LexerNone(const LexerNone &);
    LexerNone &operator=(const LexerNone &);
};

}

#endif
