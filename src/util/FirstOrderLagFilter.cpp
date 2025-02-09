/*
 * FirstOrderLagFilter.cpp
 *   Copyright (c) 2012-2014,北京中电建投微电网科技有限公司
 *   All rights reserved.
 *   创建日期: 2014-3-13
 *   作          者: 马中华
 *
 *   版          本：
 *   作          者：
 *   修改内容:
 */

#include "FirstOrderLagFilter.h"

FirstOrderLagFilter::FirstOrderLagFilter()
{
	// TODO 构造函数
	m_T = 1;//功率平滑周期s
	m_y = 0.0;//功率平滑数据

}

FirstOrderLagFilter::~FirstOrderLagFilter()
{
	// TODO 析构函数
}


float FirstOrderLagFilter::Filter(float x)
{
	float tmp = 0.0;
	tmp = m_y * (m_T - 1) / m_T;
	tmp += x / m_T;
	m_y = tmp;
	return m_y;
}
float FirstOrderLagFilter::GetY()
{
	return m_y;
}
