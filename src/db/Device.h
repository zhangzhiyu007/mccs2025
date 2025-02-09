/*
 * Device.h
 *
 *   创建日期: 2013-3-5
 *   作         者: mzh
 */

#ifndef DEVICE_H_
#define DEVICE_H_
#include "../util/Util.h"

// 设备类型
/*名称：DeviceType
 *描述：从设备类型常量，用于表示不同从设备类型，如逆变器、汇流箱、负载等
 *使用方法：
 *1、直接使用DeviceType.DEVICE_TYPE_NULL
 *创建人：mzh
 *创建日期：2013-3-5
 */
class DeviceType {
  public:
    // 系统
    const static int DEVICE_TYPE_NULL  = -1; // 无效-1
    const static int DEVICE_TYPE_MCCS  = 0;  // 微电网系统
    const static int DEVICE_TYPE_GRID  = 1;  // 电网、模拟电网
    const static int DEVICE_TYPE_DGS   = 2;  // 柴油发电机系统
    const static int DEVICE_TYPE_LOADS = 3;  // 负载系统
    const static int DEVICE_TYPE_PVS   = 4;  // 光伏系统
    const static int DEVICE_TYPE_WTS   = 5;  // 风电系统
    const static int DEVICE_TYPE_DCS   = 6;  // 直流系统、储能系统

    // 实际设备
    const static int DEVICE_TYPE_DO            = 10; // DO，交流接触器
    const static int DEVICE_TYPE_DI            = 11; // DI
    const static int DEVICE_TYPE_ADE           = 12; // ADE77578
    const static int DEVICE_TYPE_WATT          = 13; // 智能电表
    const static int DEVICE_TYPE_PCS           = 14; // PCS
    const static int DEVICE_TYPE_PV            = 15; // PVS
    const static int DEVICE_TYPE_CB            = 16; // 智能汇流箱
    const static int DEVICE_TYPE_DCDC          = 17; // DCDC
    const static int DEVICE_TYPE_DC_LOAD       = 18; // 直流负载
    const static int DEVICE_TYPE_AC_LOAD       = 19; // 交流负载
    const static int DEVICE_TYPE_WT            = 20; // 风电变流器
    const static int DEVICE_TYPE_DC_PILE       = 22; // 直流充电桩
    const static int DEVICE_TYPE_AC_PILE       = 23; // 交流充电桩
    const static int DEVICE_TYPE_BMS           = 24; // BMS
    const static int DEVICE_TYPE_DG            = 25; // 柴油发电机
    const static int DEVICE_TYPE_SUPCAP        = 26; // 超级电容
    const static int DEVICE_TYPE_LIION         = 27; // 锂电池
    const static int DEVICE_TYPE_EVN           = 28; // 环境监测装置
    const static int DEVICE_TYPE_WTA           = 29; // 风电阵列
    const static int DEVICE_TYPE_PVA           = 30; // 光伏阵列
    const static int DEVICE_TYPE_APF           = 31; // APF/SVG无功补偿装置
    const static int DEVICE_TYPE_DO_OPT        = 32; // 交流电操
    const static int DEVICE_TYPE_DC_DO_OPT     = 33; // 直流电操
    const static int DEVICE_TYPE_DC_DO         = 34; // 直流接触器/继电器
    const static int DEVICE_TYPE_DC_DI         = 35; // 直流侧DI
    const static int DEVICE_TYPE_ANTI_REFLUX   = 36; // 防逆流采集箱
    const static int DEVICE_TYPE_POWER_DETEC   = 37; // 电能质量在线监测仪
    const static int DEVICE_TYPE_DI_MODULE     = 38; // DI模块
    const static int DEVICE_TYPE_SIGNAL_MODULE = 39; // 信号采集模块
    const static int DEVICE_TYPE_DO_MODULE     = 52; // DO模块
    const static int DEVICE_TYPE_CNCU          = 54; // 复合节点控制器 - Mingyu
    const static int DEVICE_TYPE_INSULATION_MONITOR = 55; // 绝缘监测仪 - Mingyu

