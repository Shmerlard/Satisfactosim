#pragma once

#include "core/nodes/ProductionNode.h"
// #include "core/managers/GameLibrary.h"
#include "core/types/Types.h"
#include <QMap>
#include <QPixmap>
#include <QString>

#include <QQuickImageProvider>

// a singleton that manage all the assest
class AssetManager {
public:
    static AssetManager& get()
    {
        static AssetManager* inst = new AssetManager();
        return *inst;
    }

    AssetManager(const AssetManager&) = delete;
    void operator=(const AssetManager&) = delete;

    void loadAssets();

    QPixmap getMachineIcon(const Machine* machine) const;
    QPixmap getItemIcon(const Item* item) const;

private:
    AssetManager() = default;

    QMap<const Machine*, QPixmap> m_machineIcons;
    QMap<const Item*, QPixmap> m_itemIcons;
};

class AssetImageProvider : public QQuickImageProvider {
public:
    AssetImageProvider();
    QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize) override;
};
