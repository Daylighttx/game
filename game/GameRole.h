#pragma once
#include <zinx.h>

/*为了避免循环引用，在这里创建一个GameProtocol对象进行双向绑定*/
class GameProtocol;

class GameRole :
    public Irole
{
public:
    // 通过 Irole 继承
    bool Init() override;
    UserData* ProcMsg(UserData& _poUserData) override;
    void Fini() override;
    GameProtocol* m_pProto = NULL;
};

