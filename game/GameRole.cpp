#include "GameRole.h"
#include "GameMsg.h"
#include "AOIWorld.h"
#include "msg.pb.h"
#include <iostream>
#include "GameProtocol.h"
#include "GameChannel.h"
#include <algorithm>
#include <random>
#include "ZinxTimer.h"
#include "RandomName.h"
#include <fstream>
#include <hiredis/hiredis.h>

/*�������������ȫ�ֶ���*/
RandomName random_name;

/*������Ϸ����ȫ�ֶ���*/
static AOIWorld world(0, 400, 0, 400, 20, 20);

class ExitTimer :public TimerOutProc
{
    // ͨ�� TimerOutProc �̳�
    void Proc() override
    {
        ZinxKernel::Zinx_Exit();
    }
    int GetTimeSec() override
    {
        return 20;
    }
};

static ExitTimer g_exit_timer;

GameMsg* GameRole::CreateIDNameLogin()
{
    pb::SyncPid* pmsg = new pb::SyncPid();
    pmsg->set_pid(iPid);
    pmsg->set_username(szName);
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pmsg);
    return pRet;
}

GameMsg* GameRole::CreateSrdPlayers()
{
    pb::SyncPlayers* pMsg = new pb::SyncPlayers();
    auto srd_list = world.GetSrdPlayers(this);

    for (auto single : srd_list)
    {
        auto pPlayer = pMsg->add_ps();
        auto pRole = dynamic_cast<GameRole*>(single);
        pPlayer->set_pid(pRole->iPid);
        pPlayer->set_username(pRole->szName);
        auto pPosition = pPlayer->mutable_p();
        pPosition->set_x(pRole->x);
        pPosition->set_y(pRole->y);
        pPosition->set_z(pRole->z);
        pPosition->set_v(pRole->v);
    }
    GameMsg* pret = new GameMsg(GameMsg::MSG_TYPE_SRD_POSITION, pMsg);
    return pret;
}

GameMsg* GameRole::CreateSelfPosition()
{
    pb::BroadCast* pMsg = new pb::BroadCast();
    pMsg->set_pid(iPid);
    pMsg->set_username(szName);
    //��ʾ������ϢΪ��ʼλ��
    pMsg->set_tp(2);

    auto pPosition = pMsg->mutable_p();
    pPosition->set_x(x);
    pPosition->set_y(y);
    pPosition->set_z(z);
    pPosition->set_v(v);


    GameMsg* pret = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg);
    return pret;
}

GameMsg* GameRole::CreateIDNameLogoff()
{
    pb::SyncPid* pmsg = new pb::SyncPid();
    pmsg->set_pid(iPid);
    pmsg->set_username(szName);
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGOFF_ID_NAME, pmsg);
    return pRet;
}

GameMsg* GameRole::CreateTalkBroadCast(std::string _content)
{
    pb::BroadCast* pmsg = new pb::BroadCast();
    pmsg->set_pid(iPid);
    pmsg->set_username(szName);
    pmsg->set_tp(1);
    pmsg->set_content(_content);
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pmsg);
    return pRet;
}

void GameRole::ProcTalkMsg(std::string _content)
{
    /*ȡ����������*/
    auto content = _content;
    //����������
    auto role_list = ZinxKernel::Zinx_GetAllRole();
    for (auto pRole : role_list)
    {
        auto pGameRole = dynamic_cast<GameRole*>(pRole);
        auto pmsg = CreateTalkBroadCast(content);
        ZinxKernel::Zinx_SendOut(*pmsg, *(pGameRole->m_pProto));
    }
}

void GameRole::ProcMoveMsg(float _x, float _y, float _z, float _v)
{
    /*1.��������*/

    /*��ȡԭ�����ھ�s1*/
    auto s1 = world.GetSrdPlayers(this);
    /*ժ���ɸ��ӣ��������꣬����¸��ӻ�ȡ���ھ�s2*/
    world.DelPlayer(this);
    x = _x;
    y = _y;
    z = _z;
    v = _v;
    world.AddPlayer(this);
    auto s2 = world.GetSrdPlayers(this);
    /*����s2,��Ԫ�ز�����s1����Ұ����*/
    for (auto single_player : s2)
    {
        if (s1.end() == find(s1.begin(), s1.end(), single_player))
        {
            //��Ұ����
            ViewAppear(dynamic_cast<GameRole*>(single_player));
        }
    }
    /*����s1����Ԫ�ز�����s2����Ұ��ʧ*/
    for (auto single_player : s1)
    {
        if (s2.end() == find(s2.begin(), s2.end(), single_player))
        {
            //��Ұ��ʧ
            ViewLost(dynamic_cast<GameRole*>(single_player));
        }
    }

    /*2.�㲥��λ�ø���Χ���*/

    /*ȡ����λ��*/
    /*������Χ��ҷ���*/
    auto srd_list = world.GetSrdPlayers(this);
    for (auto single : srd_list)
    {
        /*��ɴ����͵ı���*/
        pb::BroadCast* pMsg = new pb::BroadCast();
        auto pPos = pMsg->mutable_p();
        pPos->set_x(_x);
        pPos->set_y(_y);
        pPos->set_z(_z);
        pPos->set_v(_v);
        pMsg->set_pid(iPid);
        pMsg->set_tp(4);
        pMsg->set_username(szName);
        auto pRole = dynamic_cast<GameRole*>(single);
        ZinxKernel::Zinx_SendOut(*(new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg)), *(pRole->m_pProto));
    }
}

