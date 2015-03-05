#include "lunarapi.h"
#include <stdlib.h>
#include <qapplication.h>
#include <qdatastream.h>
#include <qdir.h>
#include <qevent.h>
#include <qfile.h>
#include <qmap.h>
#include <qtextstream.h>
#include <qthread.h>

#include <QLibraryInfo>

#include "Qsci/qscilexer.h"

LunarApi::LunarApi(QsciLexer *parent) :
    QsciAPIs(parent)
{
}

void LunarApi::updateAutoCompletionList(const QStringList &context,
            QStringList &list)
{
    QsciAPIs::updateAutoCompletionList(context, list);
}

QStringList LunarApi::callTips(const QStringList &context, int commas,
        QsciScintilla::CallTipsStyle style,
        QList<int> &shifts)
{
    return QsciAPIs::callTips(context, commas, style, shifts);
}
