#include "position_items.h"

#include "definitions.h"
#include "primes.h"


BlockInWorld::BlockInWorld(const PosInWorld &pos)
    : BlockInChunk(Primes::round(pos.px), floor(pos.py), Primes::round(pos.pz)), ChunkInWorld(pos.cx, pos.cz) { norm(); }

BlockInWorld BlockInWorld::getSide(char side) const
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
    while (bx >= CHUNK_SIZE_XZ)
    {
		bx -= CHUNK_SIZE_XZ;
		cx++;
	}
    while (bz >= CHUNK_SIZE_XZ)
    {
		bz -= CHUNK_SIZE_XZ;
		cz++;
	}
    while (bx < 0)
    {
		bx += CHUNK_SIZE_XZ;
		cx--;
	}
    while (bz < 0)
    {
		bz += CHUNK_SIZE_XZ;
		cz--;
	}
}

void PosInWorld::norm()
{
    while (px >= CHUNK_SIZE_XZ)
    {
        px -= CHUNK_SIZE_XZ;
		cx++;
	}
    while (pz >= CHUNK_SIZE_XZ)
    {
        pz -= CHUNK_SIZE_XZ;
		cz++;
	}
    while (px < 0)
    {
        px += CHUNK_SIZE_XZ;
		cx--;
	}
    while (pz < 0)
    {
        pz += CHUNK_SIZE_XZ;
		cz--;
	}
}

BlockInWorld PosInWorld::toBlockInChunk() {
	return 	BlockInWorld(*this);
}
