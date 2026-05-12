#pragma once

#include <QObject>
#include <QColor>
#include <QJsonObject>
#include <QJsonArray>

class Port;

class Connection : public QObject {
    Q_OBJECT;

    Q_PROPERTY(QObject* srcNode READ srcNodeObj CONSTANT)
    Q_PROPERTY(QObject* dstNode READ dstNodeObj CONSTANT)
    Q_PROPERTY(int srcPortIdx READ srcPortIdx CONSTANT)
    Q_PROPERTY(int dstPortIdx READ dstPortIdx CONSTANT)
private:
    Port* m_src = nullptr;
    Port* m_dst = nullptr;
    QVariantList m_mid;
    QColor m_color = QColor("black");
    float amount = 0;

public:
    explicit Connection(Port* src, Port* dst);
    void addMidPoint(int index);
    void removeMidPoint(int index);
    void disconnect(Port* port);
    Port* getPeer(Port& port);
    QJsonObject getJsonObject();

    QObject* srcNodeObj() const;
    QObject* dstNodeObj() const;
    int srcPortIdx() const;
    int dstPortIdx() const;
};
