#ifndef MAGNETICCOREDIALOG_H
#define MAGNETICCOREDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QDebug>
#include <QFile>

#include <base/coremodel.h>
#include "coretabmodel.h"

namespace Ui {
class MagneticCoreDialog;
}

/*!
 * \class MagneticCoreDialog
 * \brief A dialog for managing magnetic core items.
 *
 * This dialog provides a user interface for selecting, adding, and viewing details
 * of magnetic core items. It uses a table view to display the list of items.
 */
class MagneticCoreDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MagneticCoreDialog(QWidget *parent = nullptr);
    ~MagneticCoreDialog();

    void setCores(const QList<CoreTableItem>& core_items);
    void applyCoreDataToForm(const db::CoreModel* core);

signals:
    void sendIdValue(int value); // Signal: send the ID of the core with which will work in the main program
    void requestCore(int id); // Signal: send the core ID so that the main program can find it in the database

public slots:
    void handleCorelReceived(const db::CoreModel* core); // Slot: take of the core object

private slots:
    void onAppend(); // Slot: add kernel info to database
    void sendId(); // Slot: select id in view table, send signal to search database
    void seeDetail(); // Slot: select id to refine kernel parameters

private:
    void logToFile(const QString &message);
    Ui::MagneticCoreDialog *ui;
    CoreTabModel *m_model;
    QFile m_logFile;
    QString m_logPath;
};

#endif // MAGNETICCOREDIALOG_H
