#include "ZinxTimer.h"
#include <sys/timerfd.h>

bool ZinxTimerChannel::Init()
{
    /*������ʱ���ļ�������*/

    bool bRet = false;
    /*�����ļ�������*/
    int iFd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (0 <= iFd)
    {
        /*���ö�ʱ����*/
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

/*��ȡ��ʱ����*/
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

/*�ر��ļ�������*/
void ZinxTimerChannel::Fini()
{
    close(m_Timerfd);
    m_Timerfd = -1;
}

/*����*/
int ZinxTimerChannel::GetFd()
{
    return m_Timerfd;
}

std::string ZinxTimerChannel::GetChannelInfo()
{
    return "Timerfd";
}


class output_hello : public AZinxHandler {
    // ͨ�� AZinxHandler �̳�
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

/*���ش���ʱʱ��Ķ���*/
AZinxHandler* ZinxTimerChannel::GetInputNextStage(BytesMsg& _oInput)
{
    return &TimerOutMng::GetInstance();
}

/*------------------------------*/
TimerOutMng TimerOutMng::single;

TimerOutMng::TimerOutMng()
{
    /*����ʮ����*/
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
        /*�ƶ��̶�*/
        cur_index++;
        cur_index %= 10;
        std::list<TimerOutProc*> m_cache;
        /*������ǰ�̶����нڵ㣬ָ��������Ȧ��-1*/
        for (auto itr = m_timer_wheel[cur_index].begin(); itr != m_timer_wheel[cur_index].end();)
        {
            /*���Ȧ��<=0 -->ִ�г�ʱ����*/
            if ((*itr)->iCount <= 0)
            {
                /*���������ĳ�ʱ�ڵ�*/
                m_cache.push_back(*itr);
                /*��taskժ��*/
                auto ptmp = *itr;
                itr = m_timer_wheel[cur_index].erase(itr);
                AddTask(ptmp);
            }
            /*else Ȧ��--*/
            else
            {
                (*itr)->iCount--;
                ++itr;
            }
        }

        /*ͬ�������ʱ����*/
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
    /*���㵱ǰ����Ҫ���ĸ�����*/
    int index = (_ptask->GetTimeSec() + cur_index) % 10;
    /*������浽�ó���*/
    m_timer_wheel[index].push_back(_ptask);
    /*��������Ȧ��*/
    _ptask->iCount = _ptask->GetTimeSec() / 10;
}

void TimerOutMng::DelTask(TimerOutProc* _ptask)
{
    /*����ʱ�������гݣ�ɾ������*/
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