    // 模拟设备
    const static int DEVICE_TYPE_SIMU_AC_GRID = 40; // 交流模拟电网
    const static int DEVICE_TYPE_SIMU_PV      = 41; // 光伏模拟器
    const static int DEVICE_TYPE_SIMU_RLC     = 42; // RLC负载
    const static int DEVICE_TYPE_SIMU_CABLE   = 43; // 电缆阻抗模拟器
    const static int DEVICE_TYPE_SIMU_DC_LOAD = 44; // 可编程直流负载
    const static int DEVICE_TYPE_SIMU_AC_LOAD = 45; // 可编程交流负载
    const static int DEVICE_TYPE_SIMU_WT      = 46; // 风机模拟器
    const static int DEVICE_TYPE_SIMU_DG      = 47; // 柴油发电机模拟器
    const static int DEVICE_TYPE_SIMU_DC      = 48; // 直流模拟电源
    const static int DEVICE_TYPE_SIMU_FLASH   = 49; // 谐波闪烁网络阻抗模拟器
    const static int DEVICE_TYPE_SINMU_LIB    = 50; // 电池模拟器
    const static int DEVICE_TYPE_SINMU_GZ     = 51; // 故障模拟器
    const static int DEVICE_TYPE_BIPOLAR_ACDC = 53; // 双极性AC/DC
    const static int DEVICE_TYPE_STS          = 56; // 晶石STS
    const static int DEVICE_TYPE_PVS_XV       = 57; // PVs
    const static int DEVICE_TYPE_CHARGE_PILE  = 58; // 充电桩
    const static int DEVICE_TYPE_XVJI         = 59; // 许继

    // 2025年2.8设备分块化
    const static int DEVICE_TYPE_FJ = 2000; // 15KW直驱风机
    const static int DEVICE_TYPE_CHROMA_GF_61845_61860 =
        4000;                                              // chroma功率放大器
    const static int DEVICE_TYPE_CHROMA_CN_62180D = 6000;  // chroma储能模拟器
    const static int DEVICE_TYPE_CHROMA_GF_62180D = 8000;  // chroma光伏模拟器
    const static int DEVICE_TYPE_WB_CN_DCDC       = 10000; // 王兵-储能dcdc
    const static int DEVICE_TYPE_WB_GF_DCDC       = 12000; // 王兵-光伏dcdc
    const static int DEVICE_TYPE_WB_CN_DCAC       = 14000; // 王兵-储能dcac
    const static int DEVICE_TYPE_GDW_GF_DCAC      = 16000; // 固德威光伏dcac
    const static int DEVICE_TYPE_WB_120KWPCS      = 18000; // 王兵-120kwpcs
    const static int DEVICE_TYPE_WB_15KWPCS       = 20000; // 王兵-15kw隔离模块
    const static int DEVICE_TYPE_HC201            = 22000; // 惠测HC201
    const static int DEVICE_TYPE_HC202            = 24000; // 惠测HC202
    const static int DEVICE_TYPE_HC221            = 26000; // 惠测HC221
    const static int DEVICE_TYPE_HC209            = 28000; // 惠测HC209
    const static int DEVICE_TYPE_HC33B            = 30000; // 惠测HC33B
    const static int DEVICE_TYPE_NGI_N35200       = 32000; // NGI光伏储能模拟器
    const static int DEVICE_TYPE_HJ_ACLOAD        = 34000; // 郝杰交流负载
    const static int DEVICE_TYPE_HJ_DCLOAD        = 36000; // 郝杰直流负载
};

/*名称：DevControllerType
 *描述：控制器类型常量，用于表示不同控制器类型
 *使用方法：
 *1、直接使用DevControllerType::SLAVE
 *创建人：mzh
 *创建日期：2013-3-5
 */
