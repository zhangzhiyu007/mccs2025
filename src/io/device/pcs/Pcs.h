/*
 * Pcs.h
 *
 *   创建日期: 2013-6-24
 *   作         者: mzh
 */

#ifndef PCS_H_
#define PCS_H_
#include "../../../db/Device.h"
#include "EcmPcs.h"

class Com;
class TcpClient;

/*名称:Pcs
 *描述:用于PCS控制与数据采集
 *用法:
 *创建人:mzh
 *创建日期:2013-6-24
 */
class Pcs {
    //方法
  public:
    Pcs();
    virtual ~Pcs();

    //设置是否采用以太网通讯
    void SetIsTcp(bool isTcp);

    //保存串口指针
    void SetCom(Com *com);

    //设置TcpClient指针
    void SetTcp(TcpClient *tcpClient);

    /*
     *  Read
     *  功能: 读取PCS数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int Read(Device::SlaveDev *dev);

    /*
     *  Preset
     *  功能: 设置PCS命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int Preset(Device::SlaveDev *dev);

  protected:
  private:
    /*
     *  Read
     *  功能: 读取PCS数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int BydPcsRead(Device::SlaveDev *dev);

    /*
     *  Preset
     *  功能: 设置PCS命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int BydPcsPreset(Device::SlaveDev *dev);

    /*
     *  SunGrowPcsRead
     *  功能: 读取PCS数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int SunGrowPcsRead(Device::SlaveDev *dev);

    /*
     *  SunGrowPcsPreset
     *  功能: 设置PCS命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int SunGrowPcsPreset(Device::SlaveDev *dev);

    //	/*
    //	 *  NjPcsRead
    //	 *  功能: 读取PCS数据
    //	 *  输入：
    //	 *  dev    :Device::SlaveDev* 从设备指针
    //	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
    //	 */
    //    int NjPcsRead(Device::SlaveDev* dev);
    //
    //	/*
    //	 *  NjPcsPreset
    //	 *  功能: 设置PCS命令
    //	 *  输入：
    //	 *  dev    :Device::SlaveDev* 从设备指针
    //	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
    //	 */
    //    int NjPcsPreset(Device::SlaveDev* dev);

    /*
     *  BosPcsRead
     *  功能: 读取博奥斯PCS数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int BosPcsRead(Device::SlaveDev *dev);

    /*
     *  BosPcsPreset
     *  功能: 设置博奥斯PCS命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int BosPcsPreset(Device::SlaveDev *dev);

    /*
     *  SinexcelPcsRead
     *  功能: 读取深圳盛弘PCS数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int SinexcelPcsRead(Device::SlaveDev *dev);

    /*
     *  SinexcelPcsPreset
     *  功能: 设置深圳盛弘PCS命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int SinexcelPcsPreset(Device::SlaveDev *dev);

    /*
     *  EcmPcsRead
     *  功能: 读取艾克迈PCS数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int EcmPcsRead(Device::SlaveDev *dev);

    /*
     *  EmcPcsPreset
     *  功能: 设置艾克迈PCS命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int EcmPcsPreset(Device::SlaveDev *dev);

    /*
     *  BipolarAcdcRead
     *  功能: 读取双极性AC/DC数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int BipolarAcdcRead(Device::SlaveDev *dev);

    /*
     *  BipolarAcdcPreset
     *  功能: 设置双极性AC/DC命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int BipolarAcdcPreset(Device::SlaveDev *dev);

    int JingshiPcsRead(Device::SlaveDev *dev);

    int JingshiPcsPreset(Device::SlaveDev *dev);

    /*
     *  EcmHFIPcsRead
     *  功能: 读取艾科迈HFI PCS数据
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int EcmHFIPcsRead(Device::SlaveDev *dev);

    /*
     *  EcmHFIPcsPreset
     *  功能: 设置艾科迈HFI PCS命令
     *  输入：
     *  dev    :Device::SlaveDev* 从设备指针
     *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
     */
    int EcmHFIPcsPreset(Device::SlaveDev *dev);
    
    //变量
  public:
  protected:
  private:
    Com *m_com;
    TcpClient *m_tcpClient; //以太网
    bool m_isTcp;
    EcmPcs m_ecmPcs;
};

#endif /* PCS_H_ */
