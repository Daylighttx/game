#pragma once
#include <zinx.h>
class GameProtocol :
    public Iprotocol
{
    // ͨ�� Iprotocol �̳�
    UserData* raw2request(std::string _szInput) override;
    std::string* response2raw(UserData& _oUserData) override;
    Irole* GetMsgProcessor(UserDataMsg& _oUserDataMsg) override;
    Ichannel* GetMsgSender(BytesMsg& _oBytes) override;
};

