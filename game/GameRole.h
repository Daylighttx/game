#pragma once
#include <zinx.h>

/*Ϊ�˱���ѭ�����ã������ﴴ��һ��GameProtocol�������˫���*/
class GameProtocol;

class GameRole :
    public Irole
{
public:
    // ͨ�� Irole �̳�
    bool Init() override;
    UserData* ProcMsg(UserData& _poUserData) override;
    void Fini() override;
    GameProtocol* m_pProto = NULL;
};

