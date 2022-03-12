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
#include "../core/worker.hpp"
#include "../core/config.hpp"

#include <mediacopier/version.hpp>

#include <QFileDialog>
#include <QFinalState>
#include <QMap>
#include <QPushButton>
#include <QScreen>
#include <QStateMachine>
#include <QThread>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/qt_sinks.h>
#include <spdlog/sinks/stdout_color_sinks.h>

static constexpr const size_t DEFAULT_DIALOG_WIDTH = 600;
static constexpr const size_t DEFAULT_DIALOG_HEIGHT = 600;
static constexpr const unsigned int DEFAULT_WAIT_MS = 200;

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

    auto logger = spdlog::qt_logger_mt("mediacopier", ui->logText);
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::debug);
    logger->sinks().push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    logger->sinks().at(0).get()->set_pattern("[%l] %v");
    logger->sinks().at(0).get()->set_level(spdlog::level::info);

    this->setWindowTitle(mediacopier::MEDIACOPIER_PROJECT_NAME);
    this->resize(DEFAULT_DIALOG_WIDTH, DEFAULT_DIALOG_HEIGHT);
    move(screen()->geometry().center() - frameGeometry().center());

    Q_FOREACH(CommandMapItem item, commands) {
        ui->paramCommand->addItem(item.first);
    }

    syncConfig();

    connect(ui->dirsInputDirButton, &QDialogButtonBox::clicked,
            this, &MediaCopierDialog::onOpenInputDirClicked);

    connect(ui->dirsOutputDirButton, &QDialogButtonBox::clicked,
            this, &MediaCopierDialog::onOpenOutputDirClicked);

    connect(ui->dirsInputDirText, &QLineEdit::textChanged,
            this, &MediaCopierDialog::onInputDirChanged);

    connect(ui->dirsOutputDirText, &QLineEdit::textChanged,
            this, &MediaCopierDialog::onOutputDirChanged);

    connect(ui->paramBaseFormat, &QLineEdit::textChanged,
            this, &MediaCopierDialog::onBaseFormatChanged);

    connect(ui->paramCommand, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MediaCopierDialog::onCommandChanged);

    initStateMachine();
}

MediaCopierDialog::~MediaCopierDialog()
{
    if (ui)
        delete ui;
    if (fsm)
        delete fsm;
    if (m_worker)
        delete m_worker;
}

void MediaCopierDialog::initStateMachine()
{
    fsm = new QStateMachine();

    auto s1 = new QState();
    auto s2 = new QState();
    auto s3 = new QState();
    auto s4 = new QFinalState();

    s1->addTransition(ui->dialogButtonBox, &QDialogButtonBox::rejected, s4);
    s1->addTransition(ui->dialogButtonBox, &QDialogButtonBox::accepted, s2);
    s2->addTransition(ui->dialogButtonBox, &QDialogButtonBox::rejected, s3);
    s2->addTransition(this, &MediaCopierDialog::rejected, s3);
    s2->addTransition(this, &MediaCopierDialog::operationDone, s1);
    s3->addTransition(this, &MediaCopierDialog::operationDone, s1);

    auto m_btnOk = ui->dialogButtonBox->button(QDialogButtonBox::Ok);
    auto m_btnCancel = ui->dialogButtonBox->button(QDialogButtonBox::Cancel);

    s1->assignProperty(m_btnOk, "enabled", true);
    s1->assignProperty(m_btnCancel, "enabled", true);
    s2->assignProperty(m_btnOk, "enabled", false);
    s3->assignProperty(m_btnCancel, "enabled", false);

    QObject::connect(s2, &QState::entered, this, &MediaCopierDialog::startOperation);
    QObject::connect(s3, &QState::entered, this, &MediaCopierDialog::cancelOperation);
    QObject::connect(fsm, &QStateMachine::finished, this, &MediaCopierDialog::close);

    fsm->addState(s1);
    fsm->addState(s2);
    fsm->addState(s3);
    fsm->addState(s4);
    fsm->setInitialState(s1);
    fsm->start();
}

void MediaCopierDialog::aboutToQuit()
{
    while (fsm->active())
        this->thread()->msleep(DEFAULT_WAIT_MS);
}

void MediaCopierDialog::syncConfig()
{
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
}

void MediaCopierDialog::onOutputDirChanged(const QString& text)
{
    m_config->setOutputDir(text);
    spdlog::debug("Changed output dir to " + m_config->outputDir().string());
    m_config->readConfigFile();
    syncConfig();
}

void MediaCopierDialog::onBaseFormatChanged(const QString& text)
{
    m_config->setPattern(text);
    spdlog::debug("Changed base format to " + m_config->pattern());
}

void MediaCopierDialog::onCommandChanged(int index)
{
    m_config->setCommand(commands.at(index).second);
    spdlog::debug("Changed command to " + commands.at(index).first.toStdString());
}

void MediaCopierDialog::startOperation()
{
    if (m_worker)
        delete m_worker;

    m_worker = new Worker{*m_config};
    QObject::connect(m_worker, &Worker::finished, this, &MediaCopierDialog::operationDone);

    m_worker->start();
}

void MediaCopierDialog::cancelOperation()
{
    m_worker->kill();
}
