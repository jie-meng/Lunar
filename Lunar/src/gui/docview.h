#ifndef GUI_DOCVIEW_H
#define GUI_DOCVIEW_H

#include <QWidget>
#include <QFont>
#include "util/base.hpp"

class QsciLexer;
class QsciScintilla;
class QsciAbstractAPIs;

namespace gui
{

class QsciAPIsEx;
class ApiLoader;

class DocView : public QWidget
{
    Q_OBJECT
public:
    enum FileType
    {
        Unknown,
        Avs,
        Bash,
        Batch,
        CMake,
        CPP,
        CSharp,
        CSS,
        D,
        Diff,
        Html,
        Java,
        JavaScript,
        Lua,
        Makefile,
        Matlab,
        Octave,
        Pascal,
        Perl,
        Properties,
        Python,
        Ruby,
        Sql,
        Tcl,
        Tex,
        Verilog,
        Vhdl,
        Xml,
        Yaml
    };

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
    inline FileType GetFileType() const { return file_type_; }
    inline std::string GetExecutor() const { return executor_; }
Q_SIGNALS:
    void UpdateTitle(DocView*);
    void TextModified(DocView*);
private Q_SLOTS:
    void TextChanged();
    void ApisPreparationFinished();
    void LinesChanged();
    void ResetMarginLineNumberWidth();
private:
    void Init();
    void InitGui();
    void InitTextEdit();
    void InitConnections();
    void ClearLexerApi();
    void SetLuaLexerApi();
    void SetOctaveLexerApi();
    void SetUnknownLexerApi();
    bool TestFileFilter(const std::string& file_filter);
    void ResetLexer();
    QString GetTitleFromPath(const QString& path) const;
    QsciLexer* GetLexerFromTypeName(const std::string& type_name, FileType* pout_filetype);

    QString pathname_;
    QString title_;
    QsciScintilla* ptext_edit_;
    QsciAbstractAPIs* papis_;
    QsciLexer* plexer_;
    static int s_new_docview_sequence_;
    bool is_apis_preparing_;
    ApiLoader* papi_loader_;
    FileType file_type_;
    std::string executor_;
private:
    DISALLOW_COPY_AND_ASSIGN(DocView)
};

} // namespace gui

#endif // GUI_DOCVIEW_H
