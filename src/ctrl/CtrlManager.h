/*
 * CtrlManager.h
 *
 *   创建日期: 2014-1-10
 *   作          者: 马中华
 */

#ifndef CTRLMANAGER_H_
#define CTRLMANAGER_H_

/*名称:CtrlManager
 *描述:控制管理接口
 *用法:
 *作者:马中华
 *创建日期:2014-1-10
 */
class CtrlManager {
    //方法
  public:
    CtrlManager();
    virtual ~CtrlManager();

    /*
     * 名称: Init
     * 功能: 初始化控制策略
     * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERROR_OK
     */
    int Init();

    /*
     * 名称: Uninit
     * 功能: 关闭控制策略
     * 输出: 无
     */
    void Uninit();

  protected:
  private:
    //变量
  public:
  protected:
  private:
};

#endif /* CTRLMANAGER_H_ */
