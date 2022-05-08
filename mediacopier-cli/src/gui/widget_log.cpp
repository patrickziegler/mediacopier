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

#include "gui/widget_log.hpp"

#include <mediacopier/version.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/qt_sinks.h>

// auto generated by moc
#include "ui_widget_log.h"

MediaCopierLogWidget::MediaCopierLogWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::MediaCopierLogWidget)
{
    ui->setupUi(this);

    auto sink = std::make_shared<spdlog::sinks::qt_sink_st>(ui->logText, "appendPlainText");
    spdlog::default_logger()->sinks().push_back(std::move(sink));
}

MediaCopierLogWidget::~MediaCopierLogWidget()
{
    delete ui;
}

void MediaCopierLogWidget::update(Status info)
{
    ui->logProgressBar->setMaximum(info.fileCount());
    ui->logProgressBar->setValue(info.progress());
}