﻿#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include <QList>
#include <QMap>
#include <QObject>

class CommunicateUnit;

class Communication : public QObject
{
    Q_OBJECT
public:
    static Communication * getInstance();
    void sendDataBeforeDestroyed();
    ~ Communication();
    void addCommunication(const QString &portName,const quint32 nBaudRate);
    void sendData(quint8 nDeviceId,const QByteArray & data);
    bool hasDataWaitToSend();
    void stop();
    void recordRemainCmds();
    void addRelateIdToUnit(quint8 nUnitId,quint8 nRelateId);
    void removeUnAvailablePorts();
    void changeUnitRelateId(quint8 nOldId,quint8 nNewId);
    void setUnitConnectionStatus(quint8 nUnitId,quint8 nStatus);
public slots:
    void unitFinished(quint8 unitId);
protected:
    explicit Communication(QObject * parent = nullptr);
signals:
    void response(quint8 unitId,const QByteArray response);
    void request(const QByteArray &request);
    void connectionError(quint8 unitId,quint16 errorId,QString errorStr);
private:
    class GC{
    public:
        ~GC()
        {
            if(m_pCommucation!=nullptr)
            {
                m_pCommucation->sendDataBeforeDestroyed();
                delete m_pCommucation;
                m_pCommucation = nullptr;
            }
        }
        static GC gc;
    };
private:
    static Communication * m_pCommucation;
    QList<CommunicateUnit *> m_lUnits;
public:
};

#endif // COMMUNICATION_H
