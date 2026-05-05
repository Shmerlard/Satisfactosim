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
    void handleAddEdge(const QStringList& parts);
    void handleRm(const QStringList& args);
    void handleLs();
    void handleCd(const QStringList& args);
    void handleConnect(const QStringList& args);
    void handleDisconnect(const QStringList& args);
    void handleLimit(const QStringList& args);
    void handlePurity(const QStringList& args);
    void handleTier(const QStringList& args);
    // void handleSolve();
    void handleRename(const QStringList& args);
    void handleSave(const QStringList& args);
    // void handleHelp();

private slots:
    void onInputReady();

signals:
    void quitRequested();

public:
    explicit CliManager(QObject* parent = nullptr);
    ~CliManager();
};