class DevControllerType {
  public:
    const static int CTRL_NULL = -1;
    const static int SLAVE     = 1; // 协调控制器
    const static int MASTER    = 2; // 集中控制器
    const static int Mult      = 3; // 组合控制器
};

/*名称：ConstCommonList
 *描述：通讯类型常量，用于表示不同通讯类型
 *使用方法：
 *1、直接使用ConstCommonList::COMM_RS485
 *创建人：mzh
 *创建日期：2013-3-5
 */
class ConstCommonList {
  public:
    const static int COMM_NULL  = 0; // 未知类型
    const static int COMM_RS485 = 1; // 485
    const static int COMM_TCP   = 2; // 以太网
    const static int COMM_CAN   = 3; // CAN
    const static int COMM_DO    = 4; // DO
    const static int COMM_DI    = 5; // DI
    const static int COMM_ADE   = 6; // 电能模块
    const static int COMM_OTHER = 7; // 未知类型
};

/*名称：DevManu
 *描述：设备生产厂商类型常量，用于表示不同通讯类型
 *使用方法：
 *1、直接使用ConstCommonList::COMM_RS485
 *创建人：mzh
 *创建日期：2013-3-5
 */
class DevManu {
  public:
    const static int Manu_NULL = 0; // 未知
    const static int Manu_HNZY = 1; // 北京汇能智远电力科技有限公司
    const static int Manu_STGK = 2; // 北京盛通高科新能源科技有限公司

    const static int Manu_Parwa = 3; // 深圳市菊水皇家科技有限公司
    const static int Manu_FGY   = 4; // 江苏峰谷源储能技术研究院有限公司
    const static int Manu_JZYG  = 5; // 锦州阳光气象科技有限公司
    const static int Manu_BOS   = 6; // 山东博奥斯能源科技有限公司

    const static int Manu_WASION  = 7;  // 威胜集团有限公司
    const static int Manu_ACREL   = 8;  // 安科瑞电器股份有限公司
    const static int Manu_GOODWE  = 9;  // 江苏固德威电源科技有限公司
    const static int Manu_HC      = 10; // 常州惠测电子有限公司
    const static int Manu_HUAYING = 11; // 宁波锦浪新能源科技有限公司
    const static int Manu_SUNGROW = 12; // 合肥阳光电源股份有限公司
    const static int Manu_HUAWEI  = 13; // 华为技术有限公司

    const static int Manu_KBT         = 14; // 湖南科比特新能源电器技术有限公司
    const static int Manu_BYD         = 15; // 比亚迪股份有限公司
    const static int Manu_VILVA       = 16; // 深圳市威尔华电子有限公司
    const static int Manu_SCHTITANS   = 17; // 四川泰坦豪特新能源汽车有限公司
    const static int Manu_CHROMA      = 18; // 致茂电子股份有限公司
    const static int Manu_SINEXCEL    = 19; // 深圳市盛弘电气有限公司
    const static int Manu_HZCONVERTER = 21; // 杭州禾迈电力电子技术有限公司
    const static int Manu_SHWENSHUN   = 22; // 上海文顺电器有限公司
    const static int Manu_APsytems    = 23; // APsystems
    const static int Manu_Zheda       = 24; // 浙江大学电池
    const static int Manu_ECM         = 26; // 北京艾科迈
    const static int Manu_QINGBIAO    = 27; // 青岛青表电器仪表有限公司
    const static int Manu_GREE        = 28; // 格力
    const static int Manu_TAIKAI      = 29; // 山东泰开电器机构有限公司
    const static int Manu_TSINGHUA    = 30; // 清华四川能源互联网
    const static int Manu_DEXN        = 31; // 北京德意新能科技有限公司 - Mingyu
    const static int Manu_JINGCE      = 32; // 北京京测科技有限公司- Mingyu
    const static int Manu_AINUO       = 33; // 山东艾诺仪器有限公司 - Mingyu
    const static int Manu_EA          = 34; // 德国EA Elektro-Automatik GmbH
    const static int Manu_BJGONGYUAN  = 35; // 北京共元科技有限公司
    const static int Manu_BENAN       = 36; // 贲安能源
    const static int Manu_DALIANHUAWU = 37; // 大连化学物理研究
    const static int Manu_SHENYANGWEIKONG = 38; // 沈阳微控
    const static int Manu_ZJNC            = 39; // 北京中京能测科技有限公司 郝杰
    const static int Manu_HUAGONGWULI     = 40; // 中科院 化工物理所 邵家云
    const static int Manu_HAIBOSICHUANG   = 41; // 固态锂离子，海博思创
    const static int Manu_JINGSHI     = 42; //   西安晶石，泰开项目使用的DCDC
    const static int Manu_XVJI        = 43; // 许继
    const static int Manu_XVJI_AcF    = 44; // 许继AcFaultSimulator
    const static int Manu_XVJI_Rlc    = 45; // 许继RlcLoad
    const static int Manu_XVJI_LISP   = 46; // 许继线路阻抗模拟器
    const static int Manu_XVJI_N35200 = 47; // 许继N35200
    const static int Manu_XVJI_CPC    = 48; // 许继ChargePileControl
    const static int Manu_AINUO_vh    = 49; // 艾诺储能vh
};

