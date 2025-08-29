#include "coremanager.h"
#include <inc/loggercategories.h>
#include "coremodel.h"
#include <QScopedPointer>

QString db::CoreManager::TABLE_NAME_MATERIAL = QString("material");
QString db::CoreManager::TABLE_NAME_GEOMETRY = QString("geometry");
QString db::CoreManager::TABLE_NAME_GAPPING = QString("gapping");
QString db::CoreManager::TABLE_NAME_CORES = QString("core");
QString db::CoreManager::CONNECTION_NAME_CORES = QString("CORELIB");

db::CoreManager::CoreManager()
    :DBManager(nullptr)
{
    allocateDb();    
    const QString tableDoNotExist = "Tables its't exist. Create tables.";

    bool materialTable = tableExists(TABLE_NAME_MATERIAL);
    bool geometryTable = tableExists(TABLE_NAME_GEOMETRY);
    bool gappingTable = tableExists(TABLE_NAME_GAPPING);
    bool coresTable = tableExists(TABLE_NAME_CORES);

    if(!materialTable && !geometryTable && !gappingTable && !coresTable){
        qInfo(logInfo()) << tableDoNotExist;
        createTables();
    }
    qInfo(logInfo()) << CONNECTION_NAME_CORES << "DB manager started was successful - OK";
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
bool db::CoreManager::removeCoreById(int coreId)
{
    return removeCoreByIdHelper(coreId);
}

bool db::CoreManager::removeCoreByModel(const QString &model)
{
    return removeCoreByModelHelper(model);
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

QList<QString> db::CoreManager::listMaterial()
{
    QList<QString> result;
    QString sqlQuery;
    sqlQuery = sql("SELECT name FROM %1").arg(TABLE_NAME_MATERIAL);

    QSqlQuery q(sqlQuery, db());
    if(!q.exec()){
        setLastError((q.lastError().text()));
        return QList<QString>();
    }

    QSqlRecord rec;
    while(q.next()){
        rec = QSqlRecord(q.record());
        result.append(q.value(rec.indexOf("name")).toString());
    }

    /*
     *     if (result.isEmpty()) {
        qWarning() << "No materials found in the database.";
    }*/
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
* \brief Creates a CoreTableItem object based on the data from the current SQL query row.
*
* The method extracts the field values from the passed SQL query and executes them on the CoreTableItem object.
* It is assumed that the query contains the fields: id, gap, model, geometry, material.
*
* \param query SQL query positioned in the current row.
* \return CoreTableItem Object, regardless of the data from the current query row.
*
* \note If the field in the database is NULL, the method will return the default value:
* 0 for int, false for bool, empty text for QString.
* For explicit handling of NULL values, it is recommended to add a query.isNull("field") check.
*/
CoreTableItem db::CoreManager::createCoreTableItem(const QSqlQuery &query)
{
    CoreTableItem item;
    item.m_id = query.value("id").toInt();
    item.m_gapped = query.value("gapped").toBool();
    item.m_model = query.value("model").toString();
    item.m_geom = query.value("geometry_type").toString();
    item.m_mat = query.value("material").toString();
    return item;
}

/*!
* \brief Returns a list of CoreTableItem objects to display in the table.
*
* The method executes a SQL query on the TABLE_NAME_CORES table and returns a list of objects
* filled with data from the database. This is used to populate tables in the user interface.
*
* \return QList<CoreTableItem> A list of objects containing data from the TABLE_NAME_CORES table.
* Returns an empty list if the query fails.
*
* \note The query selects all records from the TABLE_NAME_CORES table.
* TODO: Implement the onlyGapped parameter to filter records by the gapped field.
* Warning: The current implementation passes the query twice to q.exec(sqlQuery).
* You should use q.exec() without arguments.
*
* \warning If the query fails, an error message is logged,
* and an empty list is returned. This may hide the problem.
* Consider throwing an exception or using std::optional.
*/
QList<CoreTableItem> db::CoreManager::getListDataForTable()
{
    QList<CoreTableItem> result;
    result.reserve(100);

    //QString sqlQuery = sql("SELECT id, model, geometry, material, gapped FROM %1").arg(TABLE_NAME_CORES);
    QString sqlQuery = sql("SELECT c.id, c.model, c.geometry, c.material, c.gapped, g.type AS geometry_type FROM %1 c JOIN %2 g ON c.geometry = g.model;")
            .arg(TABLE_NAME_CORES)
            .arg(TABLE_NAME_GEOMETRY);

    // TODO: Add onlyGapped parameter to the method and check it if need the special sampling - sqlQuery += " WHERE gapped = 1";

    QSqlQuery q(sqlQuery, db());
    if(!q.exec(sqlQuery)){
        QString errorMsg = QString("Sql error where make core list for table:").arg(q.lastError().text());

        setLastError(errorMsg);
        qInfo(logCritical()) << errorMsg;
        return {};
    }

    while(q.next()) {
        result.append(createCoreTableItem(q));
    }

    return result;
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
                   "effective_magnetic_volume INTEGER,"       // Vc/Ve
                   "window_cross_section REAL,"               // Wa/An
                   "effective_magnetic_path_length REAL,"     // l_c/l_e
                   "effective_magnetic_cross_section REAL,"   // Ac/Ae
                   "lengh_turn REAL,"                         // l_t/l_n
                   "geometry TEXT,"
                   "PRIMARY KEY(id AUTOINCREMENT),"
                   "FOREIGN KEY(gapping) REFERENCES gapping(model) ON DELETE CASCADE,"
                   "FOREIGN KEY(geometry) REFERENCES geometry(model) ON DELETE CASCADE,"
                   "FOREIGN KEY(material) REFERENCES material(name)"
                   ")").arg(TABLE_NAME_CORES);
    QSqlQuery q4(sqlQuery, db());
    if(!q4.exec()){
        setLastError(q4.lastError().text());
        qInfo(logCritical()) << "createTables() method";
        qInfo(logCritical()) << "Sql error:" << q4.lastError().text();
        return false;
    }
    qInfo(logInfo()) << "Table" << TABLE_NAME_CORES << " is created.";

    return true;
}

db::CoreModel *db::CoreManager::openCoreHelper(int coreId)
{
    qInfo(logInfo()) << "Open core, with" << coreId << "core id.";

    QString CoresSqlQuery = sql("SELECT * FROM %1 WHERE id=%2").arg(TABLE_NAME_CORES).arg(coreId);
    QSqlQuery CoresQuery(CoresSqlQuery, db());
    //CoresQuery.bindValue(":id", coreId);

    if(!CoresQuery.exec() || !CoresQuery.next()){
        setLastError(CoresQuery.lastError().text());
        qInfo(logCritical()) << "openCoreHelper() method, query for core";
        qInfo(logCritical()) << "Sql error:" << CoresQuery.lastError().text();
        qInfo(logCritical()) << "Final SQL Query:" << CoresSqlQuery;
        qInfo(logCritical()) << "Bound ID:" << coreId;
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
    //qInfo(logInfo()) << "model:" << coreModel;
    //qInfo(logInfo()) << "material:" << materialName;

    //TABLE_NAME_MATERIAL
    QString MaterialSqlQuery = sql("SELECT * FROM %1 WHERE name='%2'").arg(TABLE_NAME_MATERIAL).arg(materialName);
    QSqlQuery MaterialQuery(MaterialSqlQuery, db());
    //MaterialQuery.bindValue(":name", materialName);
    if(!MaterialQuery.exec() || !MaterialQuery.next()){
        setLastError(MaterialQuery.lastError().text());
        qInfo(logCritical()) << "openCoreHelper() method, query for material";
        qInfo(logCritical()) << "Sql error:" << MaterialQuery.lastError().text();
        qInfo(logCritical()) << "Final SQL Query:" << MaterialSqlQuery;
        delete core;
        return nullptr;
    }
    QSqlRecord rec_mat(MaterialQuery.record());
    Material mat(MaterialQuery.value(rec_mat.indexOf("name")).toString(),
                 MaterialQuery.value(rec_mat.indexOf("high_relative_permeability")).toInt(),
                 MaterialQuery.value(rec_mat.indexOf("coercive_field")).toInt(),
                 MaterialQuery.value(rec_mat.indexOf("temp_curie")).toInt(),
                 MaterialQuery.value(rec_mat.indexOf("core_losses_relative")).toInt(),
                 MaterialQuery.value(rec_mat.indexOf("upper_operating_frequency")).toInt(),
                 MaterialQuery.value(rec_mat.indexOf("flux_density")).toDouble(),
                 MaterialQuery.value(rec_mat.indexOf("electrical_resistivity")).toDouble());
    core->coreMaterial(mat);

    //TABLE_NAME_GAPPING
    QString GappingSqlQuery = sql("SELECT * FROM %1 WHERE model='%2'").arg(TABLE_NAME_GAPPING).arg(coreModel);
    QSqlQuery GappingQuery(GappingSqlQuery, db());
    //GappingQuery.bindValue(":model", coreModel);
    if(!GappingQuery.exec() || !GappingQuery.next()){
        setLastError(GappingQuery.lastError().text());
        qInfo(logCritical()) << "openCoreHelper() method, query for gapping";
        qInfo(logCritical()) << "Sql error:" << GappingQuery.lastError().text();
        qInfo(logCritical()) << "Final SQL Query:" << GappingSqlQuery;
        delete core;
        return nullptr;
    }
    QSqlRecord rec_gap(GappingQuery.record());
    Gapping gap(GappingQuery.value(rec_gap.indexOf("model")).toString(),
                GappingQuery.value(rec_gap.indexOf("actual_relative_permeability")).toInt(),
                GappingQuery.value(rec_gap.indexOf("inductance_factor")).toDouble(),
                GappingQuery.value(rec_gap.indexOf("gap_length")).toDouble(),
                GappingQuery.value(rec_gap.indexOf("actual_core_losses")).toDouble());
    core->coreGapping(gap);

    //TABLE_NAME_GEOMETRY
    QString GeometrySqlQuery = sql("SELECT * FROM %1 WHERE model='%2'").arg(TABLE_NAME_GEOMETRY).arg(coreModel);
    QSqlQuery GeometryQuery(GeometrySqlQuery, db());
    //GeometryQuery.bindValue(":model", coreModel);
    if(!GeometryQuery.exec() || !GeometryQuery.next()){
        setLastError(GeometryQuery.lastError().text());
        qInfo(logCritical()) << "openCoreHelper() method, query for geometry";
        qInfo(logCritical()) << "Sql error:" << GeometryQuery.lastError().text();
        qInfo(logCritical()) << "Final SQL Query:" << GeometrySqlQuery;
        delete core;
        return nullptr;
    }

    if(GeometryQuery.size() == 0) {
        qInfo(logCritical()) << "No geometry found with name" << coreModel;
        delete core;
        return nullptr;
    }

    QSqlRecord rec_geom(GeometryQuery.record());
    Geometry geom(GeometryQuery.value(rec_geom.indexOf("model")).toString(),
                  db::getCoreType(GeometryQuery.value(rec_geom.indexOf("type")).toString()),
                  GeometryQuery.value(rec_geom.indexOf("h")).toDouble(),
                  GeometryQuery.value(rec_geom.indexOf("inner_diam")).toDouble(),
                  GeometryQuery.value(rec_geom.indexOf("outer_diam")).toDouble(),
                  GeometryQuery.value(rec_geom.indexOf("c")).toDouble(),
                  GeometryQuery.value(rec_geom.indexOf("b")).toDouble(),
                  GeometryQuery.value(rec_geom.indexOf("f")).toDouble(),
                  GeometryQuery.value(rec_geom.indexOf("a")).toDouble(),
                  GeometryQuery.value(rec_geom.indexOf("e")).toDouble(),
                  GeometryQuery.value(rec_geom.indexOf("d")).toDouble(),
                  GeometryQuery.value(rec_geom.indexOf("g")).toDouble());
    core->geometry(geom);
    return core;
}

bool db::CoreManager::saveCoreHelper(db::CoreModel *core)
{
    if(!beginTransaction()){
        qInfo(logCritical()) << QString::fromLatin1("Can not begin transaction");
        return false;
    }

    auto executeInsertQuery = [this](const QString& tableName, const QVariantMap& values) {
        QStringList columns = values.keys();
        QStringList placeholders;
        for(const QString& column : columns) {
            placeholders.append(":" + column);
        }
        qDebug() << columns;
        qDebug() << placeholders;

        QString sqlQuery = sql("INSERT OR REPLACE INTO %1(%2) VALUES (%3)")
                .arg(tableName)
                .arg(columns.join(", "))
                .arg(placeholders.join(", "));

        QSqlQuery query(sqlQuery, db());
        for(auto it = values.begin(); it != values.end(); ++it) {
            query.bindValue(":" + it.key(), it.value());
        }

        if(!query.exec()) {
            setLastError(query.lastError().text());
            qInfo(logCritical()) << QString::fromLatin1("Sql error:") << query.lastError().text();
            return false;
        }
        return true;
    };

    // insert TABLE_NAME_MATERIAL
    QVariantMap malerialValues = {
        {"name", core->coreMaterial().materialName},
        {"high_relative_permeability", core->coreMaterial().highRelativePermeability},
        {"coercive_field", core->coreMaterial().coerciveField},
        {"temp_curie", core->coreMaterial().tempCurie},
        {"core_losses_relative", core->coreMaterial().coreLossesRelative},
        {"upper_operating_frequency", core->coreMaterial().upperOperatingFrequency},
        {"flux_density", core->coreMaterial().fluxDensity},
        {"electrical_resistivity", core->coreMaterial().electricalResistivity},
    };
    if(!executeInsertQuery(TABLE_NAME_MATERIAL, malerialValues)) {
        return false;
    }

    // insert TABLE_NAME_GEOMETRY
    QVariantMap geometryValues = {
        {"model", core->geometry().model_},
        {"type", static_cast<int>(core->geometry().type_)},
        {"h", core->geometry().H},
        {"inner_diam", core->geometry().innerDiam},
        {"outer_diam", core->geometry().outerDiam},
        {"c", core->geometry().C},
        {"b", core->geometry().B},
        {"f", core->geometry().F},
        {"a", core->geometry().A},
        {"e", core->geometry().E},
        {"d", core->geometry().D},
        {"g", core->geometry().G},
    };
    if(!executeInsertQuery(TABLE_NAME_GEOMETRY, geometryValues)) {
        return false;
    }

    // insert TABLE_NAME_GAPPING
    QVariantMap gappingValues = {
        {"model", core->coreGapping().modelName},
        {"actual_relative_permeability", core->coreGapping().actualRelativePermeability},
        {"inductance_factor", core->coreGapping().inductanceFactor},
        {"gap_length", core->coreGapping().gapLength},
        {"actual_core_losses", core->coreGapping().actualCoreLosses},
    };
    if(!executeInsertQuery(TABLE_NAME_GAPPING, gappingValues)) {
        return false;
    }

    // insert TABLE_NAME_CORES
    int isGapped = (core->gapped()) ? 1 : 0;
    QString type = getCoreString(core->type());
    QVariantMap coreValues = {
        {"name", core->name()}, // String type, for ex. "EE8"
        {"model", core->model()}, // String type, for ex. "PC47EE8-Z"
        {"gapped", isGapped}, // Int type use 0 is not gapped 1 is gapped
        {"type", type}, // Write string implementation of core type i.e. CoreType::TOR --> "TOR"
        {"material", core->coreMaterial().materialName}, // String type, for ex. "PC47"
        {"gapping", core->coreGapping().modelName}, // String type equal core model name, for ex. "PC47EE8-Z"
        {"resistance_factor", core->resistanceFactor()},
        {"effective_magnetic_volume", core->effectiveMagneticVolume()},
        {"window_cross_section", core->windowCrossSection()},
        {"effective_magnetic_path_length", core->effectiveMagneticPathLength()},
        {"effective_magnetic_cross_section", core->effectiveMagneticCrossSection()},
        {"lengh_turn", core->lengthTurn()},
        {"geometry", core->geometry().model_} // String type equal core model name, for ex. "PC47EE8-Z"
    };
    if(!executeInsertQuery(TABLE_NAME_CORES, coreValues)) {
        return false;
    }
    return endTransaction();
}

/*!
 * \brief db::CoreManager::removeCoreHelper - Performs a core
 *  deletion from the database by the given coreId
 * \param coreId - Current the core id for to be deleted
 * \return Should return true if the deletion was successful.
 */
bool db::CoreManager::removeCoreByIdHelper(int coreId)
{
    // Check if a core with the given coreId exists
    QString checkExistenceQuery = sql("SELECT 1 FROM %1 WHERE id = %2").arg(TABLE_NAME_CORES).arg(coreId);
    QSqlQuery checkQuery(checkExistenceQuery, db());
    if(!checkQuery.exec() || !checkQuery.next()) {
        // No core with this coreId found
        return false;
    }

    // Generate a SQL query to delete the core by id
    QString deleteByIdSqlQuery = sql("DELETE FROM %1 WHERE id = %2").arg(TABLE_NAME_CORES).arg(coreId);
    QSqlQuery deleteQuery(deleteByIdSqlQuery, db());
    if(!deleteQuery.exec()) {
        setLastError(deleteQuery.lastError().text());
        qInfo(logCritical()) << QString::fromLatin1("Sql error:") << deleteQuery.lastError().text();
        return false;
    }
    // The deletion was successful
    return true;
}

/*!
 * \brief db::CoreManager::removeCoreByModelHelper - Performs a core
 *  deletion from the database for the specified core model.
 * \param model- Current the core model for to be deleted
 * \return Should return true if the deletion was successful.
 */
bool db::CoreManager::removeCoreByModelHelper(const QString &model)
{
    // Check for existence of a core with the given model
    QString checkExistenceQuery = sql("SELECT 1 FROM %1 WHERE model = %2").arg(TABLE_NAME_CORES).arg(model);
    QSqlQuery checkQuery(db());
    checkQuery.prepare(checkExistenceQuery);
    if(!checkQuery.exec() || !checkQuery.next()) {
        // No core with this core model found
        return false;
    }

    // Generate a SQL query to delete the core by model
    QString deleteByNameSqlQuery = sql("DELETE FROM %1 WHERE model = %2").arg(TABLE_NAME_CORES).arg(model);
    QSqlQuery deleteQuery(db());
    deleteQuery.prepare(deleteByNameSqlQuery);
    if(!deleteQuery.exec()) {
        setLastError(deleteQuery.lastError().text());
        qInfo(logCritical()) << QString::fromLatin1("Sql error:") << deleteQuery.lastError().text();
        return false;
    }

    // The deletion was successful
    return true;
}

/*!
 * \brief db::CoreManager::removeMaterial - The function is designed
 *  to delete material from the database by its name.
 * \param name - Material name
 * \return Should return true if the deletion was successful.
 */
bool db::CoreManager::removeMaterial(const QString &name)
{
    // Check for existence of a material with the given mane
    QString checkExistenceQuery = sql("SELECT 1 FROM %1 WHERE name = %2").arg(TABLE_NAME_MATERIAL).arg(name);
    QSqlQuery checkQuery(db());
    checkQuery.prepare(checkExistenceQuery);
    if(!checkQuery.exec()) {
        // No material with this material name found
        return false;
    }

    // Generate a SQL query to delete the material by name
    QString deleteByNameSqlQuery = sql("DELETE FROM %1 WHERE name = %2").arg(TABLE_NAME_MATERIAL).arg(name);
    QSqlQuery deleteQuery(db());
    deleteQuery.prepare(deleteByNameSqlQuery);
    if(!deleteQuery.exec()) {
        setLastError(deleteQuery.lastError().text());
        qInfo(logCritical()) << QString::fromLatin1("Sql error:") << deleteQuery.lastError().text();
        return false;
    }

    // The deletion was successful
    return true;
}
