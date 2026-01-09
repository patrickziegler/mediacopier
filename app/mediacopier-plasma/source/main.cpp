/* Copyright (C) 2025 Patrick Ziegler <zipat@proton.me>
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

#include "widgets/MediaCopierDialogSlim.hpp"
#include "worker.hpp"

#include <mediacopier/version.hpp>

#include <KJob>
#include <KUiServerV2JobTracker>

#include <QApplication>
#include <QTranslator>

#include <spdlog/spdlog.h>

class KMediaCopierJob : public KJob {

public:
    KMediaCopierJob(Worker* worker, QString command, const std::filesystem::path& dstDir)
        : m_command { std::move(command) }
        , m_worker { worker }
    {
        QApplication::setDesktopFileName("org.kde.dolphin");
        setCapabilities(Killable | Suspendable);
        setProgressUnit(Files);
        setProperty("destUrl", "file://" + QString::fromStdString(std::filesystem::absolute(dstDir)));
        setProperty("immediateProgressReporting", false);
        QObject::connect(m_worker, &Worker::updateDescription, this, &KMediaCopierJob::updateDescription);
        QObject::connect(m_worker, &Worker::updateProgress, this, &KMediaCopierJob::updateProgress);
        QObject::connect(m_worker, &Worker::finished, this, &KMediaCopierJob::quit);
    }
    void start() override
    {
        // nothing to do here
    }
public Q_SLOTS:
    void updateDescription(StatusDescription info)
    {
        description(this, m_command,
            qMakePair<QString, QString>(
                QCoreApplication::translate("Strings", stringSource()), QString::fromStdString(info.inputPath)),
            qMakePair<QString, QString>(
                QCoreApplication::translate("Strings", stringDestination()), QString::fromStdString(info.outputPath)));
    }
    void updateProgress(StatusProgress info)
    {
        setTotalAmount(Files, info.count);
        setProcessedAmount(Files, info.progress);
    }
    void quit()
    {
        emitResult();
    }

protected:
    bool doKill() override
    {
        m_worker->kill();
        return true;
    }
    bool doSuspend() override
    {
        m_worker->suspend();
        return true;
    }
    bool doResume() override
    {
        m_worker->resume();
        return true;
    }

private:
    QString m_command;
    Worker* m_worker;
};

class PlasmaWorker : public Worker {
public:
    PlasmaWorker(std::shared_ptr<Config> config, const QString& description)
        : Worker(config)
    {
        auto job = new KMediaCopierJob(this, description, getConfig()->getOutputDir());
        m_tracker.registerJob(job);
    }

private:
    KUiServerV2JobTracker m_tracker;
};

class PlasmaWorkerFactory : public WorkerFactory {
public:
    explicit PlasmaWorkerFactory(std::shared_ptr<Config> config)
        : WorkerFactory(config)
    {
    }
    std::unique_ptr<Worker> make_worker(const QString& description) override
    {
        return std::make_unique<PlasmaWorker>(getConfig(), description);
    }
};

int main(int argc, char* argv[])
{
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#endif
    try {
        QApplication app(argc, argv);

        QTranslator translator;
        if (translator.load(":/translations/lang_de.qm"))
            app.installTranslator(&translator);

        app.setApplicationName(mediacopier::MEDIACOPIER_PROJECT_NAME);
        app.setApplicationVersion(mediacopier::MEDIACOPIER_VERSION);

        auto config = std::make_shared<Config>(app);
        auto factory = std::make_unique<PlasmaWorkerFactory>(config);

        MediaCopierDialogSlim dialog;
        dialog.init(config, std::move(factory));
        dialog.show();
        return app.exec();

    } catch (const std::exception& err) {
        spdlog::error(err.what());
        return 1;
    }
}
