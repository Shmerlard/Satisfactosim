#pragma once


#include <QObject>
#include <QSocketNotifier>
#include <QTextStream>
#include "SessionManager.h"

class CliManager : public QObject {
    Q_OBJECT;
private:
    QSocketNotifier* m_notifier;
    QTextStream m_out;
    SessionManager* m_session;

    void printPrompt();
    void processCommand(const QString& line);

    void handleAdd(const QStringList& args);
    void handleAddProd(const QStringList& parts);
    void handleAddExtractor(const QStringList& parts);
    void handleAddFact(const QStringList& parts);
    void handleRm(const QStringList& args);
    void handleLs();

private slots:
    void onInputReady();

signals:
    void quitRequested();

public:
    explicit CliManager(QObject* parent = nullptr);
    ~CliManager();

    void start();
    void run();
};
