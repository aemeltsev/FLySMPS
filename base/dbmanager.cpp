#include <base/dbmanager.h>
#include <inc/loggercategories.h>

QString db::DBManager::DBASE_COMMON_NAME = QString("base.db");

db::DBManager::DBManager(QObject *parent)
    :QObject(parent)
{
}

db::DBManager::~DBManager()
{
    closeAll();
}

/*!
 * \brief db::DBManager::isOpen - Checks if the database is open.
 * \return boolean value
 */
bool db::DBManager::isOpen() const
{
    return m_db.isOpen();
}

/*!
 * \brief db::DBManager::sql - The method is designed to convert
 *  a SQL query string from const char* to QString. The method
 *  assumes that the sql string is encoded in Latin-1 encoding.
 *  Here is an example of what it might look like
 *  QString queryString = dbManager.sql("SELECT * FROM cores WHERE id = ?");
 *  QSqlQuery query(dbManager.db());
 *  query.prepare(queryString);
 *  query.addBindValue(coreId);
 *  if (query.exec()) {
 *      // Processing query results
 *  } else {
 *      // Processing error
 *      qDebug() << "Query failed:" << query.lastError().text();
 *  }
 * \param sql SQL query string
 * \return QString converted from char*
 */
QString db::DBManager::sql(const char *sql) const
{
    return QString::fromLatin1(sql);
}

/*!
 * \brief db::DBManager::tableExists - Checks if a table exists
 *  in a SQLite database. "sqlite_master" - A master list of
 *  all database objects in the database
 *  and the SQL used to create each object. "type" - The type
 *  of database object, such as a table, index, trigger, or view.
 *  "name" - The name of the database object.
 * \param table - The name of the database table.
 * \return The result from the query, if there is one,
 *  then the table exists.
 */
bool db::DBManager::tableExists(const QString &table)
{
    QSqlQuery q(QString::fromLatin1("SELECT name FROM sqlite_master WHERE type = \"table\" AND name = \"%1\"").arg(table), m_db);
    if(!q.exec())
    {
        qInfo(logCritical()) << QString::fromLatin1("'table' exist failed: \"%1\"").arg(table) << q.lastError();
        return false;
    }
    return q.next();
}

/*!
 * \brief db::DBManager::db
 * \return Returns a QSqlDatabase object
 */
QSqlDatabase &db::DBManager::db()
{
    return m_db;
}

/*!
 * \brief db::DBManager::closeAll - The method is intended to
 *  close the database connection and remove it from the list of
 *  connections managed by QSqlDatabase.
 */
void db::DBManager::closeAll()
{
    // Checking if the connection is open
    if(m_db.isOpen()){
        // Gets the name of the current database connection.
        QString connectionName = m_db.connectionName();
        // Closes the database connection.
        m_db.close();
        // Removes the connection from the list of connections managed by QSqlDatabase.
        QSqlDatabase::removeDatabase(connectionName);
        qInfo(logInfo()) << "Database connection closed and removed:" << connectionName;
    } else {
        qWarning(logWarning()) << "Database connection is already closed.";
    }
}

/*!
 * \brief db::DBManager::setLastError - Sets the last error
 * \param msg - Error message
 */
void db::DBManager::setLastError(const QString &msg)
{
    if(!msg.isEmpty()){
        qCritical(logCritical()) << QString::fromLatin1("DB error: \"%1\"").arg(msg);
    }
}

/*!
 * \brief db::DBManager::beginTransaction - The method is designed to
 *  start a transaction in the database. This method can be used
 *  before performing a series of database operations that must be atomic.
 *  if (dbManager.beginTransaction()) {
 *     // Perform database operations
 *     if (all operations are successful) {
 *         dbManager.endTransaction(); // We are fixing the transaction
 *     } else {
 *         dbManager.rollback(); // Roll back the transaction in case of an error
 *     }
 * } else {
 *     // Handling transaction start error
 * }
 * \return If the transaction is started successfully, the method returns true.
 */
bool db::DBManager::beginTransaction()
{
    qInfo(logInfo()) << "Start transaction to " << dbName();
    if(!m_db.isOpen()){
        setLastError(QString("Database is not open"));
        return false;
    }
    if(!m_db.transaction()){
        setLastError(m_db.lastError().text());
        return false;
    }
    qInfo(logInfo()) << "Manager ready to start transaction - OK";
    return true;
}

/*!
 * \brief db::DBManager::rollback - The method is designed to
 *  roll back a transaction in the database.
 * \return If the rollback is successful, the method returns true.
 */
bool db::DBManager::rollback()
{
    qInfo(logInfo()) << "Start roll back";
    if(!m_db.isOpen()){
        setLastError(QString("Database is not open"));
        return false;
    }
    if(!m_db.rollback()){
        setLastError(m_db.lastError().text());
        return false;
    }
    qInfo(logInfo()) << "Rollback of write attempt was successful - OK";
    return true;
}

/*!
 * \brief db::DBManager::endTransaction - The method is designed to
 *  commit a transaction to the database.
 * \return If the commit is successful, the method returns true.
 */
bool db::DBManager::endTransaction()
{
    qInfo(logInfo()) << "End transaction to " << dbName();
    if(!m_db.isOpen()){
        setLastError(QString("Database is not open"));
        return false;
    }
    if(!m_db.commit()){
        setLastError(m_db.lastError().text());
        return false;
    }
    qInfo(logInfo()) << "Manager ready to end transaction - OK";
    return true;
}

/*!
 * \brief db::DBManager::dropTable - The method is designed to delete
 *  a table from the database. The method creates a SQL query to delete
 *  the table using the sql() method, which formats the query string.
 * \param table - The name of table who is being deleted. Typical, it used
 *  to remove a table from the database if it is no longer needed.
 * if (dbManager.dropTable("table_name")) {
 *     // Table successfully deleted
 * } else {
 *     // Handling table deletion error
 * }
 * \return If the request is successful, the method returns true.
 */
bool db::DBManager::dropTable(const QString &table)
{
    QString sqlQuery = sql("DROP TABLE %1").arg(table);
    QSqlQuery q(sqlQuery, m_db);
    if(!q.exec()){
        setLastError(q.lastError().text());
        return false;
    }
    return true;
}

/*!
 * \brief db::DBManager::allocateDb - The method is designed to
 *  configure and open a connection to the SQLite database.
 *  The method adds a SQLite database using QSqlDatabase::addDatabase(),
 *  passing the connection name returned by the connectionName() method.
 *  Sets the name of the database file using the dbName() method.
 *  Attempts to open the database. If opening fails, sets an error message.
 *  Executes the SQL query PRAGMA foreign_keys = ON to enable
 *  foreign key support in SQLite.
 * dbManager.allocateDb();
 * if (dbManager.isOpen()) {
 *     // Perform database operations
 * } else {
 *     // Handling database open error
 * }
 */
void db::DBManager::allocateDb()
{
    qInfo(logInfo()) << "Attempting to allocate a database" << dbName();
    m_db = QSqlDatabase::addDatabase("QSQLITE", connectionName());
    m_db.setDatabaseName(dbName());

    if(!m_db.open()){
        setLastError(m_db.lastError().text());
    }

    QSqlQuery q("PRAGMA foreign_keys = ON", m_db);
    if(!q.exec()){
        setLastError((q.lastError().text()));
    }
    qInfo(logInfo()) << "Database allocation was successful - OK";

}
