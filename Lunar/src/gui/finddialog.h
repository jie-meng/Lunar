#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include "util/base.hpp"

class QCheckBox;
class QLabel;
class QPushButton;
class QTabWidget;

namespace gui
{
class FindTab;
class ReplaceTab;

class FindDialog : public QDialog
{
    Q_OBJECT
public:
    enum Tab {
        TabFind,
        TabReplace
    };

    explicit FindDialog(QWidget *parent = 0);
    virtual ~FindDialog();
Q_SIGNALS:
    void Find(const QString &str, bool first_find, Qt::CaseSensitivity cs, bool find_previous, bool whole_word, bool wrap, bool find_in_output);
    void Replace(const QString& replace_with_text, bool find_in_output);
    void ReplaceAll(const QString& text, const QString& replace_with_text, Qt::CaseSensitivity cs, bool find_previous, bool whole_word, bool find_in_output);
protected:
    virtual void closeEvent(QCloseEvent* e);
private Q_SLOTS:
    void TabFindTextChanged(const QString& text);
    void CmdFind(const QString& text);
    void CmdReplace(const QString& replace_with_text);
    void CmdReplaceAll(const QString& text, const QString& replace_with_text);
    void OptionsChanged();
    void TabChanged(int index);
private:
    void Init();
    void InitGui();
    void InitConnections();
private:
    QTabWidget* ptab_widget_;
    QCheckBox* pcase_check_box_;
    QCheckBox* pbackward_checkbox_;
    QCheckBox* pwholeword_checkbox_;
    QCheckBox* pwrap_around_checkbox_;
    QCheckBox* pfind_in_output_checkbox_;
    FindTab* pfind_tab_;
    ReplaceTab* preplace_tab_;
    bool first_find_;
    QString find_text_;
private:
    DISALLOW_COPY_AND_ASSIGN(FindDialog)
};

////////////////////////////////////////////////////
// class name : FindTab
// description :
// author :
// time : 2012-01-10-18.46
////////////////////////////////////////////////////
class FindTab : public QWidget
{
    Q_OBJECT

public:
    explicit FindTab(QWidget *parent = 0);
    virtual ~FindTab();
    QString GetFindText() { return  pfind_edit_->text(); }
    void SetFindText(const QString& text) { pfind_edit_->setText(text); }
Q_SIGNALS:
    void FindTextChangeSignal(const QString& text);
    void CmdFind(const QString&);
    void OptionsChanged();
private Q_SLOTS:
    void FindTextChanged(const QString&);
    void FindClicked();
private:
    void Init();
    void InitGui();
    void InitConnections();
private:
    QLabel *plabel_find_;
    QLineEdit *pfind_edit_;
    QPushButton *pfind_button_;
private:
    DISALLOW_COPY_AND_ASSIGN(FindTab)
};

////////////////////////////////////////////////////
// class name : ReplaceTab
// description :
// author :
// time : 2012-01-10-19.57
////////////////////////////////////////////////////
class ReplaceTab : public QWidget
{
    Q_OBJECT

public:
    explicit ReplaceTab(QWidget *parent = 0);
    virtual ~ReplaceTab();
    QString GetFindText() { return  pfind_edit_->text(); }
    void SetFindText(const QString& text) { pfind_edit_->setText(text); }
Q_SIGNALS:
    void FindTextChangeSignal(const QString& text);
    void CmdFind(const QString& text);
    void OptionsChanged();
    void CmdReplace(const QString& replace_with_text);
    void CmdReplaceAll(const QString& text, const QString& replace_with_text);
private Q_SLOTS:
    void TextChanged();
    void FindClicked();
    void ReplaceClicked();
    void ReplaceAllClicked();
private:
    void Init();
    void InitGui();
    void InitConnections();
private:
    QLabel *plabel_find_;
    QLabel *plabel_replace_;
    QLineEdit *pfind_edit_;
    QLineEdit *preplace_edit_;
    QPushButton *pfind_button_;
    QPushButton *preplace_button_;
    QPushButton *preplace_all_button_;
private:
    DISALLOW_COPY_AND_ASSIGN(ReplaceTab)
};
} // namespace gui

#endif // FINDDIALOG_H
