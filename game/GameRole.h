#pragma once
#include <zinx.h>
#include "AOIWorld.h"
#include "GameMsg.h"

/*Ϊ�˱���ѭ�����ã������ﴴ��һ��GameProtocol�������˫���*/
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
    GameMsg* CreateTalkBroadCast(std::string _content);
    void ProcTalkMsg(std::string _content);
    void ProcMoveMsg(float _x, float _y, float _z, float _v);
    void ViewAppear(GameRole* _pRole);
    void ViewLost(GameRole* _pRole);
public:
    GameRole();
    virtual ~GameRole();
    // ͨ�� Irole �̳�
    bool Init() override;
    UserData* ProcMsg(UserData& _poUserData) override;
    void Fini() override;
    GameProtocol* m_pProto = NULL;

    // ͨ�� Player �̳�
    int GetX() override;
    int GetY() override;

    
};

