#pragma once
#include <QObject>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QCoreApplication>

class QmlReloader : public QObject {
    Q_OBJECT;
public:
    explicit QmlReloader(QQmlEngine* engine, QObject* parent = nullptr)
        : QObject(parent)
        , m_engine(engine)
    {
    }

    void registerContextFile(const QString& name, const QUrl& url)
    {
        m_contextFiles.append({ name, url });
        reloadContextFile(name, url);
    }

public slots:
    void clearCache()
    {
        if (!m_engine)
            return;
        QCoreApplication::processEvents();
        m_engine->clearComponentCache();
        m_engine->trimComponentCache();
        m_engine->collectGarbage();
        for (auto& [name, url] : m_contextFiles)
            reloadContextFile(name, url);
        qDebug() << "C++: cleared QML cache";
    }

private:
    void reloadContextFile(const QString& name, const QUrl& url)
    {
        QQmlComponent comp(m_engine, url);
        QObject* obj = comp.create();
        if (obj)
            m_engine->rootContext()->setContextProperty(name, obj);
        else
            qWarning() << "QmlReloader: failed to reload" << name << comp.errorString();
    }

    QQmlEngine* m_engine;
    QList<std::pair<QString, QUrl>> m_contextFiles;
};
