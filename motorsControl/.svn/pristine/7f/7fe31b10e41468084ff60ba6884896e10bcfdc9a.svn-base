#include "autorecoginze.h"
#include "communication.h"
#include "innfosproxy.h"
#include <QTimer>
#include "mediator.h"
#include "userdefine.h"
#include <QThread>
#include <QDebug>

AutoRecognize * AutoRecognize::m_pAutoRecognize = nullptr;
AutoRecognize::AutoRecognize(QObject *parent) :
    QObject(parent),
    m_bFindAvaliable(false),
    m_bTryNext(true)
{
    findAvailablePorts();
    m_motorsInfo.clear();
}

void AutoRecognize::findAvailablePorts()
{
    m_portList.clear();
    QList<QSerialPortInfo> allportList = QSerialPortInfo::availablePorts();
    foreach (QSerialPortInfo info, allportList) {
        QString manufacturer = info.manufacturer();
        if(manufacturer == "FTDI")//filter some serialport that is not FTDI
            m_portList.append(info);
    }
}

AutoRecognize *AutoRecognize::getInstance()
{
    if (!m_pAutoRecognize)
    {
        m_pAutoRecognize = new AutoRecognize;
        //connect(Communication::getInstance(),&Communication::OpenPort,m_pAutoRecognize,&AutoRecognize::onPortOpen);//todo
    }

    return m_pAutoRecognize;
}

void AutoRecognize::autoDestroy()
{
    if(m_pAutoRecognize)
       delete m_pAutoRecognize;
    m_pAutoRecognize = nullptr;
}

void AutoRecognize::startRecognize(bool bRetry)
{
    if(bRetry)
    {
        Communication::getInstance()->stop();
        findAvailablePorts();
        m_motorsInfo.clear();
    }
    for(int i=0;i<m_portList.size();++i)
    {
        QSerialPortInfo info = m_portList.at(i);
        Communication::getInstance()->addCommunication(info.portName(),1500000);
    }

    InnfosProxy::SendProxy(0,D_CAN_CONNECT);
    InnfosProxy::SendProxy(0,D_READ_ADDRESS);
    QTimer::singleShot(800,this,SLOT(waitTimeout()));
}

void AutoRecognize::addMototInfo(quint8 nDeviceId, quint32 nDeviceMac)
{
    m_motorsInfo.insert(nDeviceId,nDeviceMac);
}

void AutoRecognize::openFailed()
{
    m_bTryNext = true;
}

void AutoRecognize::waitTimeout()
{
    mediator->recognizeFinished(m_motorsInfo);
    autoDestroy();
}

