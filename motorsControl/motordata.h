﻿#ifndef MOTORDATA_H
#define MOTORDATA_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <QVariant>
#include <QTimer>
#include <QXmlStreamReader>
#include <QFile>
#include <QList>

class MotorData : public QObject
{
    Q_OBJECT

public:
    enum Motor_Mode{
        Mode_None,
        Mode_Cur,
        Mode_Vel,
        Mode_Pos,
        Mode_Teaching,
        Mode_Profile_Pos=6,
        Mode_Profile_Vel,
        Mode_Homing,
    };

    enum Motor_Data_Id{
        CUR_IQ_SET,
        CUR_PROPORTIONAL,
        CUR_INTEGRAL,
        CUR_ID_SET,
        CUR_MINIMAL,
        CUR_MAXIMUM,
        CUR_NOMINAL,
        CUR_OUTPUT,
        CUR_MAXSPEED,
        CUR_ACTURAL,
        CUR_DEMAND,
        VEL_SET,
        VEL_PROPORTIONAL,
        VEL_INTEGRAL,
        VEL_MINIMAL,
        VEL_MAXIMUM,
        VEL_ACTURAL,
        VEL_DEMAND,
        POS_SET,
        POS_PROPORTIONAL,
        POS_INTEGRAL,
        POS_DIFFERENTIAL,
        POS_MINIMAL,
        POS_MAXIMUM,
        POS_STEP_ADD,
        POS_MIN_POS,
        POS_MAX_POS,
        POS_HOMING,
        POS_ACTURAL,
        POS_DEMAND,
        PROFILE_POS_MAX_SPEED,
        PROFILE_POS_ACC,
        PROFILE_POS_DEC,
        PROFILE_VEL_MAX_SPEED,
        PROFILE_VEL_ACC,
        PROFILE_VEL_DEC,
        CHART_FREQUENCY,
        CHART_THRESHOLD,
        CHART_SWITCH,
        POS_OFFSET,
        VOLTAGE,
        HOMING_LIMIT,//if
        HOMING_CUR_MAX,
        HOMING_CUR_MIN,
        CURRENT_SCALE,
        VELOCITY_SCALE,
        FILTER_C_STATUS,
        FILTER_C_VALUE,
        FILTER_V_STATUS,
        FILTER_V_VALUE,
        FILTER_P_STATUS,
        FILTER_P_VALUE,
        INERTIA,
        LOCK_ENERGY,
        TEMP_MOTOR,
        TEMP_INVERTER,
        TEMP_PROTECT,
        TEMP_RECOVERY,
        CALIBRATION_SWITCH,
        CALIBRATION_ANGLE,
        MOTOR_SWITCH,
        MOTOR_VERSION,//
        ONLINE_STATUS,
        DEVICE_ID,
        MAC_ID,
        MODE_ID,
        ERROR_ID,
        RESERVE_0,
        RESERVE_1,
        RESERVE_2,
        RESERVE_3,
        DATA_CNT,
        DATA_CHART,//special data,don't need to save
        DATA_INVALID,
    };

    explicit MotorData(const quint8 nDeviceId,const quint32 nDeviceMac,QObject *parent = 0);
    quint8 deviceId() const;
    qint8 requestDeviceId()const;
    quint32 deviceMac()const;
    void setValueByProxy(const int nProxyId,QVariant value);//save value that recive
    void requestAllValue();
    QVariant getValue(Motor_Data_Id nDataId)const;
    QVariant getUserRequestValue(Motor_Data_Id nDataId)const;
    void setValueByUser(Motor_Data_Id nDataId,QVariant value,bool bSendProxy=true);//set value by user, will send proxy or not
    void responseHeart(bool bSuccess);
    bool isOnline()const;
    QVector<quint16> errorHistory()const{
        return m_errorHistory;
    }
    Motor_Mode currentMode()const;
    ~MotorData();

    bool deviceIdIsAvailable()const;
    void changeDemand(qreal value);//demand value change
    void switchAutoRequestActual(bool bStart);
    void setAutoRequestInterval(quint32 mSec);

    void saveAllParams();
    void clearHomingInfo();
    void setHomingOperationMode(const quint8 nMode);
    void openChartChannel(const int nChannelId);
    void closeChartChannel(const int nChannelId);
    void switchChartAllChannel(bool bOn);
    void switchCalibrationVel(quint8 nValue);
    void switchCalibration(quint8 nValue);
    void startCalibration();
    void requestSuccessfully(quint8 nDataId);
protected:
    void setValue(int nDataId,QVariant value,bool bEmitSignal = true);
    void userRequestValue(int nDataId,QVariant value);
private:
    void initData();
public slots:
    void activeModeSuccessfully();
    void saveData();
    void loadData();
    void reconnect();

private slots:
    void requestActualValue();//request value initiatively
private:
    void saveDataToFile(QString fileName);
    void readDataFromFile(QString fileName);
    void readParams(QXmlStreamReader *reader);
signals:
    void acturalVauleChange(Motor_Mode Id,QVector<qreal> values);//only use by chart
    void currentErrorChange(const int nErrorId);
private:
private:
    QVariant m_motorData[DATA_CNT];
    QVariant m_userRequestData[DATA_CNT];
    int m_nHeartFailCnt;//
    QTimer * m_pHeartTimer;
    QTimer * m_pValueTimer;
    QVector<quint16> m_errorHistory;

