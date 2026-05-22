#pragma once

#include "Port.h"
#include <QJsonObject>
#include <QObject>
#include <QPointF>
#include <QUuid>
#include <QVariantList>
#include <QVariantMap>
#include "core/types/Types.h"


class Connection;
class Factory;

enum class NodeType : uint8_t {
    Abstract,
    FactoryEdge,
    Factory,
    Extraction,
    Production,
    Splitter
};

class AbstractNode : public QObject {
    Q_OBJECT;
    Q_PROPERTY(double posX READ posX WRITE setPosX NOTIFY posChanged)
    Q_PROPERTY(double posY READ posY WRITE setPosY NOTIFY posChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int nodeType READ nodeTypeInt CONSTANT)
    Q_PROPERTY(QVariantList inputs READ inputsQml NOTIFY portsChanged)
    Q_PROPERTY(QVariantList outputs READ outputsQml NOTIFY portsChanged)

protected:
    explicit AbstractNode(
        Factory& parentFactory,
        QString name = QString(),
        QUuid id = QUuid());

    QUuid m_id;
    NodeType m_type = NodeType::Abstract;
    Factory* m_parentFactory = nullptr;
    QString m_name = QString("");
    QPointF m_pos = QPointF(0, 0);
    std::vector<std::unique_ptr<Port>> m_inputs;
    std::vector<std::unique_ptr<Port>> m_outputs;

public:
    virtual ~AbstractNode() = default; // FIXME: may need improvement

    // ---------- GETTERS ---------------
    QUuid id() const { return m_id; }
    NodeType type() const { return m_type; }
    int nodeTypeInt() const { return static_cast<int>(m_type); }
    Factory* parentFactory() const { return m_parentFactory; }
    QString name() const { return m_name; }
    QPointF pos() const { return m_pos; }
    double posX() const { return m_pos.x(); }
    double posY() const { return m_pos.y(); }

    const std::vector<std::unique_ptr<Port>>& inputs() const { return m_inputs; }
    const std::vector<std::unique_ptr<Port>>& outputs() const { return m_outputs; }

    // ---------- MISC ---------------
    QSet<AbstractNode*> getNeighbors() const;
    virtual QJsonObject getJsonNode() const;
    virtual QString getHeaderInfo() const;
    int index() const;
    virtual Frac portRate(const Port* port) const = 0; // FIX: up to delete

    // ---------- PORTS ---------------
    void deletePorts();
    void disconnectAllPorts();
    int getPortIndex(Port& port, bool isOffset = true) const;
    Port* getPortFromIndex(int index) const;
    bool isMachineNode() const { return m_type == NodeType::Extraction || m_type == NodeType::Production; }

    virtual void onPortConnected(Port& port) { };
    virtual void onPortDisconnected(Port& port) { };

    QVariantList inputsQml() const;
    virtual QVariantList outputsQml() const;

    // ---------- SETTERS ---------------
    void setId(QUuid id) { m_id = id; }
    void setName(QString name);
    void setPos(QPointF pos);
    void setPosX(double x);
    void setPosY(double y);
    virtual void notifySolved() { emit portsChanged(); }

signals:
    void posChanged();
    void nodeUpdated();
    void nameChanged();
    void portsChanged();
};
