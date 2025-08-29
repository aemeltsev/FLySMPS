#ifndef CORETABMODEL_H
#define CORETABMODEL_H

#include <QAbstractTableModel>
#include "base/coremodel.h"

struct CoreTableItem
{
    bool m_gapped;
    int m_id;
    QString m_model;
    QString m_geom;
    QString m_mat;

    CoreTableItem(){}

    CoreTableItem(bool gapped, int id, QString model, QString geom, QString mat)
        :m_gapped(gapped)
        ,m_id(id)
        ,m_model(model)
        ,m_geom(geom)
        ,m_mat(mat)
    {}
};


class CoreTabModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CoreTabModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void appendCoreRow(const int id, const QString& model, const QString& geom, const bool gapped, const QString& mat);
    void appendCoreRows(const QList<CoreTableItem>& items);

    db::CoreModel* getCoreModel() const;
    void setCoreModel(const db::CoreModel& core_model);


private:

    bool isHaveDuplicate(const int id);

    enum Column
    {
        ID = 0,
        MODEL,
        TYPE_GEOMETRY,
        GAPPED,
        MATERIAL,
        LAST
    };

    // list cores for view in table widget
    typedef QHash<Column, QVariant> CoreTableData;
    typedef QList<CoreTableData> Cores;
    Cores m_cores;

    // core model to send in main form
    db::CoreModel* m_core_model;
};

#endif // CORETABMODEL_H
