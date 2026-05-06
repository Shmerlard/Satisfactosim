#pragma once

#include "Port.h"
#include <QJsonObject>
// #include <QList>
#include <QObject>
#include <QUuid>

class Factory;

enum class NodeType : uint8_t {
    Abstract,
    FactoryEdge,
    Factory,
    Extraction,
    Production
};

class AbstractNode {
protected:
    explicit AbstractNode(
        Factory& parentFactory,
        QString name = QString(),
        QUuid id = QUuid());
    QUuid m_id;
    NodeType m_type = NodeType::Abstract;
    Factory* m_parentFactory;
    QString m_name;
    std::vector<std::unique_ptr<Port>> m_inputs;
    std::vector<std::unique_ptr<Port>> m_outputs;

public:
    virtual ~AbstractNode() = default; // FIXME: may need improvement

    QUuid id() const { return m_id; }
    NodeType type() const { return m_type; }
    Factory* parentFactory() const { return m_parentFactory; }
    QString name() const { return m_name; }

    int hierarchyLevel() const { return static_cast<int>(m_type);}
    virtual float portRate(const Port* port) const = 0;
    virtual QJsonObject getJsonNode() const;

    void deletePorts();
    void disconnectAllPorts();
    virtual void disconnectPort(Port* port, Port* peer, QString* err = nullptr);
    virtual bool connectToPort(Port& src, Port& dst, QString* err = nullptr);
    virtual void onPortConnected(Port& port) {};
    virtual void onPortDisconnected(Port& port) {};

    const std::vector<std::unique_ptr<Port>>& inputs() const { return m_inputs; }
    const std::vector<std::unique_ptr<Port>>& outputs() const { return m_outputs; }
    void setId(QUuid id) { m_id = id; }
    void setName(QString name) { m_name = name; }

    int index() const;
    int getPortIndex(Port& port) const;
    Port* getPortFromIndex(int index) const;
};
