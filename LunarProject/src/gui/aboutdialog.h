#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include "util/base.hpp"

class QLabel;

namespace gui
{

class AboutDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AboutDialog(QWidget *parent = 0);
    virtual ~AboutDialog();
    void Init();
    void InitGui();
private:
    QLabel *plabel_email_;
    QLabel *plabel_version_;
    QLabel *plabel_platform_;
private:
    DISALLOW_COPY_AND_ASSIGN(AboutDialog)
};

} // namespace gui

#endif // ABOUTDIALOG_H
