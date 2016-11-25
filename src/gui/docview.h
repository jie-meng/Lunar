#ifndef GUI_DOCVIEW_H
#define GUI_DOCVIEW_H

#include <QWidget>
#include <QFont>
#include <map>
#include "util/lexicalcast.hpp"
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

    inline QsciScintilla* getTextEdit() const { return ptext_edit_; }
    inline QString getPathname() const { return pathname_; }
    inline void setPathname(const QString& pathname) { pathname_ = pathname; }
    inline FileType getFileType() const { return file_type_; }
    inline std::string getExecutor() const { return executor_; }
    inline void setSaveDialogInitDir(const QString& dir) { save_dialog_init_dir_ = dir; }
    inline QString getSaveDialogInitDir() const { return save_dialog_init_dir_; }

    QString getTitle();
    int getCurrentLine();
    void focusOnText();
    void setEditTextFont(const QFont& font);
    void replace(const QString& replace_with_text);
    bool saveDoc();
    bool saveAsDoc();
    bool doSave(bool reset_lexer, const QString& pathname);
    bool find(const QString& expr,
              bool re,
              bool cs,
              bool wo,
              bool wrap,
              bool forward,
              bool first_find,
			  bool from_start
              );
    void commentSelection(bool comment_line_or_block);
    void commentSelectionLine();
    void commentSelectionBlock();
    QString getSelectedText() const;
    void gotoLine(int line);
    void focusOnEdit();
    bool getDefinitions(std::vector<std::string>& out_results);
    void selectCursorWord();
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
    QString removeTextReturn(const QString& text) const;
    QString getTitleFromPath(const QString& path) const;
    QsciLexer* getLexerFromTypeName(const std::string& type_name, FileType* pout_filetype);
    size_t getStartSpaceCount(const QString& str);
    template<typename T>
    static inline T getValueFromMap(const std::map<std::string, std::string>& from_map, const std::string& key, T default_value)
    {
        std::map<std::string, std::string>::const_iterator it = from_map.find(key);
        if (it == from_map.end())
            return default_value;
        else
            return util::lexicalCastDefault<T>(it->second, default_value);
    }

    QString save_dialog_init_dir_;
    QString pathname_;
    QString title_;
    QsciScintilla* ptext_edit_;
    QsciAPIsEx* papis_;
    QsciLexer* plexer_;
    static int s_new_docview_sequence_;
    bool is_apis_preparing_;
    ApiLoader* papi_loader_;
    FileType file_type_;
    std::string executor_;
    std::string parse_api_script_;
    std::string project_src_dir_;
    std::string goto_script_;
    QString comment_line_symbol_;
    QString comment_block_symbol_begin_;
    QString comment_block_symbol_end_;
    int selection_match_indicator_;
    int new_file_sequence_no_ = 0;
private:
    DISALLOW_COPY_AND_ASSIGN(DocView)
};

} // namespace gui

#endif // GUI_DOCVIEW_H
