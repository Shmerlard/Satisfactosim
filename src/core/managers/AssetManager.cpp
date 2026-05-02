#include "AssetManager.h"
#include "GameLibrary.h"
#include <QDir>
#include <QFileInfo>

void AssetManager::loadAssets()
{
    int success = 0;
    int failures = 0;
    for (Item* i : GameLibrary::get().Items()) {
        QString iconPath = i->iconPath;
        QPixmap icon(iconPath);

        if (!icon.isNull()) {
            m_itemIcons[i] = icon;
            success++;
        } else {
            qWarning() << "WARNING: Could not find Icon: " << iconPath;
            failures++;
        }
        // m_itemIcons[i] = QPixmap(iconPath);
    }
        qDebug() << "Got: " << success << "Sucesses and: " 
            << failures << " failures";


    for (Machine* m : GameLibrary::get().Machines()) {
        QString iconPath = m->iconPath;
        QPixmap icon = QPixmap(iconPath);
        if (!icon.isNull()) {
            m_machineIcons[m] = icon;
        } else {
            qDebug() << "Failed to load machine icon:" << m->iconPath;
        }
    }
}

QPixmap AssetManager::getMachineIcon(const Machine* machine) const
{
    return m_machineIcons.value(machine, QPixmap());
}

QPixmap AssetManager::getItemIcon(const Item* item) const
{
    return m_itemIcons.value(item, QPixmap());
}

AssetImageProvider::AssetImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap AssetImageProvider::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize)
{
    // format: type/className (e.g., machine/Build_Constructor_C)
    QStringList parts = id.split('/');
    if (parts.size() < 2)
        return QPixmap();

    QString type = parts[0];
    QString className = parts[1];

    if (type == "machine") {
        const Machine* m = GameLibrary::get().getMachine(className);
        if (m) {
            QPixmap pix = AssetManager::get().getMachineIcon(m);
            if (size)
                *size = pix.size();
            return pix;
        }
    } else if (type == "item") {
        const Item* i = GameLibrary::get().getItem(className);
        if (i) {
            QPixmap pix = AssetManager::get().getItemIcon(i);
            if (size)
                *size = pix.size();
            return pix;
        }
    }

    return QPixmap();
}