/*名称：DevModel
 *描述：设备型号类型常量
 *使用方法：
 *创建人：LiuHaitao
 *创建日期：2014-9-16
 */
class DevModel {
  public:
    const static int Model_NULL          = 0;  // 未知
    const static int Model_HN_WindM_05D  = 1;  // 北京汇能智远5kW直驱风机模拟器
    const static int Model_HN_Lib_15     = 2;  // 江苏峰谷源15kWh锂电池
    const static int Model_HN_DCDC_15    = 3;  // 北京汇能智远15kWDC/DC
    const static int Model_PC_4          = 4;  // 锦州阳光环境监测仪
    const static int Model_PV_RLC_30     = 5;  // 菊水皇家30KW可编程交流负载
    const static int Model_DTSD341       = 6;  // 威盛智能双向计量表
    const static int Model_APMD730       = 7;  // 安科瑞电能质量分析装置
    const static int Model_WIND_05D      = 8;  // 锦浪5kW直驱风机
    const static int Model_HN_PCS_15     = 9;  // 博奥斯15kW储能逆变器
    const static int Model_GD5KW_DT      = 10; // 江苏固德威5KW光伏逆变器
    const static int Model_PVS1000       = 11; // 菊水皇家10kW光伏模拟器
    const static int Model_HC_209        = 12; // 惠测电子DIDO模块
    const static int Model_HC_33B        = 13; // 惠测电子电能模块
    const static int Model_HC_33BD       = 14; // 惠测电子直流电能模块
    const static int Model_HN_DSLM_30    = 15; // 北京汇能智远30kW柴油模拟器
    const static int Model_HN_Wind_05S   = 16; // 北京汇能智远5kW双馈风机模拟器
    const static int Model_M4FTRA_KV_T2  = 17; // 安科瑞汇流箱
    const static int Model_VILVA_30K     = 18; // 深圳市威尔华30kW负载
    const static int Model_VILVA_10K     = 19; // 深圳市威尔华10kW负载
    const static int Model_TCZ_J         = 20; // 四川泰坦豪特充电桩
    const static int Model_CHROMA_62180D = 74; // chroma62180D 储能模拟器
    const static int Model_CHROMA_61860  = 21; // chroma60kW电网模拟器
    const static int Model_CHROMA_62150H_1000S = 22; // chroma15kW光伏模拟器
    const static int Model_CHROMA_17040        = 23; // chroma60kW电池模拟器
    const static int Model_CHROMA_62020H_150S = 24; // chroma2kW太阳能电池模拟器
    const static int Model_CHROMA_63804       = 25; // chroma4.5kW交流电子负载
    const static int Model_CHROMA_63204A_150_400 = 26; // chroma直流电子负载
    const static int Model_SPC030KTL           = 27; // 禾迈30kW组串型光伏逆变器
    const static int Model_WSTF_LDJ_RLC        = 28; // 上海文顺RLC交流负载
    const static int Model_SINEXCEL_SEV        = 29; // 深圳盛弘充电桩
    const static int Model_SINEXCEL_APF        = 30; // 深圳盛弘APF/SVG
    const static int Model_SINEXCEL_PCS        = 31; // 深圳盛弘PCS
    const static int Model_WSTF_LDJ_ISLAND_RLC = 32; // 上海文顺孤岛交流负载
    const static int Model_HC_201              = 33; // 惠测电子开关量输入模块
    const static int Model_APSYSTEMS_PVS       = 34; // APsystems微型逆变器
    const static int Model_PARWA_XLZK          = 35; // 线缆阻抗
    const static int Model_PARWA_GZMN          = 36; // 故障模拟器
    const static int Model_ZHEDA_BMS           = 37; // 浙大电池
    const static int Model_HC_202              = 38; // 惠测电子开关量输出模块
    const static int Model_HC_221              = 39; // 惠测电子模拟量测量模块
    const static int Model_HC_63C              = 40; // 惠测三相多功能电表
    const static int Model_APM_810             = 41; // 安科瑞APM810网络电力仪表
    const static int Model_PV_DCDC             = 42; // 光伏DC/DC
    const static int Model_BATT_DCDC           = 43; // 储能DC/DC
    const static int Model_ECM_DCAC            = 44; // DC/AC
    const static int Model_TK900M     = 45; // 2022年泰开项目电表 交流电表
    const static int Model_KONGTIAO   = 46; // 格力空调外机
    const static int Model_GREEBATT   = 47; // 格力储能
    const static int Model_F05012     = 48; // 山东泰开相变储热
    const static int Model_ADG_B40075 = 49; // 双极性AC/DC

