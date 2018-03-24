#include "communicateunit.h"
#include <QSerialPort>
#include <QMap>
#include <QVector>
#include "innfosproxy.h"
#include <QDebug>
#include "userdefine.h"

CommunicateUnit::CommunicateUnit(quint32 unitId, const QString portName, quint32 bauRate, QObject *parent) :
    QObject(parent),
    m_nUnitId(unitId),
    m_sPortName(portName),
    m_nBauRate(bauRate),
    m_bStop(false),
    m_nConnectionStatus(UserDefine::NO_CONNECT)
{
    
}

void CommunicateUnit::addRelateId(quint8 id)
{
    if(!isContainsRelateId(id))
        m_relateIdList.append(id);
}

void CommunicateUnit::switchOldRelateIdToNewRelateId(quint8 oldId, quint8 newId)
{
    for(int i=0;i<m_relateIdList.size();++i)
    {
        if(m_relateIdList[i] == oldId)
        {
            m_relateIdList.removeAt(i);
            break;
        }
    }
    m_relateIdList.append(newId);
}

void CommunicateUnit::setRelateIdList(const QList<quint8> &idList)
{
    m_relateIdList.clear();
    m_relateIdList = idList;
}

bool CommunicateUnit::isContainsRelateId(const quint8 id) const
{
    if(m_relateIdList.contains(id))
        return true;
    return false;
}

void CommunicateUnit::sendData(const QByteArray &sendData)
{
    QMutexLocker locker(&m_qmMutex);

    QList<quint8> keys = m_dataMap.uniqueKeys();
    quint8 nDeviceId = sendData[1];
    if(keys.contains(nDeviceId))
    {
        m_dataMap[nDeviceId].push_back(sendData);
    }
    else
    {
        QVector<QByteArray> array;
        array.push_back(sendData);
        m_dataMap.insert(nDeviceId,array);
    }

}

void CommunicateUnit::stopCommunication()
{
    QMutexLocker locker(&m_qmMutex);
    m_bStop = true;
}

bool CommunicateUnit::hasDataWaiting()
{
    QMutexLocker locker(&m_qmMutex);
    QMapIterator<quint8,QVector<QByteArray>> it(m_dataMap);
    while (it.hasNext())
    {
        it.next();
        if(it.value().size()>0)
            return true;
    }
    return false;
}

bool CommunicateUnit::isAvailable() const
{
    return m_relateIdList.size() > 0;
}

void CommunicateUnit::setConnectionStatus(quint8 nStatus)
{
    m_nConnectionStatus += nStatus;
}

quint8 CommunicateUnit::getConnectionStatus() const
{
    return m_nConnectionStatus;
}


void CommunicateUnit::progress()
{
    QSerialPort serial(m_sPortName);
    serial.setBaudRate(m_nBauRate);
    if(!serial.open(QIODevice::ReadWrite))
    {
        emit error(tr("open %1 failed!").arg(m_sPortName));
        return;
    }
    quint8 nWaitTime = 1;
    int nFailCnt = 0;
    QByteArray sendData;

    while (!m_bStop)
    {
        if(serial.waitForReadyRead(nWaitTime))
        {
            QByteArray readData = serial.readAll();
            if(serial.error() == QSerialPort::ReadError)
            {
                serial.clearError();
            }
            emit response(m_nUnitId,readData);
            qDebug()<<"readProxy"<<readData.toHex()<<QTime::currentTime().toString("hh:mm:ss.zzz")<<m_nUnitId;

        }
        //timer.restart();
        if(sendData.size() > 0 && serial.isWritable())
        {
            int nLen = serial.write(sendData);
            if(serial.waitForBytesWritten(30))
            {
                qDebug()<<"SendProxy"<<nLen<<sendData.toHex()<<QTime::currentTime().toString("hh:mm:ss.zzz")<<m_nUnitId;
                sendData.clear();
            }
            else {

                if(++nFailCnt > 2)
                {
                    serial.clearError();
                    sendData.clear();
                    nFailCnt = 0;
                }
            }
        }

        m_qmMutex.lock();
        if(sendData.size()==0)
        {
            QMapIterator<quint8,QVector<QByteArray>> it(m_dataMap);
            while (it.hasNext())
            {
                it.next();
                if(it.value().size()>0)
                {
                    QByteArray data = it.value().front();
                    sendData.append(data);
                    m_dataMap[it.key()].pop_front();
                    quint8 proxyId = data[2];
                    if(proxyId != D_SET_POSITION)//只有不返回的指令才能多条同时发
                        break;
                }
            }
        }
        m_qmMutex.unlock();
    }

    emit finished(m_nUnitId);
}
