#include "GameRole.h"
#include "GameMsg.h"

bool GameRole::Init()
{
    return true;
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
}
