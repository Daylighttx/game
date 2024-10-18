#pragma once
#include <zinx.h>

class GameChannel;

class GameProtocol :
    public Iprotocol
{
    std::string szLast;

public:
    GameChannel* m_channel = NULL;

    // ͨ�� Iprotocol �̳�
    UserData* raw2request(std::string _szInput) override;
    std::string* response2raw(UserData& _oUserData) override;
    Irole* GetMsgProcessor(UserDataMsg& _oUserDataMsg) override;
    Ichannel* GetMsgSender(BytesMsg& _oBytes) override;
};

