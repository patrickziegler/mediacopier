#ifndef MEDIACOPIERDIALOG_H
#define MEDIACOPIERDIALOG_H

#include <QDialog>
#include <QApplication>

namespace Ui {
class MediaCopierDialog;
}

class MediaCopierDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MediaCopierDialog(QWidget *parent = nullptr);
    ~MediaCopierDialog();

private:
    Ui::MediaCopierDialog *ui;
};

#endif // MEDIACOPIERDIALOG_H
