#include "LemmiGridTable.h"


LemmiGridTable::LemmiGridTable()
:	table()
{
}


LemmiGridTable::~LemmiGridTable()
{
}

void LemmiGridTable::Insert( LemmiEntity* entity )
{
	pair<int, int> gridPosition = std::make_pair((int)(entity->position.x*0.01f), (int)(entity->position.z*0.01f));

	table[gridPosition].push_back(entity);
}

const vector<LemmiEntity*>& LemmiGridTable::GetCell( XMFLOAT3 position ) const
{
	pair<int, int> gridPosition = std::make_pair((int)(position.x*0.01f), (int)(position.z*0.01f));

	return table.at(gridPosition);
}

const vector<LemmiEntity*>& LemmiGridTable::GetCell( int posX, int posY ) const
{
	return table.at(std::make_pair(posX, posY));
}

