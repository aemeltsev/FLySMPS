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

bool db::DBManager::isOpen() const
{
    return m_db.isOpen();
}

QString db::DBManager::sql(const char *sql) const
{
    return QString::fromLatin1(sql);
}

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

QSqlDatabase &db::DBManager::db()
{
    return m_db;
}

void db::DBManager::closeAll()
{
    QString connectionName = m_db.connectionName();
    m_db.close();
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
