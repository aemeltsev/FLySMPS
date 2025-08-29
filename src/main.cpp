/**
  Copyright 2021 Anton Emeltsev

  This file is part of FSMPS - asymmetrical converter model estimate.

  FSMPS tools is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  FSMPS tools is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see http://www.gnu.org/licenses/.
*/
/**
  For logging implementation see https://github.com/tvabramov/qt-sample-logging
*/
#include <QApplication>
//#include <QScopedPointer>
#include <QThread>
#include "inc/FLySMPS.h"
#include "inc/loggercategories.h"
#include "inc/logfilewriter.h"

QThread *log_thread;
LogFileWriter *log_writer;

/*//Message handler - qDebug, qInfo and etc
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    log_writer->push(QDateTime::currentDateTime(), type, QString(context.category), msg);
}*/

int main(int argc, char *argv[])
{
    // Регистрируем QtMsgType для передачи через потоки
    qRegisterMetaType<QtMsgType>("QtMsgType");

    // Run the logger in a separate thread
    log_thread = new QThread();
    //log_writer = new LogFileWriter("fsmps-logging-out", static_cast<qint64>(FILE_MAX_SIZE));
    log_writer = LogFileWriter::instance("fsmps-logging-out", static_cast<qint64>(FILE_MAX_SIZE));
    log_writer->moveToThread(log_thread);
    QObject::connect(log_thread, &QThread::started, log_writer, &LogFileWriter::main_loop);
    /** The LogFileWriter class uses its main_loop() loop,
     *  i.e. does not exit this function -> the thread event loop is blocked.
     *  Therefore, you need to use Qt::DirectConnection.
     */
    QObject::connect(log_writer, &LogFileWriter::finished, log_thread, &QThread::quit, Qt::DirectConnection);
    log_thread->start();

    // Set global handler BEFORE creating any windows
    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg){
        QDateTime timestamp = QDateTime::currentDateTime();
        LogFileWriter::instance()->push(timestamp, type, context.category, msg);
    });

    QApplication a(argc, argv);
    FLySMPS w;
    w.show();

    //Reset handler
    qInstallMessageHandler(0); //If not, we can send a message to a non-existent object.

    log_writer->abort();
    log_thread->wait(5000); //Better to limit the waiting time

    delete log_thread;
    //delete log_writer;

    return a.exec();
}
