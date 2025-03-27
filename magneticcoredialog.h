#ifndef MAGNETICCOREDIALOG_H
#define MAGNETICCOREDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QDebug>
#include "coretabmodel.h"

namespace Ui {
class MagneticCoreDialog;
}

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

private:
    Ui::MagneticCoreDialog *ui;

    CoreTabModel *m_model;
};

#endif // MAGNETICCOREDIALOG_H
