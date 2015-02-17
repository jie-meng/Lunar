#ifndef GUI_DOCVIEW_H
#define GUI_DOCVIEW_H

#include <QWidget>
#include <QFont>
#include "util/base.hpp"

class QsciAPIs;
class QsciLexerLua;
class QsciScintilla;

namespace gui
{

class ApiLoader;

class DocView : public QWidget
{
    Q_OBJECT
public:
    explicit DocView(const QString& pathname, QWidget* parent = 0);
    virtual ~DocView();
    QsciScintilla* get_text_edit() const { return ptext_edit_; }
    QString get_pathname() const { return pathname_; }
    QString get_title() const { return title_; }
    void SetEditTextFont(const QFont& font);
    void Replace(const QString& replace_with_text);
    bool SaveDoc();
    bool SaveAsDoc();
    bool DoSave();
    bool Find(const QString& expr,
              bool re,
              bool cs,
              bool wo,
              bool wrap,
              bool forward,
              bool first_find,
			  bool from_start
              );

Q_SIGNALS:
    void UpdateTitle(DocView*);
    void TextModified(DocView*);
private Q_SLOTS:
    void TextChanged();
    void ApisPreparationFinished();
    void LinesChanged();
    void ResetMarginLineNumberWidth();
private:
    void UpdateApisProc();
    void UpdateApis();
    void Init();
    void InitGui();
    void InitTextEdit();
    void InitConnections();
    void ParseRequireFiles(const std::string& filepath);
    void SetLuaLexerAndPslApi();
    QString GetTitleFromPath(const QString& path) const;
    QString pathname_;
    QString title_;
    QsciScintilla* ptext_edit_;
    QsciAPIs* papis_;
    QsciLexerLua* plexer_;
    static int s_new_docview_sequence_;
    bool is_apis_preparing_;
    ApiLoader* papi_loader_;
private:
    DISALLOW_COPY_AND_ASSIGN(DocView)
};

} // namespace gui

#endif // GUI_DOCVIEW_H
