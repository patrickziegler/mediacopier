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

#include <mediacopier/version.hpp>
#include <mediacopier/gui/dialog.hpp>

#include <QAbstractButton>
#include <QFileDialog>
#include <QPushButton>

#include "ui_dialog.h"

namespace cli = MediaCopier::Cli;

using CMapItem = QPair<QString, cli::Command>;

static constexpr const size_t DEFAULT_DIALOG_WIDTH = 500;
static constexpr const size_t DEFAULT_DIALOG_HEIGHT = 550;

static const QList<CMapItem> commands = {
    CMapItem("Copy", cli::Command::COPY),
    CMapItem("Move", cli::Command::MOVE)
};

MediaCopierDialog::MediaCopierDialog(MediaCopier::Cli::ConfigManager config, QWidget *parent) :
    QDialog(parent), ui(new Ui::MediaCopierDialog), m_config(config)
{
    ui->setupUi(this);

    auto title = QString{"%1 v%2"}.arg(MEDIACOPIER_PROJECT_NAME, MEDIACOPIER_VERSION);

    this->setWindowTitle(title);
    this->resize(DEFAULT_DIALOG_WIDTH, DEFAULT_DIALOG_HEIGHT);

    ui->lineInputDir->setText(m_config.inputDir().c_str());
    ui->lineOutputDir->setText(m_config.outputDir().c_str());
    ui->lineBaseFormat->setText(m_config.baseFormat().c_str());

    Q_FOREACH(CMapItem item, commands) {
        ui->comboCommand->addItem(item.first);
    }

    m_config.setCommand(commands.at(0).second);

    m_thread = new QThread();
    m_worker = new Worker(m_config);

    m_worker->moveToThread(m_thread);

    connect(m_thread, SIGNAL(started()),
            m_worker, SLOT(start()));

    connect(m_worker, SIGNAL(finishedSignal()),
            this, SLOT(onWorkerFinished()));

    connect(m_worker, SIGNAL(logInfoMessage(QString)),
            this, SLOT(onWorkerLogInfo(QString)));

    connect(m_worker, SIGNAL(logWarningMessage(QString)),
            this, SLOT(onWorkerLogWarning(QString)));

    connect(m_worker, SIGNAL(logErrorMessage(QString)),
            this, SLOT(onWorkerLogError(QString)));

    connect(m_worker, SIGNAL(progressValue(int)),
            this, SLOT(onWorkerProgress(int)));

    connect(ui->buttonOpenInputDir, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(onOpenInputDirClicked()));

    connect(ui->buttonOpenOutputDir, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(onOpenOutputDirClicked()));

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

    m_btnOk = ui->buttonDialogControl->button(QDialogButtonBox::Ok);
}

MediaCopierDialog::~MediaCopierDialog()
{
    delete ui;
    delete m_thread;
    delete m_worker;
}

void MediaCopierDialog::onOpenInputDirClicked()
{
    QString dir = QFileDialog::getExistingDirectory(
                this, "Open Directory",QDir::currentPath(),
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    ui->lineInputDir->setText(dir);
}

void MediaCopierDialog::onOpenOutputDirClicked()
{
    QString dir = QFileDialog::getExistingDirectory(
                this, "Open Directory", QDir::currentPath(),
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    ui->lineOutputDir->setText(dir);
}

void MediaCopierDialog::onDialogControlClicked(QAbstractButton *button)
{
    if (ui->buttonDialogControl->standardButton(button) == QDialogButtonBox::Ok) {
        // TODO: use state machine for this
        m_btnOk->setEnabled(false);
        m_thread->start();
    } else if(!m_thread->isRunning()) {
        close();
    } else {
        m_worker->cancel();
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

void MediaCopierDialog::onWorkerFinished()
{
    m_thread->quit();
    m_btnOk->setEnabled(true);
}

void MediaCopierDialog::onWorkerLogInfo(QString message)
{
    ui->textLog->appendPlainText(message);
}

void MediaCopierDialog::onWorkerLogWarning(QString message)
{
    ui->textLog->appendPlainText("WARNING: " + message);
}

void MediaCopierDialog::onWorkerLogError(QString message)
{
    ui->textLog->appendPlainText("ERROR: " + message);
}

void MediaCopierDialog::onWorkerProgress(int value)
{
    ui->barProgress->setValue(value);
}
