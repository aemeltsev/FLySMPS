/**
  See https://github.com/tvabramov/qt-sample-logging
*/
#include <QTextStream>
#include "logfilewriter.h"

LogFileWriter::LogFileWriter(QString prefix, qint64 maxsize, QObject *parent)
    :QObject(parent)
    ,m_prefix(prefix)
    ,m_max_file_size(maxsize)
    ,m_abort(false)
{}

LogFileWriter::~LogFileWriter()
{
    QMutexLocker locker(&m_mutex);
    m_abort = true;
    m_condition.wakeAll();
}

void LogFileWriter::push(QDateTime timestamp, QtMsgType type, QString category, QString msg)
{
    QMutexLocker locker(&m_mutex);
    m_data.enqueue(log_quantum(timestamp, type, category, msg));
    m_condition.wakeAll();
}

void LogFileWriter::abort()
{
    QMutexLocker loker(&m_mutex);

    m_abort = true;
    m_data.enqueue(log_quantum(QDateTime::currentDateTime(), QtInfoMsg, "LogWriter", "Abort requested"));

    m_condition.wakeAll();
}

void LogFileWriter::main_loop()
{
    forever
    {
        m_mutex.lock();

        if(m_data.isEmpty() && !m_abort){
            m_condition.wait(&m_mutex);
        }

        bool abort = m_abort;
        QQueue<log_quantum> lqq = QQueue<log_quantum>(std::move(m_data));

        m_mutex.unlock();

        while(!lqq.isEmpty()){

            log_quantum lq = lqq.dequeue();

            //If file is not open, open him
            if(!m_file.isOpen()){

                m_file.setFileName(QString("./%1_%2.log").arg(m_prefix).arg(lq.m_timestamp.toString("yyyy-MM-dd_hh-mm-ss")));
                m_file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
                if(m_file.pos() == 0){
                    QTextStream out(&m_file);
                    out << "Time\tLevel\tSource\tMessage" << endl;
                    out.flush();
                }
            }

            //If the file is too large, reopen him
            if(m_file.size() > m_max_file_size){

                m_file.close();
                m_file.remove();
                m_file.setFileName(QString("./%1_%2.log").arg(m_prefix).arg(lq.m_timestamp.toString("yyyy-MM-dd_hh-mm-ss")));
                m_file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);

                QTextStream out(&m_file);
                out << "Time\tLevel\tSource\tMessage" << endl;
                out.flush();
            }

            //Open write to file stream
            QTextStream out(&m_file);
            //Write date of the recording
            out << lq.m_timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz") << "\t";
            //By type, select which level the message belongs
            switch(lq.m_msgtype)
            {
            case QtInfoMsg:     out << "INF "; break;
            case QtDebugMsg:    out << "DBG "; break;
            case QtWarningMsg:  out << "WRN "; break;
            case QtCriticalMsg: out << "CRT "; break;
            case QtFatalMsg:    out << "FTL "; break;
            }
            out << "\t";

            out << lq.m_category << "\t";

            //Write into output category of the message and the message itself
            out << lq.m_msg << endl;
        }

        if(abort){
            emit finished();
            return;
        }
    }
}
