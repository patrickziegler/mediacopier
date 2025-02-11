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

#include "config.hpp"

#include <QWidget>

namespace Ui {
class MediaCopierParamWidget;
}

class MediaCopierParamWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MediaCopierParamWidget(QWidget *parent=nullptr);
    ~MediaCopierParamWidget();
    void init(std::shared_ptr<Config> config);
    void validateParameters();
    QString getCommandDescription(const Config::Command& command) const;

Q_SIGNALS:
    void validParameters();
    void invalidParameters();

private Q_SLOTS:
    void onOpenInputDirClicked();
    void onOpenOutputDirClicked();
    void onPatternUpdateClicked(bool checked);
    void onInputDirChanged(const QString& text);
    void onOutputDirChanged(const QString& text);
    void onPatternChanged(const QString& text);
    void onTimezoneChanged(int index);
    void onCommandChanged(int index);

private:
    Ui::MediaCopierParamWidget *ui;
    std::shared_ptr<Config> m_config;
};
