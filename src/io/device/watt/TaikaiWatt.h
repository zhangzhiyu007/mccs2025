/*
 * QingbiaoWatt.h
 *
 *  创建日期: 2019-7-31
 *  作          者: 刘海涛
 */

#ifndef QINGBIAOWATT_H_
#define QINGBIAOWATT_H_

#include "../../../db/Device.h"

class Com;
class TcpClient;

class TaikaiWatt {
  public:
    TaikaiWatt();
    virtual ~TaikaiWatt();

    //设置是否采用以太网通讯
    void SetIsTcp(bool isTcp);

    //设置TcpClient指针
    void SetTcp(TcpClient *tcpClient);

    /*
     *  Read
     *  功能: 读取青表电表数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int Read(Device::SlaveDev *dev);

  protected:
  private:
    int TK900M_Read(Device::SlaveDev *dev);

  protected:
  private:
    bool m_isTcp;
    TcpClient *m_tcpClient; //以太网，用于串口服务器
};

#endif /* QINGBIAOWATT_H_ */
