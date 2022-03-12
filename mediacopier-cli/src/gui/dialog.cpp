/* Copyright (C) 2022 Patrick Ziegler
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
#include "../core/config.hpp"

#include <mediacopier/version.hpp>

#include <QFileDialog>
#include <QScreen>
#include <QMap>

#include <spdlog/sinks/qt_sinks.h>

static constexpr const size_t DEFAULT_DIALOG_WIDTH = 600;
static constexpr const size_t DEFAULT_DIALOG_HEIGHT = 600;

using CommandMapItem = QPair<QString, Config::Command>;

static const QList<CommandMapItem> commands = {
    CommandMapItem(Config::commandString(Config::Command::COPY_JPEG), Config::Command::COPY_JPEG),
    CommandMapItem(Config::commandString(Config::Command::MOVE_JPEG), Config::Command::MOVE_JPEG),
    CommandMapItem(Config::commandString(Config::Command::SHOW), Config::Command::SHOW)
};

const auto ask_for_directory = [](const QString& title) -> QString
{
    return QFileDialog::getExistingDirectory(
                0, title, QDir::currentPath(),
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
};

MediaCopierDialog::MediaCopierDialog(Config* config, QWidget *parent) :
    QDialog(parent), ui(new Ui::MediaCopierDialog), m_config{config}
{
    ui->setupUi(this);

    m_logger = spdlog::qt_logger_mt("qt_logger", ui->logText);
    spdlog::set_default_logger(m_logger);
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%l] %v");

    this->setWindowTitle(mediacopier::MEDIACOPIER_PROJECT_NAME);
    this->resize(DEFAULT_DIALOG_WIDTH, DEFAULT_DIALOG_HEIGHT);
    move(screen()->geometry().center() - frameGeometry().center());

    Q_FOREACH(CommandMapItem item, commands) {
        ui->paramCommand->addItem(item.first);
    }

    syncConfig();

    connect(ui->dirsInputDirButton, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(onOpenInputDirClicked()));

    connect(ui->dirsOutputDirButton, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(onOpenOutputDirClicked()));

    connect(ui->dirsInputDirText, SIGNAL(textChanged(const QString&)),
            this, SLOT(onInputDirChanged(const QString&)));

    connect(ui->dirsOutputDirText, SIGNAL(textChanged(const QString&)),
            this, SLOT(onOutputDirChanged(const QString&)));

    connect(ui->paramBaseFormat, SIGNAL(textChanged(const QString&)),
            this, SLOT(onBaseFormatChanged(const QString&)));

    connect(ui->paramCommand, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onCommandChanged(int)));
}

MediaCopierDialog::~MediaCopierDialog()
{
    if (ui)
        delete ui;
}

void MediaCopierDialog::syncConfig()
{
    m_config->readConfigFile();

    ui->dirsInputDirText->setText(m_config->inputDir().c_str());
    ui->dirsOutputDirText->setText(m_config->outputDir().c_str());
    ui->paramBaseFormat->setText(m_config->pattern().c_str());

    for (size_t i = 0; i < commands.length(); ++i) {
        if (commands.at(i).second == m_config->command()) {
            ui->paramCommand->setCurrentIndex(i);
            break;
        }
    }
}

void MediaCopierDialog::onOpenInputDirClicked()
{
    ui->dirsInputDirText->setText(
                ask_for_directory(QObject::tr("Source folder")));
}

void MediaCopierDialog::onOpenOutputDirClicked()
{
    ui->dirsOutputDirText->setText(
                ask_for_directory(QObject::tr("Destination folder")));
}

void MediaCopierDialog::onInputDirChanged(const QString& text)
{
    m_config->setInputDir(text);
    spdlog::debug("Changed input dir to " + m_config->outputDir().string());
    syncConfig();
}

void MediaCopierDialog::onOutputDirChanged(const QString& text)
{
    m_config->setOutputDir(text);
    spdlog::debug("Changed output dir to " + m_config->outputDir().string());
    syncConfig();
}

void MediaCopierDialog::onBaseFormatChanged(const QString& text)
{
    m_config->setPattern(text);
    spdlog::debug("Changed base format to " + m_config->pattern());
    syncConfig();
}

void MediaCopierDialog::onCommandChanged(int index)
{
    m_config->setCommand(commands.at(index).second);
    spdlog::debug("Changed command to " + commands.at(index).first.toStdString());
    syncConfig();
}
