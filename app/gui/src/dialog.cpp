/* Copyright (C) 2020 Patrick Ziegler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <mediacopier/gui/dialog.hpp>
#include "ui_dialog.h"

#include <QFileDialog>

namespace mcc = MediaCopier::Cli;

using CMapItem = QPair<QString, mcc::ConfigStore::Command>;

static const QList<CMapItem> commands = {
    CMapItem("Copy", mcc::ConfigStore::Command::COPY),
    CMapItem("Move", mcc::ConfigStore::Command::MOVE)
};

MediaCopierDialog::MediaCopierDialog(MediaCopier::Cli::ConfigStore config, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MediaCopierDialog),
    m_config(config)
{
    ui->setupUi(this);

    ui->lineInputDir->setText(QString::fromStdString(m_config.inputDir().string()));
    ui->lineOutputDir->setText(QString::fromStdString(m_config.outputDir().string()));
    ui->lineBaseFormat->setText(QString::fromStdString(m_config.baseFormat()));

    Q_FOREACH(CMapItem item, commands)
    {
        ui->comboOperation->addItem(item.first);
    }

    QObject::connect(ui->comboOperation, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(do_smth(int)));
}

MediaCopierDialog::~MediaCopierDialog()
{
    delete ui;
}

void MediaCopierDialog::do_smth(int a)
{
    if (commands.at(a).second == mcc::ConfigStore::Command::COPY) {
        ui->textLog->appendPlainText("copy selected");
    } else {
        ui->textLog->appendPlainText("move selected");
    }
}
