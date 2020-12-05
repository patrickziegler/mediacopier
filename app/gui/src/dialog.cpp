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
#include <mediacopier/version.hpp>

#include <QAbstractButton>
#include <QFileDialog>
#include <QPushButton>

#include "ui_dialog.h"

namespace cli = MediaCopier::Cli;

using CMapItem = QPair<QString, cli::ConfigStore::Command>;

static const QList<CMapItem> commands = {
    CMapItem("Copy", cli::ConfigStore::Command::COPY),
    CMapItem("Move", cli::ConfigStore::Command::MOVE)
};

MediaCopierDialog::MediaCopierDialog(MediaCopier::Cli::ConfigStore config, QWidget *parent) :
    QDialog(parent), ui(new Ui::MediaCopierDialog), m_config(config)
{
    ui->setupUi(this);

    this->setWindowTitle(MEDIACOPIER_PROJECT_NAME);
    this->resize(400, 500);

    ui->lineInputDir->setText(QString::fromStdString(m_config.inputDir().string()));
    ui->lineOutputDir->setText(QString::fromStdString(m_config.outputDir().string()));
    ui->lineBaseFormat->setText(QString::fromStdString(m_config.baseFormat()));

    Q_FOREACH(CMapItem item, commands) {
        ui->comboCommand->addItem(item.first);
    }

    m_config.setCommand(commands.at(0).second);

    ui->buttonDialogControl->button(QDialogButtonBox::Cancel)->setEnabled(false);

    m_thread = new QThread();
    m_worker = new Worker();
    m_worker->moveToThread(m_thread);

    connect(m_thread, SIGNAL(started()),
            m_worker, SLOT(run()));

    connect(m_worker, SIGNAL(finished()),
            this, SLOT(onThreadFinished()));

    connect(m_worker, SIGNAL(error(QString)),
            this, SLOT(onWorkerError(QString)));

    connect(m_worker, SIGNAL(log(QString)),
            this, SLOT(onWorkerLog(QString)));

    connect(m_worker, SIGNAL(progress(double)),
            this, SLOT(onWorkerProgress(double)));

    connect(ui->buttonOpenInputDir, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(onOpenInputDirClicked(QAbstractButton*)));

    connect(ui->buttonOpenOutputDir, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(onOpenOutputDirClicked(QAbstractButton*)));

    connect(ui->buttonDialogControl, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(onDialogControlClicked(QAbstractButton*)));

    connect(ui->lineInputDir, SIGNAL(textChanged(const QString&)),
            this, SLOT(onInputDirChanged(const QString&)));

    connect(ui->lineOutputDir, SIGNAL(textChanged(const QString&)),
            this, SLOT(onOutputDirChanged(const QString&)));

    connect(ui->lineBaseFormat, SIGNAL(textChanged(const QString&)),
            this, SLOT(onBaseFormatChanged(const QString&)));

    connect(ui->comboCommand, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onOperationChanged(int)));
}

MediaCopierDialog::~MediaCopierDialog()
{
    delete ui;
    delete m_thread;
    delete m_worker;
}

void MediaCopierDialog::onOpenInputDirClicked(QAbstractButton *button)
{
    QString dir = QFileDialog::getExistingDirectory(this, "Open Directory", QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->lineInputDir->setText(dir);
}

void MediaCopierDialog::onOpenOutputDirClicked(QAbstractButton *button)
{
    QString dir = QFileDialog::getExistingDirectory(this, "Open Directory", QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->lineOutputDir->setText(dir);
}

void MediaCopierDialog::onDialogControlClicked(QAbstractButton *button)
{
    if (ui->buttonDialogControl->standardButton(button) == QDialogButtonBox::Ok) {
        ui->buttonDialogControl->button(QDialogButtonBox::Ok)->setEnabled(false);
        m_worker->useConfig(m_config);
        m_thread->start();
        ui->buttonDialogControl->button(QDialogButtonBox::Cancel)->setEnabled(true);
    } else {
        ui->buttonDialogControl->button(QDialogButtonBox::Cancel)->setEnabled(false);
        m_worker->abort();
    }
}

void MediaCopierDialog::onInputDirChanged(const QString& text)
{
    m_config.setInputDir(text.toStdString());
}

void MediaCopierDialog::onOutputDirChanged(const QString& text)
{
    m_config.setOutputDir(text.toStdString());
}

void MediaCopierDialog::onBaseFormatChanged(const QString& text)
{
    m_config.setBaseFormat(text.toStdString());
}

void MediaCopierDialog::onOperationChanged(int index)
{
    m_config.setCommand(commands.at(index).second);
}

void MediaCopierDialog::onThreadFinished()
{
    m_thread->quit();
    ui->buttonDialogControl->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void MediaCopierDialog::onWorkerError(QString message)
{

}

void MediaCopierDialog::onWorkerLog(QString message)
{
    ui->textLog->appendPlainText(message);
}

void MediaCopierDialog::onWorkerProgress(double progress)
{

}
