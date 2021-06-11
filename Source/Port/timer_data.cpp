#include "StdAfx.h"
#include "statistics.h"

AllocationStatistics AllocationStatistics::dbg_hook_data = AllocationStatistics();

TimerData::TimerData(char* title_ /*= 0*/, int group_ /*= 0*/, int dont_attach /*= 0*/)
    : title(title_)
    , group(group_)
{
}

void TimerData::clear()
{

}

void TimerData::print(XBuffer& buf)
{

}