    const static int Model_CNCU_ACDC = 50; // ACDC 复合节点控制器 -Mingyu
    const static int Model_CNCU_ESC  = 51; // 储能DCDC复合控制器 - Mingyu
    const static int Model_CNCU_PVC  = 52; // 光伏DCDC复合控制器 - Mingyu
    const static int Model_CNCU_CNCU = 53; // 上级复合控制器 - Mingyu
    const static int Model_DCXG_500  = 54; // 京测直流线路阻抗 - Mingyu
    const static int Model_ACLT_AN51 = 55; // ACLT-AN51; 艾诺光伏模拟器 - Mingyu
    const static int Model_AN51010 = 56; // ACLT-AN5010; 艾诺光伏模拟器 - Mingyu

    const static int Model_ELR_9500_60    = 57; // EA ELR9500_60 负载 - Mingyu
    const static int Model_DYDCG          = 58; // GYDCG-B02-DDRH - Mingyu
    const static int Model_BMS_AQUANA     = 59; // 水系钠离子BMS
    const static int Model_BMS_FLYWHEEL   = 60; // 飞轮储能
    const static int Model_LOAD_AAC3810   = 61; // RLC负载
    const static int Model_LOAD_VANA      = 62; // 全钒液流储能
    const static int Model_ACREL_AEM96    = 63; // 安科瑞AEM96
    const static int Model_ACREL_APM      = 64; // 安科瑞APM
    const static int Model_JINGSHI_DCDC   = 65; //  晶石DCDC 泰开项目使用
    const static int Model_ACREL_DJSF1352 = 66; // 泰开项目使用电表
    const static int Model_ACREL_AIM_T500 = 67; // 三峡项目绝缘检测仪
    const static int Model_ACREL_AIM_T350 = 68; // 泰开项目绝缘检测仪
    const static int Model_JDHN_ANGridSim = 69; // 京东汇能电网模拟器

