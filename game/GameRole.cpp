#include "GameRole.h"
#include "GameMsg.h"
#include "AOIWorld.h"
#include "msg.pb.h"
#include <iostream>
#include "GameProtocol.h"
#include "GameChannel.h"

/*创建游戏世界全局对象*/
static AOIWorld world(0, 400, 0, 400, 20, 20);



GameMsg* GameRole::CreateIDNameLogin()
{
    pb::SyncPid* pmsg = new pb::SyncPid();
    pmsg->set_pid(iPid);
    pmsg->set_username(szName);
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pmsg);
    return nullptr;
}

GameRole::GameRole()
{
    
    szName = "Tom";
}

GameRole::~GameRole()
{
}

bool GameRole::Init()
{
    /*添加自己到游戏世界*/
    bool bRet = false;
    /*设置玩家ID为当前连接fd*/
    iPid = m_pProto->m_channel->GetFd();
    bRet = world.AddPlayer(this);

    /*向自己发送ID和名称*/
    if (true == bRet)
    {
        auto pmsg = CreateIDNameLogin();
        ZinxKernel::Zinx_SendOut(*pmsg, *m_pProto);
    }

    return bRet;
}

/*处理游戏相关的用户请求*/
UserData* GameRole::ProcMsg(UserData& _poUserData)
{
    /*test，print msg content*/
    GET_REF2DATA(MultiMsg, input, _poUserData);
    for (auto single : input.m_Msgs)
    {
        std::cout << "type: " << single->enMsgType << std::endl;
        std::cout << single->pMsg->Utf8DebugString() << std::endl;
    }
    return nullptr;
}

void GameRole::Fini()
{
    world.DelPlayer(this);
}

int GameRole::GetX()
{
    return (int)x;
}

int GameRole::GetY()
{
    return (int)z;
}
