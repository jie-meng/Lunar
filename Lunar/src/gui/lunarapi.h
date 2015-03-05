#ifndef LUNARAPI_H
#define LUNARAPI_H

#include <QObject>
#include <Qsci/qsciapis.h>

class QsciLexer;

class LunarApi : public QsciAPIs
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

};

#endif // LUNARAPI_H
