#ifndef MAGNETICCOREDIALOG_H
#define MAGNETICCOREDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QDebug>
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

signals:
    void sendIdValue(int value);

private slots:
    void onAppend();
    void sendId();
    void seeDetail();

private:
    void logToFile(const QString &message);
    Ui::MagneticCoreDialog *ui;

    CoreTabModel *m_model;
};

#endif // MAGNETICCOREDIALOG_H
