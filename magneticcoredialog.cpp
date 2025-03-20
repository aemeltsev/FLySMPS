#include "magneticcoredialog.h"
#include "ui_magneticcoredialog.h"

MagneticCoreDialog::MagneticCoreDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MagneticCoreDialog)
{
    ui->setupUi(this);
}

MagneticCoreDialog::~MagneticCoreDialog()
{
    delete ui;
}
