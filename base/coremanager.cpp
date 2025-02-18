#include "coremanager.h"
#include "coremodel.h"
#include <inc/loggercategories.h>
#include <QScopedPointer>

QString db::CoreManager::TABLE_NAME_MATERIAL = QString("material");
QString db::CoreManager::TABLE_NAME_GEOMETRY = QString("geometry");
QString db::CoreManager::TABLE_NAME_GAPPING = QString("gapping");
QString db::CoreManager::TABLE_NAME_CORES = QString("core");
QString db::CoreManager::CONNECTION_NAME_CORES = QString("corelib");

db::CoreManager::CoreManager()
    :DBManager(nullptr)
{
    const QString tableDoNotExist = "Tables its't exist. Create tables.";

    bool materialTable = tableExists(TABLE_NAME_MATERIAL);
    bool geometryTable = tableExists(TABLE_NAME_GEOMETRY);
    bool gappingTable = tableExists(TABLE_NAME_GAPPING);
    bool coresTable = tableExists(TABLE_NAME_CORES);

    if(!materialTable && !geometryTable && !gappingTable && !coresTable){
        qInfo(logInfo()) << tableDoNotExist;
        createTables();
    }
}

db::CoreManager::~CoreManager()
{}

QString db::CoreManager::dbName() const
{
    return DBASE_COMMON_NAME;
}

/*!
 * \brief db::CoreManager::openCore - The method calls the
 *  openCoreHelper() helper method to obtain the CoreModel
 *  object by its identifier.
 *
 * CoreModel* core = coreManager.openCore(coreId);
 * if (core) {
 *     // Working with the core object
 * } else {
 *     // Handling object retrieval error
 * }
 *
 * \param coreId - Identifier core on the database
 * \return CoreModel object
 */
db::CoreModel *db::CoreManager::openCore(int coreId)
{
    return openCoreHelper(coreId);
}

/*!
 * \brief db::CoreManager::saveCore - The function does the actual
 *  work of saving the core object to the database.
 * \param core - This value is a pointer to a CoreModel object.
 * \return Returns a bool indicating the success or failure of the operation.
 */
bool db::CoreManager::saveCore(db::CoreModel *core)
{
    return saveCoreHelper(core);
}

/*!
 * \brief db::CoreManager::removeCore -The function performs the work
 *  of deleting an object with the identifier coreId from the database.
 * \param coreId - This parameter represents the ID of the object to be deleted.
 * \return Returns a bool indicating the success or failure of the operation.
 */
bool db::CoreManager::removeCore(int coreId)
{
    return removeCoreHelper(coreId);
}

/*!
 * \brief db::CoreManager::listCores - The method is designed to get
 *  a list of all CoreModels from the database data and
 *  return them as a QMap, where the key is the model
 *  and the value is the identifier. The method creates a SQL query
 *  to retrieve all records from the "TABLE_NAME_CORES" table,
 *  selecting the id and model columns.
 *  --? Instead of using QSqlRecord to get column indices,
 *  you can directly access the values ​​by column name,
 *  which can be more efficient.
 *
 * QMap<QString, int> cores = coreManager.listCores();
 * for (auto it = cores.begin(); it != cores.end(); ++it) {
 *     qDebug() << "Model:" << it.key() << ", ID:" << it.value();
 * }
 * \return
 */
QMap<QString, int> db::CoreManager::listCores()
{
    QMap<QString, int> result;
    QString sqlQuery;
    sqlQuery = sql("SELECT id, model FROM %1").arg(TABLE_NAME_CORES);

    QSqlQuery q(sqlQuery, db());
    if(!q.exec()){
        setLastError((q.lastError().text()));
        return QMap<QString, int>();
    }

    QSqlRecord rec;
    while(q.next()){
        rec = QSqlRecord(q.record());
        result.insert(q.value(rec.indexOf("model")).toString(), q.value(rec.indexOf("id")).toInt());
    }
    return result;
}

/*!
 * \brief db::CoreManager::existByKey - A method designed to check
 *  for the presence of records in the TABLE_NAME_CORES table by
 *  a given column and result. Use SELECT COUNT(*) to get the number
 *  of records at once.
 *
 * if (coreManager.existByKey("model", "PC47EE8-Z")) {
 *     // Record with model "PC47EE8-Z" exists
 * } else {
 *     // Record not found
 * }
 *
 * \param column - An attribute is a set of data values ​​of a certain type.
 * \param value - Specific type value, for example, model value - "PC47EE8-Z"
 * \return If record exist return true otherwise false
 *  if record not exist or had something error.
 */
bool db::CoreManager::existByKey(const QString &column, const QVariant &value)
{
    QString sqlQuery = sql("SELECT COUNT(*) FROM %1 WHERE %2=:value").arg(TABLE_NAME_CORES).arg(column);
    QSqlQuery q(sqlQuery, db());
    q.bindValue(":value", value);

    if(!q.exec()){
        setLastError((q.lastError().text()));
        return false;
    }

    if (q.next()) {
        int count = q.value(0).toInt();
        return count > 0;
    }

    return false;
}

