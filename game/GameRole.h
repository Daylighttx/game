#pragma once
#include <zinx.h>
class GameRole :
    public Irole
{
    // ͨ�� Irole �̳�
    bool Init() override;
    UserData* ProcMsg(UserData& _poUserData) override;
    void Fini() override;
};

