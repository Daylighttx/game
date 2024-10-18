#include "GameProtocol.h"
#include "GameMsg.h"
#include <iostream>


/*���������ͨ��������ԭʼ����
����ֵ��ת�������Ϣ����MultiMsg
ת����ʽ�� TCPճ������*/

UserData* GameProtocol::raw2request(std::string _szInput)
{
	MultiMsg* pRet = new MultiMsg();
	szLast.append(_szInput);

	while (1)
	{

		if (szLast.size() < 8)
		{
			break;
		}

		/*��ǰ�ĸ��ֽ��ж�ȡ�����Ϣ���ݳ���*/
		int iLength = 0;
		iLength |= szLast[0] << 0;
		iLength |= szLast[1] << 8;
		iLength |= szLast[2] << 16;
		iLength |= szLast[3] << 24;
		/*�м��ĸ��ֽڶ�����*/
		int id = 0;
		id |= szLast[4] << 0;
		id |= szLast[5] << 8;
		id |= szLast[6] << 16;
		id |= szLast[7] << 24;

		/*ͨ�������ĳ����жϺ��������Ƿ�Ϸ�*/
		if (szLast.size() - 8 < iLength)
		{
			/*��Ϣ���жϣ�do nothing*/
			break;
		}

		/*����һ���û�����*/
		GameMsg* pMsg = new GameMsg((GameMsg::MSG_TYPE)id, szLast.substr(8, iLength));
		pRet->m_Msgs.push_back(pMsg);
		/*�����Ѿ�����ɹ��ı���*/
		szLast.erase(0, 8 + iLength);
	}

	for (auto single : pRet->m_Msgs)
	{
		std::cout << single->pMsg->Utf8DebugString() << std::endl;
	}
	return pRet;
}

std::string* GameProtocol::response2raw(UserData& _oUserData)
{
	return nullptr;
}
 
Irole* GameProtocol::GetMsgProcessor(UserDataMsg& _oUserDataMsg)
{
	return nullptr;
}

Ichannel* GameProtocol::GetMsgSender(BytesMsg& _oBytes)
{
	return nullptr;
}
