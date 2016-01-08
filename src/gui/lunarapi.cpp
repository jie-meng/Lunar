#include "lunarapi.h"
#include <vector>
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
#include "util/base.hpp"
#include "lunarcommon.h"

using namespace std;
using namespace util;

namespace gui
{

LunarApi::LunarApi(QsciLexer *parent) :
    QsciAPIsEx(parent)
{
}

void LunarApi::updateAutoCompletionList(const QStringList &context,
            QStringList &list)
{
    QString path = context.join(".");
    QStringList::const_iterator it = prep->raw_apis.begin();

    QString pre = context.at(context.size()-1);

    while (it != prep->raw_apis.end())
    {
        QString base = QsciAPIsPrepared::apiBaseName(*it);

        if (!base.startsWith(path))
        {
            ++it;
            continue;
        }

        //put suitable items into list
        int start = path.length();
        if(start < base.length())
        {
            int pos = base.indexOf(".", start, Qt::CaseInsensitive);

            QString item = QObject::tr("");
            if(pos - start >= 0)
                item = pre + base.mid(start, pos-start);
            else
                item = pre + base.right(base.length() - start );

//                if(is_api_tip_)
//                {
//                    QString str_tip = "";
//                    pos = base.lastIndexOf(".", start, Qt::CaseInsensitive);
//                    if (pos > 0)
//                    {
//                        str_tip = " (" + base.left(pos) + ")";
//                    }
//                    item = item + str_tip;
//                }

            if (!list.contains(item))
                    list << item;
        }

        ++it;
    }
}

QStringList LunarApi::callTips(const QStringList &context, int commas,
        QsciScintilla::CallTipsStyle style,
        QList<int> &shifts)
{
    QStringList wseps = lexer()->autoCompletionWordSeparators();
    QStringList cts;
    QString path = context.join(".");
    while(path.endsWith("."))
    {
        path = path.left(path.length()-1);
    }
    QStringList::const_iterator it = prep->raw_apis.begin();
    QString prev;

    // Work out the length of the context.
    const QString &wsep = wseps.first();
    QStringList strip = path.split(wsep);
    strip.removeLast();
    int ctstart = strip.join(wsep).length();

    if (ctstart)
        ctstart += wsep.length();

    int shift;

    if (style == QsciScintilla::CallTipsContext)
    {
        shift = ctstart;
        ctstart = 0;
    }
    else
        shift = 0;

    // Make sure we only look at the functions we are interested in.
    path.append('(');

    while (it != prep->raw_apis.end())
    {
        if(!(*it).startsWith(path))
        {
            ++it;
            continue;
        }

        QString w = (*it).mid(ctstart);

        if (w != prev && enoughCommas(w, commas))
        {
            shifts << shift;
            cts << w;
            prev = w;
        }

        ++it;
    }

    return cts;
}

}
