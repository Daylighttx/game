#pragma once
#include <zinx.h>
#include "AOIWorld.h"
#include "GameMsg.h"

/*为了避免循环引用，在这里创建一个GameProtocol对象进行双向绑定*/
class GameProtocol;

class GameRole :
    public Irole,public Player
{
    float x = 0;
    float y = 0;//height
    float z = 0;
    float v = 0;
    int iPid = 0;
    std::string szName;
    GameMsg* CreateIDNameLogin();
    GameMsg* CreateSrdPlayers();
    GameMsg* CreateSelfPosition();
    GameMsg* CreateIDNameLogoff();
public:
    GameRole();
    virtual ~GameRole();
    // 通过 Irole 继承
    bool Init() override;
    UserData* ProcMsg(UserData& _poUserData) override;
    void Fini() override;
    GameProtocol* m_pProto = NULL;

    // 通过 Player 继承
    int GetX() override;
    int GetY() override;

    
};

