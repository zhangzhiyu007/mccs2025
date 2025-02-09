/*
 * FirstOrderLagFilter.h
 *   Copyright (c) 2012-2014,北京中电建投微电网科技有限公司
 *   All rights reserved.
 *   创建日期: 2014-3-13
 *   作          者: 马中华
 *
 *   版          本：1.1
 *   作          者：
 *   修改内容:
 */

#ifndef FIRSTORDERLAGFILTER_H_
#define FIRSTORDERLAGFILTER_H_

/*名称:FirstOrderLagFilter
 *描述:一阶滞后滤波，y'=(T-1)/T*x+1/T*y
 *用法:
 *1、设置滤波系数
 *FirstOrderLagFilter filter;
 *filter. SetT(10);
 *2、滤波计算
 *y=filter. Filter(x);
 *创建人:马中华
 *创建日期:2014-3-13
 */
class FirstOrderLagFilter
{
public:
	FirstOrderLagFilter();
	virtual ~FirstOrderLagFilter();

	/*
	 *  SetT
	 *  功能: 设定滤波系数
	 *  输入：
	 *  T   :[in]int 滤波系数,T必须》1
	 *  输出：无
	 */
	void SetT(int T)
	{
		m_T = T;
	}

	/*
	 *  Filter
	 *  功能: 滤波计算
	 *  输入：
	 *  x   :[in]float待滤波值
	 *  输出：float 滤波结果
	 */
	float Filter(float x);

	/*
	 *  GetY
	 *  功能: 返回滤波结果
	 *  输入：无
	 *  输出：返回滤波结果
	 */
	float GetY();

protected:
private:
	int m_T;//滤波系数
	float m_y;//滤波结果

};

#endif /* FIRSTORDERLAGFILTER_H_ */
