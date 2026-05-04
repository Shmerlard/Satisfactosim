#pragma once

#include "Port.h"
#include <QJsonObject>
// #include <QList>
#include <QObject>
#include <QUuid>

class Factory;

class AbstractNode {
protected:
    explicit AbstractNode(
        Factory& parentFactory,
        QString name = QString());
    Factory* m_parentFactory;
    QUuid m_id;
    QString m_name;
    // QList<Port*> m_inputs;
    // QList<Port*> m_outputs;
    std::vector<std::unique_ptr<Port>> m_inputs;
    std::vector<std::unique_ptr<Port>> m_outputs;

public:
    virtual ~AbstractNode() = default; // FIXME: may need improvement

    QUuid id() const { return m_id; }
    QString name() const { return m_name; }
    Factory* parentFactory() const { return m_parentFactory; }

    virtual int hierarchyLevel() const = 0;
    virtual float portRate(const Port* port) const = 0;
    virtual QJsonObject getJsonNode() const = 0;

    void deletePorts();
    // void disconnectPort(Port* port)
    void disconnectAllPorts();

    // const QList<std::unique_ptr<Port>>& inputs() const { return m_inputs; }
    // const QList<std::unique_ptr<Port>>& outputs() const { return m_outputs; }
    const std::vector<std::unique_ptr<Port>>& inputs() const { return m_inputs; }
    const std::vector<std::unique_ptr<Port>>& outputs() const { return m_outputs; }
    void setName(QString name) { m_name = name; }

    int index() const;
    int getPortIndex(Port& port) const;
    Port* getPortFromIndex(int index) const;
    bool connectToPort(Port& src, Port& dst, QString* err = nullptr);
};
