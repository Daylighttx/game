#include "ZinxTimer.h"
#include <sys/timerfd.h>

bool ZinxTimerChannel::Init()
{
    /*创建定时器文件描述符*/

    bool bRet = false;
    /*创建文件描述符*/
    int iFd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (0 <= iFd)
    {
        /*设置定时周期*/
        struct itimerspec period = {
            {1, 0}, {1, 0}
        };
        if (0 == timerfd_settime(iFd, 0, &period, NULL))
        {
            bRet = true;
            m_Timerfd = iFd;
        }
    }
    
    return bRet;
}

/*读取超时次数*/
bool ZinxTimerChannel::ReadFd(std::string& _input)
{
    bool bRet = false;
    char buff[8] = { 0 };

    if (sizeof(buff) == read(m_Timerfd, buff, sizeof(buff)))
    {
        bRet = true;
        _input.assign(buff, sizeof(buff));
    }
    return bRet;
}

bool ZinxTimerChannel::WriteFd(std::string& _output)
{
    return false;
}

/*关闭文件描述符*/
void ZinxTimerChannel::Fini()
{
    close(m_Timerfd);
    m_Timerfd = -1;
}

/*返回*/
int ZinxTimerChannel::GetFd()
{
    return m_Timerfd;
}

std::string ZinxTimerChannel::GetChannelInfo()
{
    return "Timerfd";
}


class output_hello : public AZinxHandler {
    // 通过 AZinxHandler 继承
    IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
    {
        auto pchannel = ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
        std::string output = "Hello World";
        ZinxKernel::Zinx_SendOut(output, *pchannel);
        return nullptr;
    }
    AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
    {
        return nullptr;
    }
} *pout_hello = new output_hello();

/*返回处理超时时间的对象*/
AZinxHandler* ZinxTimerChannel::GetInputNextStage(BytesMsg& _oInput)
{
    return &TimerOutMng::GetInstance();
}

/*------------------------------*/
TimerOutMng TimerOutMng::single;

TimerOutMng::TimerOutMng()
{
    /*创建十个齿*/
    for (int i = 0; i < 10; i++)
    {
        std::list<TimerOutProc*> tmp;
        m_timer_wheel.push_back(tmp);
    }

}

IZinxMsg* TimerOutMng::InternelHandle(IZinxMsg& _oInput)
{

    unsigned long iTimeoutCount = 0;
    GET_REF2DATA(BytesMsg, obytes, _oInput);
    obytes.szData.copy((char *) & iTimeoutCount, sizeof(iTimeoutCount), 0);


    while (iTimeoutCount-- > 0)
    {
        /*移动刻度*/
        cur_index++;
        cur_index %= 10;
        std::list<TimerOutProc*> m_cache;
        /*遍历当前刻度所有节点，指向处理函数或圈数-1*/
        for (auto itr = m_timer_wheel[cur_index].begin(); itr != m_timer_wheel[cur_index].end();)
        {
            /*如果圈数<=0 -->执行超时函数*/
            if ((*itr)->iCount <= 0)
            {
                /*缓存待处理的超时节点*/
                m_cache.push_back(*itr);
                /*将task摘除*/
                auto ptmp = *itr;
                itr = m_timer_wheel[cur_index].erase(itr);
                AddTask(ptmp);
            }
            /*else 圈数--*/
            else
            {
                (*itr)->iCount--;
                ++itr;
            }
        }

        /*同意待处理超时任务*/
        for (auto task : m_cache)
        {
            task->Proc();
        }
    }

    return nullptr;
}

AZinxHandler* TimerOutMng::GetNextHandler(IZinxMsg& _oNextMsg)
{
    return nullptr;
}

void TimerOutMng::AddTask(TimerOutProc* _ptask)
{
    /*计算当前任务要放哪个齿上*/
    int index = (_ptask->GetTimeSec() + cur_index) % 10;
    /*把任务存到该齿上*/
    m_timer_wheel[index].push_back(_ptask);
    /*计算所需圈数*/
    _ptask->iCount = _ptask->GetTimeSec() / 10;
}

void TimerOutMng::DelTask(TimerOutProc* _ptask)
{
    /*遍历时间轮所有齿，删掉任务*/
    for (auto &cog : m_timer_wheel)
    {
        for (auto task : cog)
        {
            if (task == _ptask)
            {
                cog.remove(_ptask);
                return;
            }
        }
    }
    
}
