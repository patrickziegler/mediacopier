#include <mediacopier/gui/mediacopierdialog.hpp>
#include "ui_mediacopierdialog.h"

MediaCopierDialog::MediaCopierDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MediaCopierDialog)
{
    ui->setupUi(this);
}

MediaCopierDialog::~MediaCopierDialog()
{
    delete ui;
}
