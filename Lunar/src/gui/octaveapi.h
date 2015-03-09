#ifndef OCTAVEAPI_H
#define OCTAVEAPI_H

#include <QObject>
#include "qsciapisex.h"

class QsciLexer;

namespace gui
{

class OctaveApi : public QsciAPIsEx
{
    Q_OBJECT
public:
    explicit OctaveApi(QsciLexer *parent = 0);
    virtual void updateAutoCompletionList(const QStringList &context,
                QStringList &list);
    virtual QStringList callTips(const QStringList &context, int commas,
            QsciScintilla::CallTipsStyle style,
            QList<int> &shifts);
signals:

public slots:

};

}

#endif // OCTAVEAPI_H
