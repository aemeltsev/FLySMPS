#ifndef COREMANAGER_H
#define COREMANAGER_H

#include "singleton.h"
#include "dbmanager.h"
#include "../coretabmodel.h"

namespace db {
class CoreModel;


class CoreManager : public DBManager, public Singleton<CoreManager>
{
public:
    CoreManager();
    ~CoreManager();
    QString dbName() const; //Redefined pure virtual function
    CoreModel* openCore(int coreId);
    bool saveCore(CoreModel* core);
    bool removeCoreById(int coreId);
    bool removeCoreByModel(const QString& model);
    bool removeMaterial(const QString& name);
    QMap<QString, int> listCores();
    QList<QString> listMaterial();
    bool existByKey(const QString& column, const QVariant &value);
    QList<CoreTableItem> getListDataForTable();

protected:
    QString connectionName() const; //Redefined pure virtual function
    int lastInsertId(const QString& table);

private:
    void testData();
    CoreTableItem createCoreTableItem(const QSqlQuery& query);

    bool createTables();

    CoreModel* openCoreHelper(int coreId);
    bool saveCoreHelper(CoreModel* core);
    bool removeCoreByIdHelper(int coreId);
    bool removeCoreByModelHelper(const QString& model);

    static QString TABLE_NAME_MATERIAL;
    static QString TABLE_NAME_GEOMETRY;
    static QString TABLE_NAME_GAPPING;
    static QString TABLE_NAME_CORES;
    static QString CONNECTION_NAME_CORES;
};
}
#endif // COREMANAGER_H
