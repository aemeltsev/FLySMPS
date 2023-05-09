#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include <QtDebug>

#include "defines.h"

class DBManager: public QObject
{
    Q_OBJECT
    QSqlDatabase m_db;

public:

    explicit DBManager(QObject *parent = nullptr);
    bool initSQL(QString dbPath="");
    bool createTable(const QString& table, const QStringList& columns);
    bool isOpen() const;
    const QString& lastError() const;
    bool addCore(const Core& core_data);
    bool addMaterial(const Material& material_data);
    bool addGapping(const Gapping& gap_data, bool gapped=true);
    bool addToroidGeometry(const GeometryToroidal& geometry);
    bool addUUGeometry(const GeometryUU& geometry);
    bool addEEGeometry(const GeometryEE& geometry);
    bool addETDGeometry(const GeometryETD& geometry);
    bool removeCore(const QString& name);
    bool removeMaterial(const QString& name);
    bool removeGeometry(const QString& name, CoreType crtype);
    bool getCoreProperties(const QString& core_name,
                           const QString& material_name,
                           Core& out_core,
                           Material& out_material,
                           Gapping& out_gapp,
                           bool gapped=true);
    bool getCoreGeometry(const QString& core_name, QVector<uint16_t> out_gm);
protected:
    bool tableExists(const QString& table);
};

#endif //DBMANAGER_H
