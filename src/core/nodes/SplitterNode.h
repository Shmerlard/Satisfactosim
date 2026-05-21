#pragma once

#include "AbstractNode.h"

class SplitterNode : public AbstractNode {
    Q_OBJECT;

    friend class Factory;
    friend class SessionManager;

private:
    explicit SplitterNode(
            Factory& parentFactory,
            QList<Frac> weights = QList({Frac(1), Frac(1)}),
            QString name = QString(),
            QUuid id = QUuid()
            );
    QMap<Port*, Frac> m_weightMap;
    Frac m_total = Frac(0);
    // Item* m_item = nullptr;
public:
    void addOutput(Frac weight = Frac(1));
    Frac proportion(Port& port);
    void setWeight(Port& port, Frac weight);
    Frac portRate(const Port* port) const override;
    QJsonObject getJsonNode() const override;

    void onPortConnected(Port& port) override;
    void onPortDisconnected(Port& port) override;

};
