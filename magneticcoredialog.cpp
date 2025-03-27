#include "magneticcoredialog.h"
#include "ui_magneticcoredialog.h"

MagneticCoreDialog::MagneticCoreDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MagneticCoreDialog)
{
    ui->setupUi(this);



    /* When the OK button is pressed (the clicked signal),
     * the sendId() slot processes it and sends the sendIdValue() signal,
     * which must be processed in the main window class, in the method setMagneticCoreDialog().*/
    connect(ui->OkButton, &QPushButton::clicked, this, &MagneticCoreDialog::sendId);
}

MagneticCoreDialog::~MagneticCoreDialog()
{
    delete ui;
}

/*!
 * \brief MagneticCoreDialog::setCores - When opening a modal window,
 * after construct this object, we send data from the database to the model
 * in the form of a list of objects with the parameters we need.
 * And we pull the appendCoreRoves method in the model,
 * which adds all this information to the table.
 * \param core_items - list of core data objects.
 */
void MagneticCoreDialog::setCores(const QList<CoreTableItem> &core_items)
{
    m_model->appendCoreRows(core_items);
}

void MagneticCoreDialog::onAppend()
{
    //m_model->
}

void MagneticCoreDialog::sendId()
{
    // Получаем модель выделения
    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();

    // Получаем список выделенных строк
    QModelIndexList selectedRows = selectionModel->selectedRows();

    if(selectedRows.isEmpty()) {
        // Предполагаем, что выделена только одна строка
        QModelIndex rowIndex = selectedRows.first();
        int row = rowIndex.row();

        // Получаем значения ячееки с индексом 0 в выделенной строке
        QModelIndex cellIndex = m_model->index(row, 0);
        int value = m_model->data(cellIndex, Qt::DisplayRole).toInt();
        emit sendIdValue(value);
        accept();

    } else {
        qDebug() << "No row is currently selected.";
    }
}