void GameRole::ViewAppear(GameRole* _pRole)
{
    /*���Լ���200��Ϣ*/
    auto pmsg = _pRole->CreateSelfPosition();
    ZinxKernel::Zinx_SendOut(*pmsg, *m_pProto);
    /*�������ҷ��Լ���200��Ϣ*/
    pmsg = CreateSelfPosition();
    ZinxKernel::Zinx_SendOut(*pmsg, *(_pRole->m_pProto));
}

void GameRole::ViewLost(GameRole* _pRole)
{
    /*���Լ����Ͳ�����ҵ�201��Ϣ*/
    auto pmsg = _pRole->CreateIDNameLogoff();
    ZinxKernel::Zinx_SendOut(*pmsg, *m_pProto);
    /*�������ҷ��Լ���201��Ϣ*/
    pmsg = CreateIDNameLogoff();
    ZinxKernel::Zinx_SendOut(*pmsg, *(_pRole->m_pProto));
}

static std::default_random_engine random_engine(time(NULL));

GameRole::GameRole()
{
    x = 100 + random_engine() % 50;
    z = 100 + random_engine() % 50;
    szName = random_name.GetName();
}

GameRole::~GameRole()
{
    random_name.Release(szName);
}

bool GameRole::Init()
{
    if (ZinxKernel::Zinx_GetAllRole().size() <= 0)
    {
        TimerOutMng::GetInstance().DelTask(&g_exit_timer);
    }
    /*����Լ�����Ϸ����*/
    bool bRet = false;
    /*�������IDΪ��ǰ����fd*/
    iPid = m_pProto->m_channel->GetFd();
    bRet = world.AddPlayer(this);

    /*���Լ�����ID������*/
    if (true == bRet)
    {
        auto pmsg = CreateIDNameLogin();
        ZinxKernel::Zinx_SendOut(*pmsg, *m_pProto);
        /*���Լ�������Χ��ҵ�λ��*/
        pmsg = CreateSrdPlayers();
        ZinxKernel::Zinx_SendOut(*pmsg, *m_pProto);
        /*����Χ��ҷ�����λ��*/
        auto srd_list = world.GetSrdPlayers(this);
        for (auto single : srd_list)
        {
            pmsg = CreateSelfPosition();
            auto pRole = dynamic_cast<GameRole*>(single);
            ZinxKernel::Zinx_SendOut(*pmsg, *(pRole->m_pProto));
        }
    }

    /*��׷����ʽ����¼��ǰ�������ļ�*/
    /*std::ofstream name_record("/tmp/name_record",std::ios::app);
    name_record << szName << std::endl;*/

    /*��¼��ǰ������redis��game_name*/
    /*1. ����redis*/
    auto context = redisConnect("127.0.0.1", 6379);
    /*2. ����lpush����*/
    if (NULL != context)
    {
        freeReplyObject(redisCommand(context, "lpush game_name %s", szName.c_str()));
        redisFree(context);
    }



    return bRet;
}




/*������Ϸ��ص��û�����*/
UserData* GameRole::ProcMsg(UserData& _poUserData)
{
    /*test��print msg content*/
    GET_REF2DATA(MultiMsg, input, _poUserData);
    for (auto single : input.m_Msgs)
    {
        std::cout << "type: " << single->enMsgType << std::endl;
        std::cout << single->pMsg->Utf8DebugString() << std::endl;

        auto NewPos = dynamic_cast<pb::Position*>(single->pMsg);
        switch (single->enMsgType)
        {
        case (GameMsg::MSG_TYPE_CHAT_CONTENT):
            ProcTalkMsg(dynamic_cast<pb::Talk*>(single->pMsg)->content());
            break;
        case(GameMsg::MSG_TYPE_NEW_POSITION):
            ProcMoveMsg(NewPos->x(), NewPos->y(), NewPos->z(), NewPos->v());
            break;
        default:
            break;
        }
    }
    return nullptr;
}



void GameRole::Fini()
{
    /*����Χ��ҷ���������Ϣ*/
    auto srd_list = world.GetSrdPlayers(this);
    for (auto single : srd_list)
    {
        auto pMsg = CreateIDNameLogoff();
        auto pRole = dynamic_cast<GameRole*>(single);
        ZinxKernel::Zinx_SendOut(*pMsg, *(pRole->m_pProto));
    }

    world.DelPlayer(this);

    /*�ж��Ƿ������һ�����--->��ʱ��*/
    if (ZinxKernel::Zinx_GetAllRole().size() <= 1)
    {
        //���˳���ʱ��
        TimerOutMng::GetInstance().AddTask(&g_exit_timer);
    }

    /*��redis game_name��ɾ����ǰ����*/
    auto context = redisConnect("127.0.0.1", 6379);
    if (NULL != context)
    {
         freeReplyObject(redisCommand(context, "lrem game_name 1 %s", szName.c_str()));
         redisFree(context);
    }

    /*���ļ���ɾ������*/
    /*1 ���ļ��ж�����������*/
    //std::list<std::string> cur_name_list;
    //std::ifstream input_stream("/tmp/name_record");
    //std::string tmp;
    //while (getline(input_stream, tmp))
    //{
    //    cur_name_list.push_back(tmp);
    //}
    ///*2 ɾ����ǰ����*/
    ///*3 д����������*/
    //std::ofstream output_stream("/tmp/name_record");
    //for (auto name : cur_name_list)
    //{
    //    if (name != szName)
    //    {
    //        output_stream << name << std::endl;
    //    }
    //}
}

int GameRole::GetX()
{
    return (int)x;
}

int GameRole::GetY()
{
    return (int)z;
}