    const static int Model_ECM_HFI_DCAC     = 70; // 2000W电源
    const static int Model_RLC_XVJI         = 71; // 许继RLC负载
    const static int Model_ECM_DCAC_kaifang = 72; // 长沙DCAC
    const static int Model_ACLT_anevh       = 73; // 长沙模拟器

    const static int RLC_A    = 74; // RLC负载A
    const static int RLC_B    = 75; // RLC负载B 天津
    const static int RLC_C    = 76; // RLC负载C 北京移动
    const static int DCLOAD_A = 77; // 直流负载A
    const static int DCLOAD_B = 78; // 直流负载B北京移动
};

/*名称：Device
 *描述：微网系统设备
 *使用方法：
 *1、得到实例并初始化
 *Device dev=Device::GetInstance();
 *2、可以使用该类的方法
 *PtrArray* array=dev->GetSlaveDevArray();
 *创建人：mzh
 *创建日期：2013-3-5
 */
class Device {
  public:
    // 串口通讯参数
    typedef struct ComDev {
        int port;     // 串口号
        int baudrate; // 波特率
        char parity;  // 校验位,'N'无校验/'O'奇校验/'E'偶校验
        int databits; // 数据位
        int stopbits; // 停止位
    } ComDev;

    // CAN口通讯参数
    typedef struct CanDev {
        int port;     // CAN口号
        int baudrate; // 波特率
    } CanDev;

    // 设备类型
    typedef struct DevType {
        int sn;           // 类型编号
        string name;      // 类型名称
        int level;        // 类型等级
        int sysTypeLevel; // 系统类型等级
        int devGroup;     // 二级设备
        string desc;      // 类型描述
    } DevType;

    // 通讯类型
    typedef struct ConstCommType {
        int type;     // 协议编号
        string name;  // 协议名称
        int commType; // 通讯类型
        string desc;  // 描述
    } ConstCommType;

    // 从设备参数
    typedef struct SlaveDev {
        int sn;           // 设备编号
        string name;      // 设备名称
        int devType;      // 设备类型
        int devManu;      // 设备制造商
        int devModel;     // 设备型号
        int protocol;     // 协议
        int commType;     // 通讯类型
        int commNo;       // 通讯号，串口1等
        string ip;        // IP地址
        int port;         // 端口
        int regStart;     // 寄存器起始地址
        int regLen;       // 长度
        string slaveAddr; // 设备地址
        int rate;         // 额定功率
        int sysType;      // 系统类型
        int sysTypeNo;    // 系统编号
        int subType1;     // 子系统类型
        int subType1No;   // 子系统编号
        int subType2;     // 三级系统类型
        int subType2No;   // 三级系统编号
        int group;        // 属于组
        int flag;         // 标志位，断路器是直流侧断路器还是交流侧断路器
        int own;     // 0/1，true从设备属于该设备，为false表示属于下面协调控制器
        bool enable; // 该设备是否使能，即是否属于可控设备
        string desc; // 设备描述

        int start; // 保存到的数据库起始地址
        int len;   // 保存到的数据库长度
    } SlaveDev;

    // 方法
  public:
    /*
     *  GetInstance
     *  功能:得到实例指针
     *  输入：无
     *  输出：返回Device*指针，
     */
    static Device *GetInstance();

    /*
     *  FreeInstanse
     *  功能:关闭类指针，释放内存
     *  输入：无
     *  输出：无
     */
    void FreeInstanse();

    /*
     * 名称: Init
     * 功能: 初始化配置
     * 输出: 成功/失败
     */
    bool Init();

    /*
     * 名称: Uninit
     * 功能: 反初始化配置
     * 输出: 无
     */
    void Uninit();

    /*
     * 名称: GetControllerType
     * 功能: 返回本控制器类型
     * 输出: int 控制器类型：协调控制器、集中控制器或组合控制器
     */
    int GetControllerType();

    /*
     * 名称: GetControllerAddr
     * 功能: 返回本控制器地址
     * 输出: int
     */
    int GetControllerAddr();

