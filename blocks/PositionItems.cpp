#include "Blocks_Definitions.h"
#include "PositionItems.h"
#include "Primes.h"

BlockInWorld::BlockInWorld(PosInWorld pos) 
	:cx(pos.cx), cz(pos.cz), bx(Primes::Round(pos.bx/BLOCK_SIZE)), by(Primes::Round(pos.by/BLOCK_SIZE)), bz(Primes::Round(pos.bz/BLOCK_SIZE)) {};

BlockInWorld BlockInWorld::getSide(char side)
{
	BlockInWorld res(*this);
	switch(side) {
	case TOP:
		return res + BlockInWorld(0,1,0);
		break;
	case BOTTOM:
		return res + BlockInWorld(0,-1,0);
		break;
	case RIGHT:
		return res + BlockInWorld(1,0,0);
		break;
	case LEFT:
		return res + BlockInWorld(-1,0,0);
		break;
	case FRONT:
		return res + BlockInWorld(0,0,-1);
		break;
	case BACK:
		return res + BlockInWorld(0,0,1);
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
	while(bx >= CHUNK_SIZE_XZ*BLOCK_SIZE) {
		bx -= CHUNK_SIZE_XZ*BLOCK_SIZE;
		cx++;
	}
	while(bz >= CHUNK_SIZE_XZ*BLOCK_SIZE) {
		bz -= CHUNK_SIZE_XZ*BLOCK_SIZE;
		cz++;
	}
	while(bx < 0) {
		bx += CHUNK_SIZE_XZ*BLOCK_SIZE;
		cx--;
	}
	while(bz < 0) {
		bz += CHUNK_SIZE_XZ*BLOCK_SIZE;
		cz--;
	}
}

BlockInWorld PosInWorld::toBlockInChunk() {
	return 	BlockInWorld(*this);
}