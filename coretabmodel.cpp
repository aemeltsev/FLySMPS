#include <inc/loggercategories.h>
#include "coretabmodel.h"

CoreTabModel::CoreTabModel(QObject *parent)
    :QAbstractTableModel (parent)
{
}

int CoreTabModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_cores.size();
}

int CoreTabModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(Column::LAST);
}

/*!
 * \brief CoreTabModel::data - function returns a QVariant, which is a type that can hold various kinds of data.
 * \param index - A QModelIndex object that specifies the location of the data in the model.
 * \param role - An integer that specifies the role of the data being requested (e.g., display, edit).
 * \return If all conditions are met, the function returns the data from m_persons at the specified row and column.
 * \details - !index.isValid(): Checks if the index is valid.
 *            If it's not valid, the function returns an invalid QVariant.
 *            m_persons.count() <= index.row(): Checks if the row specified by index is within the bounds of m_persons.
 *            If it's out of bounds, the function returns an invalid QVariant.
 *            role != Qt::DisplayRole && role != Qt::EditRole: Checks if the role is either Qt::DisplayRole or Qt::EditRole.
 *            If it's neither, the function returns an invalid QVariant.
 */
QVariant CoreTabModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || m_cores.size() <= index.row() || (role != Qt::DisplayRole && role != Qt::EditRole)) {
        return QVariant();
    }

    return m_cores[index.row()][Column(index.column())];
}

/*!
 * \brief CoreTabModel::setData - The function changes the data in the "cell" referenced by the index object.
 * \param index - A QModelIndex object indicating the location of the data in the model.
 * \param value - A QVariant object containing the new value to be set.
 * \param role - An integer indicating the role of the data (e.g. editing)
 * \return - Return true if data edited successful
 */
bool CoreTabModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid() || role != Qt::EditRole || m_cores.size() <= index.row()) {
        return false;
    }

    m_cores[index.row()][Column(index.column())] = value;
    emit dataChanged(index, index);
    return true;
}

/*!
 * \brief CoreTabModel::headerData - The headerData method is used to provide header data to the model.
 * \param section - If the orientation is Qt::Horizontal,
 *                  the method uses a switch statement to determine what
 *                  data to return depending on the value of section.
 * \param orientation - If the orientation is Qt::Vertical,
 *                      the method returns the section number.
 *                      This means that for vertical headers (usually rows),
 *                      their index will be displayed.
 * \param role - The method first checks whether role matches the Qt::DisplayRole value.
 *               This is the standard role used to display data in controls.
 * \return
 */
QVariant CoreTabModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole) {
        return QVariant();
    }

    if(orientation == Qt::Vertical) {
        return section;
    }

    switch (section) {
    case static_cast<int>(Column::ID):
        return "Id";
    case static_cast<int>(Column::MODEL):
        return "Model";
    case static_cast<int>(Column::TYPE_GEOMETRY):
        return "Type Geometry";
    case static_cast<int>(Column::GAPPED):
        return "Gapped";
    case static_cast<int>(Column::MATERIAL):
        return "Material";
    default:
        return QVariant();
    }
}

/*!
 * \brief CoreTabModel::flags - This method is used to specify
 * the element flags for a given index in the model.
 * Element flags define properties of elements in the model,
 * such as whether they can be edited, selected, etc.
 * \param index - A QModelIndex object indicating the location of the data in the model.
 * \return The method returns the item's flags,
 * which may include the Qt::ItemIsEditable flag if conditions are met.
 */
Qt::ItemFlags CoreTabModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
#if DEBUG
    if(index.isValid()) {
        if(index.column() == static_cast<int>(Column::SELECTION)) {
            flags |= Qt::ItemIsEditable;
        }
    }
#endif
    return flags;
}

void CoreTabModel::appendCoreRow(const int id, const QString &model, const QString &geom, const bool gapped, const QString &mat)
{
    if(!isHaveDuplicate(id)) {
        CoreTableData core_data;
        core_data[Column::ID] = id;
        core_data[Column::MODEL] = model;
        core_data[Column::TYPE_GEOMETRY] = geom;
        core_data[Column::GAPPED] = gapped;
        core_data[Column::MATERIAL] = mat;
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
        core_data[Column::ID] = item.m_id;
        core_data[Column::MODEL] = item.m_model;
        core_data[Column::TYPE_GEOMETRY] = item.m_geom;
        core_data[Column::GAPPED] = item.m_gapped;
        core_data[Column::MATERIAL] = item.m_mat;
        /* maybe add element to hash with properties core_data[SELECTION] = false */
        m_cores.append(core_data);
    }

    // Завершаем добавление строки
    endInsertRows();
}

/*!
 * \brief CoreTabModel::isHaveDuplicate
 * \param id
 * \return 
 */
bool CoreTabModel::isHaveDuplicate(const int id)
{
    // Проверка на дубликаты
    for (const auto& core : m_cores) {
        if (core[Column::ID].toInt() == id) {
            qWarning(logWarning()) << "Core with ID" << id << "already exists.";
            return true;
        }
    }
    return false;
}