/*!
 * \brief db::CoreManager::connectionName - Overridden virtual function
 * \return Database connection name, type QString
 */
QString db::CoreManager::connectionName() const
{
    return CONNECTION_NAME_CORES;
}

/*!
 * \brief db::CoreManager::lastInsertId - The method is designed to
 *  get the last inserted identifier in the specified table.
 *  The method checks if the table name matches TABLE_NAME_CORES.
 *  If so, it generates a SQL query to get the maximum id value
 *  from that table.
 * int lastId = coreManager.lastInsertId(TABLE_NAME_CORES);
 * if (lastId != -1) {
 *     // Use lastId
 * } else {
 *     // Handling id retrieval error
 * }
 *
 * \param table - Table name
 * \return If the request fails or the id value cannot be
 *  converted to an integer, the method returns -1
 */
int db::CoreManager::lastInsertId(const QString &table)
{
    QString sqlQuery;
    if(table.compare(TABLE_NAME_CORES) == 0){
        sqlQuery = sql("SELECT MAX(id) FROM %1").arg(table);

        QSqlQuery q(sqlQuery, db());
        if(q.exec() && q.next()){
            bool ok;
            int id = q.value(0).toInt(&ok);
            if(ok){
                return id;
            }
        }
    }
    return -1;
}

/*!
 * \brief db::CoreManager::testData - The method is designed to
 *  create and save a test object of the core model, specifically
 *  for the "EE" type core without a gap. A new CoreModel object
 *  is created with the type CoreType::EE and the boolean value false
 *  (indicating that there is no gap). Various properties of
 *  the core model are set.
 *  A Material object, a Gapping object, a Geometry object
 *  with certain properties are created and assigned to the core model.
 *  The saveCore method is called to save the core model to the database.
 */
void db::CoreManager::testData()
{
    qInfo(logInfo()) << QString::fromLatin1("Writing test data...");
    /* Create test model object type EE core without gap.
     * Use QScopedPointer to manage the memory of the CoreModel object.
     */
    QScopedPointer<CoreModel> model(new CoreModel(CoreType::EE, false));
    model->name("EE8");
    model->model("PC47EE8-Z");

    Material mat("PC47", 2500, 13, 230, 600, 500, 530., 4.);
    Gapping gap("EE8", 1350, 550., 0, 0.035);
    model->coreMaterial(mat);
    model->coreGapping(gap);

    model->resistanceFactor(0.035);
    model->effectiveMagneticVolume(78);
    model->windowCrossSection(2.7);
    model->effectiveMagneticPathLength(15.5);
    model->effectiveMagneticCrossSection(5.0);
    model->lengthTurn(14.9);

    Geometry geom("PC47EE8-Z", CoreType::EE, 0., 0., 0., 3.6, 4.0, 3.0, 8.3, 6.0, 1.85, 0.);
    model->geometry(geom);
    /* Pass a raw pointer. Deleting the object is not required
     * as QScopedPointer will automatically free the memory.
     */
    saveCore(model.data());
    qInfo(logInfo()) << QString::fromLatin1("Writing complete.");
}

/*!
 * \brief db::CoreManager::createTables
 * \return
 */
