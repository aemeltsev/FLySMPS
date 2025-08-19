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
    void applyCoreDataToForm(const db::CoreModel* core);

signals:
    // TODO: review this sentences
    void sendIdValue(int value); // Сигнал: выбор ядра по id
    void requestCore(int id); // Сигнал: запрос ядра по id

public slots:
    void handleCorelReceived(const db::CoreModel* core); // Слот: получение ядра

private slots:
    void onAppend(); // Слот: добавление информации о ядре в базу данных
    void sendId(); // Слот: выбор id в таблице представления, пересыдка сигналом для поиска в базе
    void seeDetail(); // Слот: выбор id для уточнения параметров ядра

private:
    void logToFile(const QString &message);
    Ui::MagneticCoreDialog *ui;
    CoreTabModel *m_model;
};

#endif // MAGNETICCOREDIALOG_H
