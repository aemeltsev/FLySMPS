#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QMutex>

#include <QtDebug>

namespace db {

class DBManager: public QObject
{
    Q_OBJECT

public:
    bool isOpen() const;
    const QString& lastError() const;
    virtual QString dbName() const = 0;
    QString sql(const char* sql) const;

protected:
    explicit DBManager(QObject *parent = nullptr);
    virtual ~DBManager();

    bool tableExists(const QString& table);
    virtual QString connectionName() const = 0;
    QSqlDatabase& db();
    void closeAll();
    void setLastError(const QString& msg);

    bool createTable(const QString& table, const QStringList& columns);
    bool beginTransaction();
    bool rollback();
    bool endTransaction();

    bool dropTable(const QString& table);

    QMutex dbMutex_;
    static QString DBASE_COMMON_NAME;
    virtual int lastInsertId(const QString& table) = 0;

private:
    void allocateDb();
    QSqlDatabase m_db;

};
} //namespace db
#endif //DBMANAGER_H