bool db::CoreManager::createTables()
{
    //TABLE_NAME_MATERIAL
    setLastError(QString());
    QString sqlQuery("");

    sqlQuery = sql("CREATE TABLE IF NOT EXIST %1("
                   "name TEXT NOT NULL UNIQUE,"
                   "high_relative_permeability INTEGER,"
                   "coercive_field INTEGER,"
                   "temp_curie INTEGER,"
                   "core_losses_relative INTEGER,"
                   "upper_operating_frequency INTEGER,"
                   "flux_density REAL,"
                   "electrical_resistivity REAL,"
                   "PRIMARY KEY(name)"
                   ")").arg(TABLE_NAME_MATERIAL);
    QSqlQuery q1(sqlQuery, db());
    if(!q1.exec()){
        setLastError(q1.lastError().text());
        qInfo(logCritical()) << QString::fromLatin1("Sql error:") << q1.lastError().text();
        return false;
    }
    qInfo(logInfo()) << "Table " << TABLE_NAME_MATERIAL << " is created.";

    //TABLE_NAME_GEOMETRY
    setLastError(QString());

    sqlQuery = sql("CREATE TABLE %1("
                   "model TEXT NOT NULL UNIQUE,"
                   "type TEXT,"
                   "h REAL,"
                   "inner_diam REAL,"
                   "outer_diam	REAL,"
                   "c REAL,"
                   "b REAL,"
                   "f REAL,"
                   "a REAL,"
                   "e REAL,"
                   "d REAL,"
                   "g REAL,"
                   "PRIMARY KEY(model)"
                   ")").arg(TABLE_NAME_GEOMETRY);
    QSqlQuery q2(sqlQuery, db());
    if(!q2.exec()){
        setLastError(q2.lastError().text());
        qInfo(logCritical()) << QString::fromLatin1("Sql error:") << q2.lastError().text();
        return false;
    }
    qInfo(logInfo()) << "Table " << TABLE_NAME_GEOMETRY << " is created.";

    //TABLE_NAME_GAPPING
    setLastError(QString());

    sqlQuery = sql("CREATE TABLE %1("
                   "model TEXT NOT NULL UNIQUE,"
                   "actual_relative_permeability INTEGER,"
                   "inductance_factor REAL,"
                   "gap_length REAL,"
                   "actual_core_losses REAL,"
                   "PRIMARY KEY(model)"
                   ")").arg(TABLE_NAME_GAPPING);
    QSqlQuery q3(sqlQuery, db());
    if(!q3.exec()){
        setLastError(q3.lastError().text());
        qInfo(logCritical()) << QString::fromLatin1("Sql error:") << q3.lastError().text();
        return false;
    }
    qInfo(logInfo()) << "Table" << TABLE_NAME_GAPPING << " is created.";

    //TABLE_NAME_CORES
    setLastError(QString());

    sqlQuery = sql("CREATE TABLE %1("
                   "id INTEGER NOT NULL UNIQUE,"
                   "name TEXT,"
                   "model TEXT,"
                   "gapped INTEGER,"
                   "type TEXT,"
                   "material TEXT,"
                   "gapping TEXT,"
                   "resistance_factor REAL,"
                   "effective_magnetic_volume INTEGER,"
                   "window_cross_section REAL,"
                   "effective_magnetic_path_length REAL,"
                   "effective_magnetic_cross_section REAL,"
                   "lengh_turn REAL,"
                   "geometry TEXT,"
                   "PRIMARY KEY(id AUTOINCREMENT),"
                   "FOREIGN KEY(gapping) REFERENCES gapping(model),"
                   "FOREIGN KEY(geometry) REFERENCES geometry(model),"
                   "FOREIGN KEY(material) REFERENCES material(name)"
                   ")").arg(TABLE_NAME_CORES);
    QSqlQuery q4(sqlQuery, db());
    if(!q4.exec()){
        setLastError(q4.lastError().text());
        qInfo(logCritical()) << QString::fromLatin1("Sql error:") << q4.lastError().text();
        return false;
    }
    qInfo(logInfo()) << "Table" << TABLE_NAME_CORES << " is created.";

    return true;
}