    /*
     * 名称: GetSlaveDevArray
     * 功能: 返回从设备数组指针
     * 输出: PtrArray* 从设备数组指针SlaveDev*
     */
    PtrArray *GetSlaveDevArray();

    /*
     * 名称: GetComDevArray
     * 功能: 返回从设备数组指针
     * 输出: PtrArray* 从设备数组指针ComDev*
     */
    PtrArray *GetComDevArray();
    /*
     * 名称: GetCanDevArray
     * 功能: 返回从设备数组指针
     * 输出: PtrArray* 从设备数组指针CanDev*
     */
    PtrArray *GetCanDevArray();

    /*
     * 名称: GetConstCommTypeArray
     * 功能: 返回通讯类型数组指针
     * 输出: PtrArray* 从设备数组指针ConstCommType*
     */
    PtrArray *GetConstCommTypeArray();

    /*
     * 名称: GetMemAddr
     * 功能: 读取设备保存在内存数据库中的首地址及长度
     * 输入:
     * start          :[out]int& 起始地址
     * len            :[out]int& 长度
     * sysType        :[in]int 系统类型
     * sysTypeNo      :[in]int 系统编号，从1开始
     * subType1       :[in]int 子类型1，如果是查询系统时为0
     * subType1No     :[in]int 子类型1编号，从1开始，如果是查询系统时为0
     * subType2       :[in]int 子类型2，如果是查询系统时为0
     * subType2No     :[in]int 子类型2编号，从1开始，如果是查询系统时为0
     * 输出: 返回设备首地址
     */
    static void GetMemAddr(int &start, int &len, int sysType, int sysTypeNo,
                           int subType1, int subType1No, int subType2,
                           int subType2No);

  protected:
    Device();
    virtual ~Device();

  private:
    /*
     * 名称：InitDevType
     * 功能：读取设备类型
     * 输出：成功/失败
     */
    bool InitDevType();

    /*
     * 名称：InitCommList
     * 功能：读取设备通讯类型
     * 输出：成功/失败
     */
    bool InitCommList();

    /*
     * 名称: InitSlaveDevConfig
     * 功能: 读取设备参数配置,
     * 输出: 成功/失败
     */
    bool InitSlaveDevConfig();

    /*
     * 名称: InitMemDb
     * 功能: 初始化设备设置量地址
     * 输出: 成功/失败
     */
    bool InitMemDb();

    /*
     * 名称: GetType1Addr
     * 功能: 读取设备首地址
     * 输入:sys_type:[in]char 系统类型
     *	   sub_type_1:[in]int 子类型1
     *	   sub_type_2:[in]int 子类型2
     * 输出: 返回设备子类型1首地址
     */
    static void GetType1Addr(int &start, int &len, int sysType, int sysTypeNo,
                             int subType1, int subType1No, int subType2,
                             int subType2No);

    /*
     * 名称: GetType2Addr
     * 功能: 读取设备首地址
     * 输入:sys_type:[in]char 系统类型
     *	   sub_type_1:[in]int 子类型1
     *	   sub_type_2:[in]int 子类型2
     * 输出: 返回设备子类型2首地址
     */
    static void GetType2Addr(int &start, int &len, int sysType, int sysTypeNo,
                             int subType1, int subType1No, int subType2,
                             int subType2No);

    // 变量
  public:
    static DevType m_devType[128]; // 设备类型数组
    static int m_devTypeNum;       // 设备类型数量
  protected:
  private:
    PtrArray m_comDevArray;        // 串口数组
    PtrArray m_canDevArray;        // CAN接口数组
    PtrArray m_slaveDevArray;      // 从设备数组
    PtrArray m_constCommTypeArray; // 通讯类型

    int m_controllerType; // 本控制器类型
    int m_controllerAddr; // 本控制器地址

    static Device *m_dev;
};

#endif /* DEVICE_H_ */
