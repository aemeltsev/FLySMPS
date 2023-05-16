#ifndef DEVICESMANAGER_H
#define DEVICESMANAGER_H

#include "singleton.h"
#include "dbmanager.h"
#include "defines.h"

namespace db {
class DevicesManager : public DBManager, Singleton<DevicesManager>
{
public:
    DevicesManager();
    ~DevicesManager();
    CoreModel* openCore(int coreId);
    bool saveCore(CoreModel* core);
    bool removeCore(int coreId);
    QMap<QString, int> listCores(bool onlyEnabled = true);

    bool existByKey(const QString& column, const QVariant& value);

protected:
    QString connectionName() const;
    int lastInsertId(const QString& table);

private:
    bool createTables();

    CoreModel* openCoreImpl(int coreId);
    bool saveCoreImpl(CoreModel* device);
    bool removeCoreImpl(int coreId);
};
} //namespace db
#endif // DEVICESMANAGER_H