db::CoreModel *db::CoreManager::openCoreHelper(int coreId)
{
    qInfo(logInfo()) << "Open core, with " << coreId << " core id.";

    QString CoresSqlQuery = sql("SELECT * FROM %1 WHERE id:=id").arg(TABLE_NAME_CORES);
    QSqlQuery CoresQuery(CoresSqlQuery, db());
    CoresQuery.bindValue(":id", coreId);

    if(!CoresQuery.exec() || !CoresQuery.next()){
        setLastError(CoresQuery.lastError().text());
        qInfo(logCritical()) << QString::fromLatin1("Sql error:") << CoresQuery.lastError().text();
        return nullptr;
    }

    CoreModel * core = new CoreModel();
    QSqlRecord rec(CoresQuery.record());

    core->id(CoresQuery.value(rec.indexOf("id")).toInt());
    core->name(CoresQuery.value(rec.indexOf("name")).toString());
    core->model(CoresQuery.value(rec.indexOf("model")).toString());
    core->gapped(CoresQuery.value(rec.indexOf("gapped")).toBool());
    core->type(getCoreType(CoresQuery.value(rec.indexOf("type")).toString()));
    core->resistanceFactor(CoresQuery.value(rec.indexOf("resistance_factor")).toDouble());
    core->effectiveMagneticVolume(CoresQuery.value(rec.indexOf("effective_magnetic_volume")).toInt());
    core->windowCrossSection(CoresQuery.value(rec.indexOf("window_cross_section")).toDouble());
    core->effectiveMagneticPathLength(CoresQuery.value(rec.indexOf("effective_magnetic_path_length")).toDouble());
    core->effectiveMagneticCrossSection(CoresQuery.value(rec.indexOf("effective_magnetic_cross_section")).toDouble());
    core->lengthTurn(CoresQuery.value(rec.indexOf("lengh_turn")).toDouble());

    QString coreModel = core->model();
    QString materialName = CoresQuery.value(rec.indexOf("material")).toString();

    //TABLE_NAME_MATERIAL
    QString MaterialSqlQuery = sql("SELECT * FROM %1 WHERE name:=name").arg(TABLE_NAME_MATERIAL);
    QSqlQuery q2(MaterialSqlQuery, db());
    q2.bindValue(":name", materialName);
    if(!q2.exec() || !q2.next()){
        setLastError(q2.lastError().text());
        qInfo(logCritical()) << QString::fromLatin1("Sql error:") << q2.lastError().text();
        delete core;
        return nullptr;
    }
    QSqlRecord rec_mat(q2.record());
    Material mat(q2.value(rec_mat.indexOf("name")).toString(),
                 q2.value(rec_mat.indexOf("high_relative_permeability")).toInt(),
                 q2.value(rec_mat.indexOf("coercive_field")).toInt(),
                 q2.value(rec_mat.indexOf("temp_curie")).toInt(),
                 q2.value(rec_mat.indexOf("core_losses_relative")).toInt(),
                 q2.value(rec_mat.indexOf("upper_operating_frequency")).toInt(),
                 q2.value(rec_mat.indexOf("flux_density")).toDouble(),
                 q2.value(rec_mat.indexOf("electrical_resistivity")).toDouble());
    core->coreMaterial(mat);

    //TABLE_NAME_GAPPING
    sqlQuery = sql("SELECT * FROM %1 WHERE model:=model").arg(TABLE_NAME_GAPPING);
    QSqlQuery q3(sqlQuery, db());
    q2.bindValue(":model", coreModel);
    if(!q3.exec() || !q3.next()){
        setLastError(q3.lastError().text());
        qInfo(logCritical()) << QString::fromLatin1("Sql error:") << q3.lastError().text();
        delete core;
        return nullptr;
    }
    QSqlRecord rec_gap(q3.record());
    Gapping gap(q3.value(rec_gap.indexOf("model")).toString(),
                q3.value(rec_gap.indexOf("actual_relative_permeability")).toInt(),
                q3.value(rec_gap.indexOf("inductance_factor")).toDouble(),
                q3.value(rec_gap.indexOf("gap_length")).toDouble(),
                q3.value(rec_gap.indexOf("actual_core_losses")).toDouble());
    core->coreGapping(gap);

    //TABLE_NAME_GEOMETRY
    sqlQuery = sql("SELECT * FROM %1 WHERE model:=model").arg(TABLE_NAME_GEOMETRY);
    QSqlQuery q4(sqlQuery, db());
    q4.bindValue(":model", coreModel);
    if(!q4.exec() || !q4.next()){
        setLastError(q4.lastError().text());
        qInfo(logCritical()) << QString::fromLatin1("Sql error:") << q4.lastError().text();
        delete core;
        return nullptr;
    }
    QSqlRecord rec_geom(q4.record());
    Geometry geom(coreModel, core->type(),
                  q4.value(rec_gap.indexOf("h")).toDouble(),
                  q4.value(rec_gap.indexOf("inner_diam")).toDouble(),
                  q4.value(rec_gap.indexOf("outer_diam")).toDouble(),
                  q4.value(rec_gap.indexOf("c")).toDouble(),
                  q4.value(rec_gap.indexOf("b")).toDouble(),
                  q4.value(rec_gap.indexOf("f")).toDouble(),
                  q4.value(rec_gap.indexOf("a")).toDouble(),
                  q4.value(rec_gap.indexOf("e")).toDouble(),
                  q4.value(rec_gap.indexOf("d")).toDouble(),
                  q4.value(rec_gap.indexOf("g")).toDouble());
    core->geometry(geom);

    return core;
}

bool db::CoreManager::saveCoreHelper(db::CoreModel *core)
{
    int coreId = core->id();

    if(!beginTransaction()){
        qInfo(logCritical()) << QString::fromLatin1("Can not begin transaction");
        return false;
    }
    // insert TABLE_NAME_MATERIAL
    QString MaterialSqlQuery = sql("INSERT OR REPLACE INTO %1(name, high_relative_permeability, coercive_field, temp_curie, core_losses_relative, upper_operating_frequency, flux_density, electrical_resistivity) "
                   "VALUES(:name, :high_relative_permeability, :coercive_field, :temp_curie, :core_losses_relative, :upper_operating_frequency, :flux_density, :electrical_resistivity)")
            .arg(TABLE_NAME_MATERIAL);

    QSqlQuery MaterialQuery(MaterialSqlQuery, db());

    // insert TABLE_NAME_GEOMETRY
    QString GeometrySqlQuery = sql("INSERT OR REPLACE INTO %1()");

    // insert TABLE_NAME_GAPPING
    QString GappingSqlQuery = sql("INSERT OR REPLACE INTO %1()");

    // insert TABLE_NAME_CORES
    QString CoresSqlQuery = sql("INSERT OR REPLACE INTO %1()");
}

bool db::CoreManager::removeCoreHelper(int coreId)
{
    //TODO
}


