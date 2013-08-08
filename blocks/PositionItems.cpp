#include "Blocks_Definitions.h"
#include "PositionItems.h"

BlockInWorld BlockInWorld::getSide(char side)
{
	switch(side) {
	case TOP:
		return *this + BlockInWorld(0,1,0);
		break;
	case BOTTOM:
		return *this + BlockInWorld(0,-1,0);
		break;
	case RIGHT:
		return *this + BlockInWorld(1,0,0);
		break;
	case LEFT:
		return *this + BlockInWorld(-1,0,0);
		break;
	case FRONT:
		return *this + BlockInWorld(0,0,-1);
		break;
	case BACK:
		return *this + BlockInWorld(0,0,1);
		break;
	}
	return *this;
}

void BlockInWorld::norm()
{
	while(bx >= CHUNK_SIZE_XZ) {
		bx -= CHUNK_SIZE_XZ;
		cx++;
	}
	while(bz >= CHUNK_SIZE_XZ) {
		bz -= CHUNK_SIZE_XZ;
		cz++;
	}
	while(bx < 0) {
		bx += CHUNK_SIZE_XZ;
		cx--;
	}
	while(bz < 0) {
		bz += CHUNK_SIZE_XZ;
		cz--;
	}
	if(by >= CHUNK_SIZE_Y) { by = CHUNK_SIZE_Y - 1; }
	if(by < 0) { by = 0; }
}
