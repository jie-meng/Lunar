#include "octaveapi.h"
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
#include <QMessageBox>

#include "Qsci/qscilexer.h"

#include <vector>
#include "util/string.hpp"
#include "lunarcommon.h"

using namespace std;
using namespace util;

namespace gui
{

OctaveApi::OctaveApi(QsciLexer *parent) :
    QsciAPIsEx(parent)
{
}

void OctaveApi::updateAutoCompletionList(const QStringList &context,
            QStringList &list)
{
    QsciAPIsEx::updateAutoCompletionList(context, list);
}

QStringList OctaveApi::callTips(const QStringList &context, int commas,
        QsciScintilla::CallTipsStyle style,
        QList<int> &shifts)
{
    return QsciAPIsEx::callTips(context, commas, style, shifts);
}

}
