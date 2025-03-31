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

#pragma once

#include "worker.hpp"

#include <QDialog>
#include <QStateMachine>

namespace Ui {
class MediaCopierDialogFull;
}

class QApplication;

class MediaCopierDialogFull : public QDialog
{
    Q_OBJECT

public:
    explicit MediaCopierDialogFull(QWidget *parent=nullptr);
    ~MediaCopierDialogFull();
    void init(std::shared_ptr<Config> config);

private Q_SLOTS:
    void startOperation();
    void cancelOperation();
    void awaitOperation();

Q_SIGNALS:
    void finishedOperation();

private:
    Ui::MediaCopierDialogFull* ui;
    QStateMachine* fsm = nullptr;
    std::shared_ptr<Config> m_config = nullptr;
    std::unique_ptr<Worker> m_worker = nullptr;
};
