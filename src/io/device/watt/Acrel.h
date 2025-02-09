#ifndef ACREL_METER_H_
#define ACREL_METER_H_

#include "../../../db/Device.h"

class TcpClient;

class Acrel {
  public:
    Acrel();
    virtual ~Acrel();

    //设置TcpClient指针
    void SetTcp(TcpClient *tcpClient);

    /*
     *  Read
     *  功能: 读取惠测电表数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int Read(Device::SlaveDev *dev);

  protected:
  private:
    int AEM96Read(Device::SlaveDev *dev);

    int APMRead(Device::SlaveDev *dev);

    int DJSF1352Read(Device::SlaveDev *dev);

  protected:
  private:
    int CTRatio;
    TcpClient *m_tcpClient; //以太网，用于串口服务器
    int VoltDpt;            // used for DJSF1352
    int VoltValue;          // used for DJSF1352
    int CurrentDpt;         // used for DJSF1352
    int CurrentValue;       // used for DJSF1352
    int PowerDpt;           // used for DJSF1352
    int PowerValue;         // used for DJSF1352
};

#endif
