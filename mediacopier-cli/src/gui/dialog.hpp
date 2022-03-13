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

#pragma once

#include "../core/worker.hpp"

#include <QDialog>

namespace Ui {
class MediaCopierDialog;
}

class QStateMachine;

class MediaCopierDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MediaCopierDialog(QWidget *parent=nullptr);
    ~MediaCopierDialog();
    void init(Config* config);

public Q_SLOTS:
    void aboutToQuit();
    void update(Status info);

private Q_SLOTS:
    void startOperation();
    void cancelOperation();

Q_SIGNALS:
    void operationDone();

private:
    Ui::MediaCopierDialog* ui = nullptr;
    QStateMachine* fsm = nullptr;
    Config* config = nullptr;
    Worker* worker = nullptr;
};
