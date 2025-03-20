#ifndef MAGNETICCOREDIALOG_H
#define MAGNETICCOREDIALOG_H

#include <QDialog>
#include "base/coremodel.h"

namespace Ui {
class MagneticCoreDialog;
}

class MagneticCoreDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MagneticCoreDialog(QWidget *parent = nullptr);
    ~MagneticCoreDialog();

signals:
    void coreSend(db::CoreModel core);

private slots:
    void sendCore();

private:
    Ui::MagneticCoreDialog *ui;
};

#endif // MAGNETICCOREDIALOG_H
