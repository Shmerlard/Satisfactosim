#pragma once

#include <QObject>
#include <QColor>
#include <QJsonObject>

class Port;

class Connection : public QObject {
    Q_OBJECT;

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
};
