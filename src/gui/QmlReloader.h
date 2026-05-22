#pragma once
#include <QObject>
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

public slots:
    void clearCache()
    {
        if (!m_engine)
            return;
        QCoreApplication::processEvents();
        m_engine->clearComponentCache();
        m_engine->trimComponentCache();
        m_engine->collectGarbage();
        qDebug() << "C++: cleared QML cache";
    }

private:
    QQmlEngine* m_engine;
};
