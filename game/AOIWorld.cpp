#include "AOIWorld.h"

AOIWorld::AOIWorld(int _x_begin, int _x_end, int _y_begin, int _y_end, int _x_count, int _y_count):
	x_begin(_x_begin),x_end(_x_end),y_begin(_y_begin),y_end(_y_end),x_count(_x_count),y_count(_y_count)
{
	//x�������� = (x��������� - x����ʼ���꣩/ x������������same as y_width
	x_width = (x_end - x_begin) / x_count;
	y_width = (y_end - y_begin) / y_count;

	/*����������*/
	for (int i = 0; i < x_count * y_count; i++)
	{
		Grid tmp;
		m_grids.push_back(tmp);
	}


}

std::list<Player*> AOIWorld::GetSrdPlayers(Player* _player)
{
	return std::list<Player*>();
}

bool AOIWorld::AddPlayer(Player* _player)
{
	/*�������������*/

	//������ = ��x - x��ʼ��/ x������ + (y - y��ʼ��/ y������ * x��������
	int grid_id = (_player->GetX() - x_begin) / x_width + (_player->GetY() - y_begin) / y_width * x_count;

	/*��ӵ���������*/
	m_grids[grid_id].m_player.push_back(_player);

	return true;
}

void AOIWorld::DelPlayer(Player* _player)
{
	int grid_id = (_player->GetX() - x_begin) / x_width + (_player->GetY() - y_begin) / y_width * x_count;

	m_grids[grid_id].m_player.remove(_player);
}
