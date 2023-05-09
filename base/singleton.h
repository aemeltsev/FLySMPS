#ifndef SINGLETON_H
#define SINGLETON_H
#include <QMutex>

template<class T>
class Singleton
{
public:

    static T* instance()
    {
        if(m_instance == nullptr)
        {
            m_mutex.lock();
            if(m_instance == nullptr) m_instance = new T;
        }
        return m_instance;
    }

    static void drop()
    {
        if(m_instance){
            m_mutex.lock();
            if(m_instance)
            {
                delete m_instance;
                m_instance = nullptr;
            }
        }
    }

protected:
    Singleton(){}
    virtual ~Singleton(){}

    Q_DISABLE_COPY(Singleton<T>)

private:
    static T* m_instance;
    static QMutex m_mutex;
};

template<typename T> T* Singleton<T>::m_instance = nullptr;
template<typename T> QMutex Singleton<T>::m_mutex;

#endif // SINGLETON_H
