#ifndef GUI_DOCVIEW_H
#define GUI_DOCVIEW_H

#include <QWidget>
#include <QFont>
#include "util/base.hpp"
#include "filetype.h"

class QsciLexer;
class QsciScintilla;
class QsciAbstractAPIs;
class QsciAPIsEx;

namespace gui
{

class QsciAPIsEx;
class ApiLoader;

class DocView : public QWidget
{
    Q_OBJECT
public:
    explicit DocView(const QString& pathname, QWidget* parent = 0);
    virtual ~DocView();
    QsciScintilla* getTextEdit() const { return ptext_edit_; }
    QString getPathname() const { return pathname_; }
    QString getTitle() const { return title_; }
    void setEditTextFont(const QFont& font);
    void replace(const QString& replace_with_text);
    bool saveDoc();
    bool saveAsDoc();
    bool doSave(bool reset_lexer);
    bool find(const QString& expr,
              bool re,
              bool cs,
              bool wo,
              bool wrap,
              bool forward,
              bool first_find,
			  bool from_start
              );
    inline FileType getFileType() const { return file_type_; }
    inline std::string getExecutor() const { return executor_; }
    inline void setSaveDialogInitDir(const QString& dir) { save_dialog_init_dir_ = dir; }
    inline QString getSaveDialogInitDir() const { return save_dialog_init_dir_; }
Q_SIGNALS:
    void updateTitle(DocView*);
    void textModified(DocView*);
private Q_SLOTS:
    void textChanged();
    void apisPreparationFinished();
    void linesChanged();
    void resetMarginLineNumberWidth();
    void selectionChanged();
private:
    void init();
    void initGui();
    void initTextEdit();
    void initConnections();
    void clearLexerApi();
    void setLexerApi();
    bool testFileFilter(const std::string& file_filter);
    void resetLexer();
    void refreshSupplementApi();
    QString getTitleFromPath(const QString& path) const;
    QsciLexer* getLexerFromTypeName(const std::string& type_name, FileType* pout_filetype);

    QString save_dialog_init_dir_;
    QString pathname_;
    QString title_;
    QString selected_text_;
    QsciScintilla* ptext_edit_;
    QsciAPIsEx* papis_;
    QsciLexer* plexer_;
    static int s_new_docview_sequence_;
    bool is_apis_preparing_;
    ApiLoader* papi_loader_;
    FileType file_type_;
    std::string executor_;
    std::string parse_supplement_api_func_;
    std::string parse_supplement_api_script_;
private:
    DISALLOW_COPY_AND_ASSIGN(DocView)
};

} // namespace gui

#endif // GUI_DOCVIEW_H
