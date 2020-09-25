#include "FLySMPS.h"
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QScopedPointer>
#include <QTextStream>
#include <QDateTime>
#include <QLoggingCategory>

//Pointer to log file
static QScopedPointer<QFile> fbsmps_log;

//Handler protopype
void messageHandler(QtMsgType type, const QMessageLogContext &context,
                    const QString &msg);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Set logs file;
    //path file in root path app
    fbsmps_log.reset(new QFile("fbsmps_log.txt"));
    //Open file for logging
    fbsmps_log.data()->open(QFile::Append | QFile::Text);
    //Set handler
    qInstallMessageHandler(messageHandler);

    FLySMPS w;
    w.show();

    return a.exec();
}

//Handler declare
void messageHandler(QtMsgType type, const QMessageLogContext &context,
                    const QString &msg)
{
    //Open stream write to file
    QTextStream out(fbsmps_log.data());
    //Write date
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");
    //Select the type message
    switch (type)
    {
    case QtInfoMsg:     out << "INF "; break;
    case QtDebugMsg:    out << "DBG "; break;
    case QtWarningMsg:  out << "WRN "; break;
    case QtCriticalMsg: out << "CRT "; break;
    case QtFatalMsg:    out << "FTL "; break;
    }
    //Write to out type message and data
    out << context.category << ": "
        << msg << endl;
    out.flush();    //Clear buffer
}
