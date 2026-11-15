/* Copyright (C) 2026 Patrick Ziegler <zipat@proton.me>
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

#include <mediacopier/file_status.hpp>

#include <KOverlayIconPlugin>

#include <QUrl>

namespace {

auto overlay_icons(mediacopier::FileStatus status) -> QStringList
{
    switch (status) {
    case mediacopier::FileStatus::Copied:
        return { QStringLiteral("emblem-success") };
    case mediacopier::FileStatus::Duplicate:
        return { QStringLiteral("emblem-information") };
    case mediacopier::FileStatus::Error:
        return { QStringLiteral("emblem-error") };
    case mediacopier::FileStatus::Unsupported:
        return { QStringLiteral("emblem-warning") };
    case mediacopier::FileStatus::Unknown:
    default:
        return {};
    }
}

} // namespace

class MediaCopierOverlayPlugin : public KOverlayIconPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.overlayicon.mediacopier")

public:
    using KOverlayIconPlugin::KOverlayIconPlugin;

    auto getOverlays(const QUrl& item) -> QStringList override
    {
        if (!item.isLocalFile()) {
            return {};
        }
        return overlay_icons(mediacopier::get_file_status(item.toLocalFile().toStdString()));
    }
};

#include "MediaCopierOverlayPlugin.moc"
