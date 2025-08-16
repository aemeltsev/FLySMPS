#include "magneticcoredialog.h"
#include "ui_magneticcoredialog.h"
#include <QDateTime>
#include <QFile>
#include <QThread>

/*!
 * \brief Constructs a MagneticCoreDialog.
 * \param parent The parent widget.
 *
 * Initializes the UI, sets up the table model, and connects button signals to slots.
 * Logs initialization and button click events.
 */
MagneticCoreDialog::MagneticCoreDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MagneticCoreDialog)
{
    logToFile("Initializing MagneticCoreDialog...");

    ui->setupUi(this);
    m_model = new CoreTabModel(this);
    ui->tableView->setModel(m_model);

    /* When the OK button is pressed (the clicked signal),
     * the sendId() slot processes it and sends the sendIdValue() signal,
     * which must be processed in the main window class, in the method setMagneticCoreDialog().*/
    connect(ui->OkButton, &QPushButton::clicked, this, &MagneticCoreDialog::sendId);

    /*When the Append button is pressed...*/
    connect(ui->AddButton, &QPushButton::clicked, this, &MagneticCoreDialog::onAppend);

    /*When the Detail button is pressed...*/
    connect(ui->DetailButton, &QPushButton::clicked, this, &MagneticCoreDialog::seeDetail);

    /*When the Cancel button is pressed */
    connect(ui->CancelButton, &QPushButton::clicked, this, &MagneticCoreDialog::close);

    logToFile("MagneticCoreDialog initialized successfully");
}

MagneticCoreDialog::~MagneticCoreDialog()
{
    delete ui;
    // m_model удаляется автоматически, так как передан this как parent
}

/*!
 * \brief MagneticCoreDialog::setCores - When opening a modal window,
 * after construct this object, we send data from the database to the model
 * in the form of a list of objects with the parameters we need.
 * And we pull the appendCoreRoves method in the model,
 * which adds all this information to the table.
 * \param core_items - list of core data objects to be added to the model.
 */
void MagneticCoreDialog::setCores(const QList<CoreTableItem> &core_items)
{
    // Check if the model is initialized
    if(!m_model) {
        logToFile("Model is not initialized.");
        return;
    }
    // Check if the input list is empty
    if(core_items.isEmpty()) {
        logToFile("The list of core items is empty.");
        return;
    }

    // Append the core items to the model
    m_model->appendCoreRows(core_items);

    // Log succes
    logToFile(QString("Successfully appended %1 %2").arg(core_items.size()).arg("core items to the model."));
}


void MagneticCoreDialog::onAppend()
{

    //m_model->
    logToFile("use onAppend() method");
}

/*!
 * \brief MagneticCoreDialog::sendId -
 */
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
        logToFile("No row is currently selected.");
    }
}

void MagneticCoreDialog::seeDetail()
{
    //m_model->
    logToFile("use seeDetail() method");
}

void MagneticCoreDialog::logToFile(const QString &message)
{
    // Path to the log file for the near window
    QString logPath = QCoreApplication::applicationDirPath() + "/core_dialog_" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + ".log";

    // Open the file for writing
    QFile logFile(logPath);
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&logFile);

        // Write time and message
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
            << "\t[Select Core Dialog]\t"
            << message
            << endl;

        logFile.close(); // Close the file
    } else {
        // If the file cannot be opened, send an error message to output
        qCritical() << "Failed to open dialog log file! Error:" << logFile.errorString();
    }
}
