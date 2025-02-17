#include <base/dbmanager.h>
#include <inc/loggercategories.h>

QString db::DBManager::DBASE_COMMON_NAME = QString("base.db");

db::DBManager::DBManager(QObject *parent)
    :QObject(parent)
{
    allocateDb();
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
    // Gets the name of the current database connection.
    QString connectionName = m_db.connectionName();
    // Closes the database connection.
    m_db.close();
    // Removes the connection from the list of connections managed by QSqlDatabase.
    QSqlDatabase::removeDatabase(connectionName);
}

void db::DBManager::setLastError(const QString &msg)
{
    if(!msg.isEmpty()){
        qCritical(logCritical()) << QString::fromLatin1("DB error: \"%1\"").arg(msg);
    }
}

bool db::DBManager::beginTransaction()
{
    if(!m_db.isOpen()){
        setLastError(QString("Database is not open"));
        return false;
    }
    if(!m_db.transaction()){
        setLastError(m_db.lastError().text());
        return false;
    }
    return true;
}

bool db::DBManager::rollback()
{
    if(!m_db.isOpen()){
        setLastError(QString("Database is not open"));
        return false;
    }
    if(!m_db.rollback()){
        setLastError(m_db.lastError().text());
        return false;
    }
    return true;
}

bool db::DBManager::endTransaction()
{
    if(!m_db.isOpen()){
        setLastError(QString("Database is not open"));
        return false;
    }
    if(!m_db.commit()){
        setLastError(m_db.lastError().text());
        return false;
    }
    return true;
}

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

void db::DBManager::allocateDb()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE", connectionName());
    m_db.setDatabaseName(dbName());

    if(!m_db.open()){
        setLastError(m_db.lastError().text());
    }

    QSqlQuery q("PRAGMA foreign_keys = ON", m_db);
    if(!q.exec()){
        setLastError((q.lastError().text()));
    }
}
