#pragma once

#include "Port.h"
#include "core/types/Types.h"
#include <QList>
#include <QString>
#include <QObject>
#include <QUuid>

class AbstractNode;
class FactoryNode;
class FactoryEdgeNode;
class ProductionNode;
class ExtractionNode;
class Connection;
struct Recipe;

class Factory : public QObject {
    Q_OBJECT

    friend class SessionManager;
    friend class FactoryNode;

private:
    QUuid m_id;
    QString m_name;
    Factory* m_parent;
    FactoryNode* m_node = nullptr;

    QList<AbstractNode*> m_nodes;
    QList<Factory*> m_subFactories;
    QList<Connection*> m_connections;

    QList<FactoryEdgeNode*> m_edges;

private:
    Factory* createFactory(QString name = QString(), bool ignoreNode = false);
    FactoryEdgeNode* createFactoryEdgeNode(PortType edgeType, QString name = QString(), QString* err = nullptr);
    ExtractionNode* createExtractionNode(const ExtractionRecipe& recipe, int tier = 1, QString name = QString());
    ProductionNode* createProductionNode(const ProductionRecipe& recipe, QString name = QString());
    FactoryNode* createFactoryNode(Factory* targetFactory, QString name = QString());

public:
    explicit Factory(Factory* parentFactory, QString name, QUuid id = QUuid());
    void setId(QUuid id) { m_id = id; }
    void setName(QString name) { m_name = name; }

    QUuid id() const { return m_id; }
    QString name() const { return m_name; }
    Factory* parent() const { return m_parent; }
    FactoryNode* node() const { return m_node; } // nullptr for root

    const QList<AbstractNode*>& nodes() const { return m_nodes; }
    const QList<Factory*>& subFactories() const { return m_subFactories; }
    const QList<Connection*>& connections() const { return m_connections; }
    const QList<FactoryEdgeNode*>& edges() const { return m_edges; }

    Connection* connect(Port& a, Port& b, QString* err = nullptr);
    void disconnect(Port& a, Port& b, QString* err = nullptr);
    // Connection* disconnectPort(Port& port, Port& peer, QString* err = nullptr);
    // Connection* connectToPort(Port& src, Port& dst, QString* err = nullptr);

    void addNode(AbstractNode& node);
    void removeNode(AbstractNode& node);
};
