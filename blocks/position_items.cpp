#include "position_items.h"

#include "definitions.h"
#include "primes.h"


BlockInWorld::BlockInWorld(PosInWorld pos) 
    : BlockInChunk(Primes::round(pos.bx), floor(pos.by), Primes::round(pos.bz)), cx(pos.cx), cz(pos.cz) { norm(); }

BlockInWorld BlockInWorld::getSide(char side)
{
	BlockInWorld res(*this);
    switch(side)
    {
	case TOP:
        return res + BlockInChunk(0, 1, 0);
		break;
	case BOTTOM:
        return res + BlockInChunk(0, -1, 0);
		break;
	case RIGHT:
        return res + BlockInChunk(1, 0, 0);
		break;
	case LEFT:
        return res + BlockInChunk(-1, 0, 0);
		break;
	case FRONT:
        return res + BlockInChunk(0, 0, -1);
		break;
	case BACK:
        return res + BlockInChunk(0, 0, 1);
		break;
	}
	return res;
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
}

void PosInWorld::norm()
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
}

BlockInWorld PosInWorld::toBlockInChunk() {
	return 	BlockInWorld(*this);
}
