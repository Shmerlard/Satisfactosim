#include "core/managers/AssetManager.h"
#include "core/managers/CliManager.h"
#include "core/managers/GameLibrary.h"
#include "core/managers/SessionManager.h"
#include "gui/controllers/SceneManager.h"
#include <QApplication>
#include <QDir>
#include <QQmlApplicationEngine>
#include "gui/QmlReloader.h"
#include <QtQml>
#include <QCoreApplication>
#include <iostream>

int main(int argc, char* argv[])
{
    bool headless = false;
    for (int i = 1; i < argc; ++i) {
        if (QString(argv[i]) == "--headless" || QString(argv[i]) == "-h")
            headless = true;
    }

    QScopedPointer<QCoreApplication> app;
    if (headless)
        app.reset(new QCoreApplication(argc, argv));
    else
        app.reset(new QApplication(argc, argv));

    QDir::setCurrent(QString(PROJECT_SOURCE_DIR));

    std::cout << "Loading Assets...\n";
    GameLibrary::get().loadData();

    CliManager cli;
    QObject::connect(&cli, &CliManager::quitRequested, app.data(), &QCoreApplication::quit);

    QString projDir = QString(PROJECT_SOURCE_DIR);

    QScopedPointer<QQmlApplicationEngine> engine;
    QScopedPointer<QmlReloader> reloader;
    QScopedPointer<SceneManager> sceneManager;
    if (!headless) {
        AssetManager::get().loadAssets();
        engine.reset(new QQmlApplicationEngine());
        engine->addImageProvider("assets", new AssetImageProvider());
        reloader.reset(new QmlReloader(engine.data()));
        sceneManager.reset(new SceneManager(&SessionManager::get()));
        engine->rootContext()->setContextProperty("reloader", reloader.data());
        engine->rootContext()->setContextProperty("sceneManager", sceneManager.data());
        engine->addImportPath(projDir);
        engine->addImportPath(projDir + "/src");
        auto paths = engine->importPathList();
        paths.removeAll(projDir + "/build");
        engine->setImportPathList(paths);

        const QUrl url(QUrl::fromLocalFile(projDir + "/src/main.qml"));

        QObject::connect(engine.data(), &QQmlApplicationEngine::objectCreated, app.data(), [url](QObject* obj, const QUrl& objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1); }, Qt::QueuedConnection);

        engine->load(url);
    }

    return app->exec();
}
