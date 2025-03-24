#include <inc/loggercategories.h>
#include "coretabmodel.h"

CoreTabModel::CoreTabModel(QObject *parent)
{

}

int CoreTabModel::rowCount(const QModelIndex &parent) const
{

}

int CoreTabModel::columnCount(const QModelIndex &parent) const
{

}

QVariant CoreTabModel::data(const QModelIndex &index, int role) const
{

}

bool CoreTabModel::setData(const QModelIndex &index, const QVariant &value, int role)
{

}

QVariant CoreTabModel::headerData(int section, Qt::Orientation orientation, int role) const
{

}

Qt::ItemFlags CoreTabModel::flags(const QModelIndex &index) const
{

}

void CoreTabModel::appendCoreRow(const int id, const QString &model, const QString &geom, const bool gapped, const QString &mat)
{
    if(!isHaveDuplicate(id)) {
        CoreTableData core_data;
        core_data[ID] = id;
        core_data[MODEL] = model;
        core_data[TYPE_GEOMETRY] = geom;
        core_data[GAPPED] = gapped;
        core_data[MATERIAL] = mat;
        /* maybe add element to hash with properties core_data[SELECTION] = false */

        int row = m_cores.count();

        // Уведомляем представление о том, что будут добавлена новая строка
        beginInsertRows(QModelIndex(), row, row);

        m_cores.append(core_data);

        // Завершаем добавление строки
        endInsertRows();
    } else{
        return;
    }
}

void CoreTabModel::appendCoreRows(const QList<CoreTableItem> &items)
{
    int initial_row_count = m_cores.count();
    int new_row_count = items.size();

    // Уведомляем представление о том, что будут добавлена новая строка
    beginInsertRows(QModelIndex(), initial_row_count, initial_row_count + new_row_count - 1);

    for(const auto& item : items){
        CoreTableData core_data;
        core_data[ID] = item.m_id;
        core_data[MODEL] = item.m_model;
        core_data[TYPE_GEOMETRY] = item.m_geom;
        core_data[GAPPED] = item.m_gapped;
        core_data[MATERIAL] = item.m_mat;
        /* maybe add element to hash with properties core_data[SELECTION] = false */
        m_cores.append(core_data);
    }

    // Завершаем добавление строки
    endInsertRows();
}

bool CoreTabModel::isHaveDuplicate(const int id)
{
    // Проверка на дубликаты
    for (const auto& core : m_cores) {
        if (core[ID].toInt() == id) {
            qWarning(logWarning()) << "Core with ID" << id << "already exists.";
            return true;
        }
    }
    return false;
}

