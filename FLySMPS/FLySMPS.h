#ifndef FLYSMPS_H
#define FLYSMPS_H

#include <QMainWindow>

namespace Ui {
class FLySMPS;
}

class FLySMPS : public QMainWindow
{
    Q_OBJECT

public:
    explicit FLySMPS(QWidget *parent = nullptr);
    ~FLySMPS();

private:
    Ui::FLySMPS *ui;
};

#endif // FLYSMPS_H
