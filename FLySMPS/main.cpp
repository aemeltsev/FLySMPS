#include "FLySMPS.h"
#include "loggercategories.h"
#include "logfilewriter.h"
#include <QApplication>
#include <QScopedPointer>
#include <QThread>

QThread *log_thread;
LogFileWriter *log_writer;

//Message handler - qDebug, qInfo and etc
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    log_writer->push(QDateTime::currentDateTime(), type, QString(context.category), msg);
}

int main(int argc, char *argv[])
{
    log_thread = new QThread();
    log_writer = new LogFileWriter("fsmps-logging-out", 2048LL * 1024LL * 1024LL);

    log_writer->moveToThread(log_thread);
    QObject::connect(log_thread, SIGNAL(started()), log_writer, SLOT(main_loop()));
    /** The LogFileWriter class uses its main_loop() loop,
     *  i.e. does not exit this function -> the thread event loop is blocked.
     *  Therefore, you need to use Qt::DirectConnection.
     */
    QObject::connect(log_writer, SIGNAL(finished()), log_thread, SLOT(quit()), Qt::DirectConnection);
    log_thread->start();

    //Set handler
    qInstallMessageHandler(messageHandler);

    QApplication a(argc, argv);
    FLySMPS w;
    w.show();

    //Reset handler
    qInstallMessageHandler(0); //If not, we can send a message to a non-existent object.

    log_writer->abort();
    log_thread->wait(5000); //Better to limit the waiting time

    delete log_thread;
    delete log_writer;

    return a.exec();
}
