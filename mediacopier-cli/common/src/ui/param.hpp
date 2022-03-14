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

#include <QWidget>

namespace Ui {
class MediaCopierParam;
}

class Config;

class MediaCopierParam : public QWidget
{
    Q_OBJECT

public:
    explicit MediaCopierParam(QWidget *parent = nullptr);
    ~MediaCopierParam();
    void init(std::shared_ptr<Config> config);

private:
    void syncConfig();

private Q_SLOTS:
    void onOpenInputDirClicked();
    void onOpenOutputDirClicked();
    void onInputDirChanged(const QString& text);
    void onOutputDirChanged(const QString& text);
    void onBaseFormatChanged(const QString& text);
    void onCommandChanged(int index);

private:
    Ui::MediaCopierParam *ui;
    std::shared_ptr<Config> config;
};
