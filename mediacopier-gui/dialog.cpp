/* Copyright (C) 2020-2021 Patrick Ziegler
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

#include "dialog.hpp"

#include <mediacopier/version.hpp>

#include <QFileDialog>
#include <QPushButton>
#include <QThread>
#include <QScreen>

#include <spdlog/sinks/qt_sinks.h>

static constexpr const size_t DEFAULT_DIALOG_WIDTH = 800;
static constexpr const size_t DEFAULT_DIALOG_HEIGHT = 600;

using CMapItem = QPair<QString, Worker::Command>;

static const QList<CMapItem> commands = {
    CMapItem("Copy", Worker::Command::COPY_JPEG),
    CMapItem("Move", Worker::Command::MOVE_JPEG),
    CMapItem("Simulate", Worker::Command::SIMULATE)
};

MediaCopierDialog::MediaCopierDialog(Worker *worker, QWidget *parent) :
    QDialog(parent), ui(new Ui::MediaCopierDialog), m_worker(worker)
{
    ui->setupUi(this);

    m_logger = spdlog::qt_logger_mt("qt_logger", ui->textLog);
    spdlog::set_default_logger(m_logger);
    spdlog::set_pattern("[%l] %v");

    this->setWindowTitle(mediacopier::MEDIACOPIER_PROJECT_NAME);

    this->resize(DEFAULT_DIALOG_WIDTH, DEFAULT_DIALOG_HEIGHT);
    move(screen()->geometry().center() - frameGeometry().center());

    Q_FOREACH(CMapItem item, commands) {
        ui->comboCommand->addItem(item.first);
    }

    for (size_t i = 0; i < commands.length(); ++i) {
        if (commands.at(i).second == m_worker->m_command) {
            ui->comboCommand->setCurrentIndex(i);
            break;
        }
    }

    // resyncing the selected operation
    m_worker->m_command = commands.at(ui->comboCommand->currentIndex()).second;

    ui->lineInputDir->setText(m_worker->m_inputDir);
    ui->lineOutputDir->setText(m_worker->m_outputDir);
    ui->lineBaseFormat->setText(m_worker->m_pattern);

    m_thread = new QThread();
    m_worker->moveToThread(m_thread);

    connect(m_thread, SIGNAL(started()),
            m_worker, SLOT(onOperationStarted()));

    // connect dialog related slots

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

    // connect worker related slots

    connect(m_worker, SIGNAL(operationFinished()),
            this, SLOT(onOperationFinished()));

    connect(m_worker, SIGNAL(bumpProgress()),
            this, SLOT(onBumpProgress()));

    connect(m_worker, SIGNAL(resetProgress(int)),
            this, SLOT(onResetProgress(int)));

    m_btnOk = ui->buttonDialogControl->button(QDialogButtonBox::Ok);
}

MediaCopierDialog::~MediaCopierDialog()
{
    if (m_btnOk)
        delete m_btnOk;
    if (m_thread)
        delete m_thread;
    if (ui)
        delete ui;
}

void MediaCopierDialog::onOpenInputDirClicked()
{
    QString dir = QFileDialog::getExistingDirectory(
                this, "Open Directory", QDir::currentPath(),
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
        ui->textLog->setPlainText("");
        ui->barProgress->setValue(0);

        m_btnOk->setEnabled(false);
        m_thread->start();

    } else if(!m_thread->isRunning()) {
        close();
    } else {
        m_worker->cancelOperation();
    }
}

void MediaCopierDialog::onInputDirChanged(const QString& text)
{
    m_worker->m_inputDir = text;
}

void MediaCopierDialog::onOutputDirChanged(const QString& text)
{
    m_worker->m_outputDir = text;
}

void MediaCopierDialog::onBaseFormatChanged(const QString& text)
{
    m_worker->m_pattern = text;
}

void MediaCopierDialog::onOperationChanged(int index)
{
    m_worker->m_command = commands.at(index).second;
}

void MediaCopierDialog::onBumpProgress()
{
    auto value = ui->barProgress->value();
    ui->barProgress->setValue(value + 1);
}

void MediaCopierDialog::onResetProgress(int value)
{
    ui->barProgress->setValue(0);
    ui->barProgress->setMaximum(value);
}

void MediaCopierDialog::onOperationFinished()
{
    m_thread->quit();
    m_btnOk->setEnabled(true);
}
