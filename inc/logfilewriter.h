/**
  See https://github.com/tvabramov/qt-sample-logging
*/
#ifndef LOGFILEWRITER_H
#define LOGFILEWRITER_H

#include <QObject>
#include <QString>
#include <QMutex>
#include <QFile>
#include <QQueue>
#include <QWaitCondition>
#include <QDateTime>
#define FILE_MAX_SIZE 100 * 1024 * 1024

/**
 * @brief The LogFileWriter - thread safety class for writing log file. Recommended
 *                            move it to thread.
 */

class LogFileWriter : public QObject
{
    Q_OBJECT
public:
    // Remove the constructor to prevent direct instantiation
    LogFileWriter(const LogFileWriter&) = delete;
    LogFileWriter& operator=(const LogFileWriter&) = delete;

    // Method for getting singleton
    static LogFileWriter* instance(QString prefix = "",
                                   qint64 maxsize = static_cast<qint64>(FILE_MAX_SIZE));

    // Other public methods
    void abort();

signals:
    void finished();

public slots:
    void main_loop();
    void push(QDateTime timestamp, QtMsgType type, QString category, QString msg);

private:
    // Private constructor and destructor
    explicit LogFileWriter(QString prefix = "",
                           qint64 maxsize = static_cast<qint64>(FILE_MAX_SIZE),
                           QObject *parent = nullptr);
    ~LogFileWriter();

    // Static variable for singleton
    static LogFileWriter* m_instance;

    struct log_quantum
    {
        QDateTime m_timestamp;
        QtMsgType m_msgtype;
        QString m_category;
        QString m_msg;

        log_quantum():
            m_timestamp(QDateTime::currentDateTime()),
            m_msgtype(QtDebugMsg),
            m_category(""),
            m_msg("")
        {}

        log_quantum(QDateTime timestamp, QtMsgType msgtype, QString category, QString msg):
            m_timestamp(timestamp),
            m_msgtype(msgtype),
            m_category(category),
            m_msg(msg)
        {}
    };

        QString m_prefix;
        qint64 m_max_file_size;
        bool m_abort;
        QFile m_file;
        QQueue<log_quantum> m_data;
        QMutex m_mutex;
        QWaitCondition m_condition;

};

#endif // LOGFILEWRITER_H
