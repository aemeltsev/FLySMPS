#include <base/dbmanager.h>

DBManager::DBManager(QObject *parent)
{

}

bool DBManager::isOpen() const
{
    return m_db.isOpen();
}

bool DBManager::tableExists(const QString &table)
{
    QSqlQuery q(QString::fromLatin1("SELECT name FROM sqlite_master WHERE type='table' AND name='%1'").arg(table), m_db);
    if(!q.exec())
    {
        qDebug() << QString::fromLatin1("'table' exist failed:").arg(table) << q.lastError();
        return false;
    }
    return q.next();
}
