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

void MagneticCoreDialog::applyCoreDataToForm(const db::CoreModel *core)
{
    // Gapping
    const bool is_gapped = core->gapped();
    db::Gapping gap = core->coreGapping();

    ui->IsGappedCheckBox->setChecked(is_gapped);
    if (is_gapped) {
        ui->GapGLineEdit->setText(QString::number(gap.gapLength));
    } else {
        ui->IsGappedCheckBox->setChecked(false);
        ui->GapGLineEdit->clear();
    }

    ui->GapMuLineEdit->setText(QString::number(gap.actualRelativePermeability));
    ui->GapALLineEdit->setText(QString::number(gap.inductanceFactor));
    ui->GapPvLineEdit->setText(QString::number(gap.actualCoreLosses));

    // Material
    db::Material mat = core->coreMaterial();

    ui->MatNameLineEdit->setText(mat.materialName);
    ui->MatHCLineEdit->setText(QString::number(mat.coerciveField));
    ui->MatPVLineEdit->setText(QString::number(mat.coreLossesRelative));
    ui->MatBSLineEdit->setText(QString::number(mat.fluxDensity));
    ui->MatMuRcLineEdit->setText(QString::number(mat.highRelativePermeability));
    ui->MatTCLineEdit->setText(QString::number(mat.tempCurie));
    ui->MatFHLineEdit->setText(QString::number(mat.upperOperatingFrequency));
    ui->MatRhoCLineEdit->setText(QString::number(mat.electricalResistivity));

    // Geometry
    db::CoreType c_type = core->type();
    db::Geometry geom = core->geometry();
    if(c_type == db::CoreType::TOR) {
        ui->TorCheckBox->setChecked(true);
        ui->GeomTorHLineEdit->setText(QString::number(geom.H));
        ui->GeomTorDInnLineEdit->setText(QString::number(geom.innerDiam));
        ui->GeomTorDOutLineEdit->setText(QString::number(geom.outerDiam));
    } else if(c_type == db::CoreType::EE ||
              c_type == db::CoreType::ETD ||
              c_type == db::CoreType::EP ||
              c_type == db::CoreType::EPX ||
              c_type == db::CoreType::EPO ||
              c_type == db::CoreType::ELP ||
              c_type == db::CoreType::EQ ||
              c_type == db::CoreType::ER ||
              c_type == db::CoreType::EFD ||
              c_type == db::CoreType::EV) {
        ui->ECheckBox->setChecked(true);
        ui->GeomECLineEdit->setText(QString::number(geom.C));
        ui->GeomEFLineEdit->setText(QString::number(geom.F));
        ui->GeomEELineEdit->setText(QString::number(geom.E));
        ui->GeomEBLineEdit->setText(QString::number(geom.B));
        ui->GeomEALineEdit->setText(QString::number(geom.A));
        ui->GeomEDLineEdit->setText(QString::number(geom.D));
    } else if(c_type == db::CoreType::UU ||
              c_type == db::CoreType::UI) {
        ui->UCheckBox->setChecked(true);
        ui->GeomUDLineEdit->setText(QString::number(geom.D));
        ui->GeomUELineEdit->setText(QString::number(geom.E));
        ui->GeomUFLineEdit->setText(QString::number(geom.F));
        ui->GeomUGLineEdit->setText(QString::number(geom.G));
    }

    // Core
    ui->CoreNameLineEdit->setText(core->name());
    ui->CoreTypeLineEdit->setText(db::getCoreString(core->type()));
    ui->CoreVELineEdit->setText(QString::number(core->effectiveMagneticVolume()));
    ui->CoreLELineEdit->setText(QString::number(core->effectiveMagneticPathLength()));
    ui->CoreLNLineEdit->setText(QString::number(core->lengthTurn()));
    ui->CoreModelLineEdit->setText(core->model());
    ui->CorePVLineEdit->setText(QString::number(core->resistanceFactor()));
    ui->CoreANLineEdit->setText(QString::number(core->windowCrossSection()));
    ui->CoreAELineEdit->setText(QString::number(core->effectiveMagneticCrossSection()));
}

void MagneticCoreDialog::handleCorelReceived(const db::CoreModel *core)
{
    if(!core) {
        logToFile("Error. Failed to load core data");
        return;
    }
    applyCoreDataToForm(core);
}


void MagneticCoreDialog::onAppend()
{

    //m_model->
    logToFile("use onAppend() method");
}

/*!
* \brief MagneticCoreDialog::sendId - Sends the ID of the selected row.
* If no row is selected, logs an error and does not send a signal.
*/
void MagneticCoreDialog::sendId()
{
    logToFile("sendId() method called");
    // Get the selection model
    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();

    // Get the list of selected lines
    QModelIndexList selectedRows = selectionModel->selectedRows();

    // Check if something is selected and not more than one line
    if(selectedRows.isEmpty()) {
        logToFile("No row is currently selected.");
        return; // Exit if no row is selected
    }
    if(selectedRows.size() > 1) {
        logToFile("Multiple rows are selected. Only one row should be selected.");
        return; // // Exit if more than one row is selected
    }

    logToFile(QString("Selected rows count: %1").arg(selectedRows.size()));
    // Take the first (and only) selected row
    QModelIndex rowIndex = selectedRows.first();
    int row = rowIndex.row();

    // Get the values of the cell with index 0 in the selected row
    QModelIndex cellIndex = m_model->index(row, 0);

    int value = m_model->data(cellIndex, Qt::DisplayRole).toInt();
    emit sendIdValue(value);
    accept();
}

/*!
* \brief MagneticCoreDialog::seeDetail - Shows details of the selected row.
* If no row is selected, logs an error.
* TODO : review this method
*/
void MagneticCoreDialog::seeDetail()
{
    logToFile("seeDetail() method called");
    // Get the selection model
    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();

    // Get the list of selected lines
    QModelIndexList selectedRows = selectionModel->selectedRows();

    // Check if something is selected and not more than one line
    if(selectedRows.isEmpty()) {
        logToFile("No row is currently selected.");
        return; // Exit if no row is selected
    }
    if(selectedRows.size() > 1) {
        logToFile("Multiple rows are selected. Only one row should be selected.");
        return; // // Exit if more than one row is selected
    }

    // Take the first (and only) selected row
    QModelIndex rowIndex = selectedRows.first();
    int row = rowIndex.row();

    QModelIndex cellIndex = m_model->index(row, 0);
    int id = m_model->data(cellIndex, Qt::DisplayRole).toInt();
    emit requestCore(id);
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
