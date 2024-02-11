/* Copyright (C) 2022 Patrick Ziegler <zipat@proton.me>
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

#include "gui/MediaCopierDialogFull.hpp"

#include <mediacopier/version.hpp>

#include <QApplication>
#include <QFinalState>
#include <QPushButton>
#include <QScreen>

#include <spdlog/spdlog.h>

// auto generated by moc
#include "ui_MediaCopierDialogFull.h"

static constexpr const size_t DEFAULT_DIALOG_WIDTH = 650;
static constexpr const size_t DEFAULT_DIALOG_HEIGHT = 550;
static constexpr const unsigned int DEFAULT_WAIT_MS = 200;

MediaCopierDialogFull::MediaCopierDialogFull(QWidget *parent) :
    QDialog(parent), ui(new Ui::MediaCopierDialogFull)
{
    ui->setupUi(this);
    this->resize(DEFAULT_DIALOG_WIDTH, DEFAULT_DIALOG_HEIGHT);
    this->setWindowTitle(mediacopier::MEDIACOPIER_PROJECT_NAME);
    move(screen()->geometry().center() - frameGeometry().center());
}

MediaCopierDialogFull::~MediaCopierDialogFull()
{
    delete ui;
}

void MediaCopierDialogFull::init(std::shared_ptr<Config> config, QApplication& app)
{
    QObject::connect(&app, &QApplication::aboutToQuit, this, &MediaCopierDialogFull::aboutToQuit);

    this->config = std::move(config);
    ui->param->init(this->config);

    fsm = std::make_unique<QStateMachine>();

    auto s1 = new QState();         // waiting for input
    auto s2 = new QState();         // executing operation
    auto s3 = new QState();         // aborting operation
    auto s4 = new QFinalState();    // closing dialog

    s1->addTransition(ui->dialogButtonBox, &QDialogButtonBox::accepted, s2);
    s1->addTransition(ui->dialogButtonBox, &QDialogButtonBox::rejected, s4);
    s2->addTransition(ui->dialogButtonBox, &QDialogButtonBox::rejected, s3);
    s2->addTransition(this, &MediaCopierDialogFull::rejected, s3);
    s2->addTransition(this, &MediaCopierDialogFull::operationDone, s1);
    s3->addTransition(this, &MediaCopierDialogFull::operationDone, s1);

    auto m_btnOk = ui->dialogButtonBox->button(QDialogButtonBox::Ok);
    auto m_btnCancel = ui->dialogButtonBox->button(QDialogButtonBox::Cancel);

    s1->assignProperty(m_btnOk, "enabled", true);
    s1->assignProperty(m_btnCancel, "enabled", true);
    s2->assignProperty(m_btnOk, "enabled", false);
    s3->assignProperty(m_btnCancel, "enabled", false);

    QObject::connect(s1, &QState::entered, this, &MediaCopierDialogFull::awaitOperation);
    QObject::connect(s2, &QState::entered, this, &MediaCopierDialogFull::startOperation);
    QObject::connect(s3, &QState::entered, this, &MediaCopierDialogFull::cancelOperation);
    QObject::connect(fsm.get(), &QStateMachine::finished, this, &MediaCopierDialogFull::close);

    fsm->addState(s1);
    fsm->addState(s2);
    fsm->addState(s3);
    fsm->addState(s4);
    fsm->setInitialState(s1);
    fsm->start();
}

void MediaCopierDialogFull::aboutToQuit()
{
    while (fsm->active())
        this->thread()->msleep(DEFAULT_WAIT_MS);
}

void MediaCopierDialogFull::startOperation()
{
    this->ui->log->clear();
    worker = std::make_shared<Worker>(*config);
    QObject::connect(worker.get(), &Worker::status, this->ui->log, &MediaCopierLogWidget::update);
    QObject::connect(worker.get(), &Worker::finished, this, &MediaCopierDialogFull::operationDone);
    worker->start();
}

void MediaCopierDialogFull::cancelOperation()
{
    worker->kill();
}

void MediaCopierDialogFull::awaitOperation()
{
    spdlog::default_logger()->flush();
}