    quint32 m_nAutoRequestInterval;
    //log file
    QFile * m_pFileLog;
};


//motordata manger
#define motorDataMgrInstance MotorDataMgr::getInstance()

class MotorDataMgr : public QObject
{
    Q_OBJECT
public:
    static MotorDataMgr * getInstance();
    virtual ~MotorDataMgr();

    qreal getMotorDataAttrValueAsDouble(const quint8 nDeviceId,const MotorData::Motor_Data_Id attrId)const;
    qint32 getMotorDataAttrValueAsInt(const quint8 nDeviceId,const MotorData::Motor_Data_Id attrId)const;
    void setMotorDataAttrByUser(const quint8 nDeviceId,const MotorData::Motor_Data_Id attrId,QVariant value,bool bSend=true);
    void setMotorDataAttrByProxy(const quint8 nDeviceId,int proxyId,QVariant value);//data from proxy
    void setMotorDataAttrInBatch(const QList<quint8> idList,const MotorData::Motor_Data_Id attrId,QVariant value,bool bSend=true);
    void AddMotorsData(QMap<quint8,quint32> dataMap);
    QMap<quint8,quint32> getMotorsMap()const;
    QVector<quint16> motorErrorHistory(const quint8 nDeviceId)const;
    bool deviceIdHasExist(quint8 nDeviceId);
    void activeMotorModeSuccessfully(const quint8 nDeviceId);
    void activeMotorModeInBatch(const QList<quint8> idList,const MotorData::Motor_Mode mode);
    void regainAllData(const quint8 nDeviceId);
    void regainData(const quint8 nDeviceId,MotorData::Motor_Data_Id attrId);
    void responseHeart(const quint8 nDeviceId,bool bSuccessfully);

    void switchAutoRequestActual(const quint8 nDeviceId,bool bStart);
    void setAutoRequestInterval(const quint8 nDeviceId,quint32 mSec);
    qint8 getOldDeviceId(const quint8 nNewDeviceId);

    void saveAllParams(const quint8 nDeviceId);
    void clearHomingInfo(const quint8 nDeviceId);
    void setHomingOperationMode(const quint8 nDeviceId,const quint8 nMode);
    void openChartChannel(quint8 nDeviceId, const int nChannelId);
    void closeChartChannel(quint8 nDeviceId, const int nChannelId);
    void switchChartAllChannel(quint8 nDeviceId,bool bOn);
    void switchCalibrationVel(quint8 nDeviceId,quint8 nValue);
    void switchCalibration(quint8 nDeviceId,quint8 nValue);
    void startCalibration(quint8 nDeviceId);
    void requestSuccessfully(const quint8 nDeviceId,const quint8 nProxyId);
    void reconnect(quint8 nDeviceId);
    void clearError(quint8 nDeviceId);
    void sendCmd(quint8 nDeviceId,quint16 cmdId);
    void sendCmd(quint8 nDeviceId,quint16 cmdId,quint8 value);
private:
    MotorDataMgr();
signals:
    void dataChanged(const quint8 nDeviceId,const MotorData::Motor_Data_Id attrId,QVariant value);
    void errorOccured(const quint8 nDeviceId,const quint16 erroId,QString errorInfo);
    void setProxyCallback(const quint8 nProxyId,bool bSuccess);
private:
    class GC{
    public:
        ~GC()
        {
            if(m_pMgr!=nullptr)
            {
                delete m_pMgr;
                m_pMgr = nullptr;
            }
        }
        static GC gc;
    };
private:
    MotorData * getMotorDataById(const quint8 nId)const;
    MotorData * getMotorDataByNewId(const quint8 nId)const;//if device id changed by user, sometimes we need to find the motorData using NewId we set before.
    MotorData * getMotorDataByMac(const quint32 nMac)const;
    bool checkIdUnique(QMap<quint8,quint32> dataMap)const;//check all motors's id si unique or not, if not warning and exit the app
    void handleUnuiqueError(QMap<quint8,quint32> dataMap);
    QVariant getMotorDataAttrValue(const quint8 nDeviceId,const MotorData::Motor_Data_Id attrId)const;
private:
    QList<MotorData *> m_allMotorDatas;
    static MotorDataMgr * m_pMgr;
};


#endif // MOTORDATA_H
