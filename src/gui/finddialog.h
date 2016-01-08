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
    void setFocusOnFindInput();
Q_SIGNALS:
    void find(const QString &str, bool first_find, Qt::CaseSensitivity cs, bool find_previous, bool whole_word, bool wrap, bool find_in_output);
    void replace(const QString& replace_with_text, bool find_in_output);
    void replaceAll(const QString& text, const QString& replace_with_text, Qt::CaseSensitivity cs, bool find_previous, bool whole_word, bool find_in_output);
protected:
    virtual void closeEvent(QCloseEvent* e);
    virtual void showEvent(QShowEvent* e);
private Q_SLOTS:
    void tabFindTextChanged(const QString& text);
    void cmdFind(const QString& text);
    void cmdReplace(const QString& replace_with_text);
    void cmdReplaceAll(const QString& text, const QString& replace_with_text);
    void optionsChanged();
    void tabChanged(int index);
private:
    void init();
    void initGui();
    void initConnections();
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
    QString getFindText() { return  pfind_edit_->text(); }
    void setFindText(const QString& text) { pfind_edit_->setText(text); }
    void setFocusOnFindInput();
Q_SIGNALS:
    void findTextChangeSignal(const QString& text);
    void cmdFind(const QString&);
    void optionsChanged();
protected:
    virtual void showEvent(QShowEvent* e);
private Q_SLOTS:
    void findTextChanged(const QString&);
    void findClicked();
private:
    void init();
    void initGui();
    void initConnections();
    void setFindTextSelected();
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
    QString getFindText() { return  pfind_edit_->text(); }
    void setFindText(const QString& text) { pfind_edit_->setText(text); }
Q_SIGNALS:
    void findTextChangeSignal(const QString& text);
    void cmdFind(const QString& text);
    void optionsChanged();
    void cmdReplace(const QString& replace_with_text);
    void cmdReplaceAll(const QString& text, const QString& replace_with_text);
private Q_SLOTS:
    void textChanged();
    void findClicked();
    void replaceClicked();
    void replaceAllClicked();
private:
    void init();
    void initGui();
    void initConnections();
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
