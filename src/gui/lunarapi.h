#ifndef LUNARAPI_H
#define LUNARAPI_H

#include <QObject>
#include "qsciapisex.h"
#include "lunarcommon.h"

class QsciLexer;

namespace gui
{

class LunarApi : public QsciAPIsEx
{
    Q_OBJECT
public:
    explicit LunarApi(QsciLexer *parent = 0);
    virtual void updateAutoCompletionList(const QStringList &context,
                QStringList &list);
    virtual QStringList callTips(const QStringList &context, int commas,
            QsciScintilla::CallTipsStyle style,
            QList<int> &shifts);
signals:

public slots:

private:
    DISALLOW_COPY_AND_ASSIGN(LunarApi)
};

}

#endif // LUNARAPI_H
