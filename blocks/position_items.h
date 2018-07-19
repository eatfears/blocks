#pragma once

#include "definitions.h"


class PosInWorld;
typedef signed short	BlockCoord;
typedef signed short	ChunkCoord;

class BlockInChunk
{
public:
    BlockInChunk() {}
    BlockInChunk(BlockCoord x, BlockCoord y, BlockCoord z) : bx(x), by(y), bz(z) {}
    ~BlockInChunk() {}

    BlockCoord bx;
    BlockCoord by;
    BlockCoord bz;
};

class BlockInWorld : public BlockInChunk
{
public:
    ChunkCoord cx, cz;

    BlockInWorld() : BlockInChunk(0, 0, 0), cx(0), cz(0) {}

	BlockInWorld(PosInWorld pos);

	BlockInWorld(ChunkCoord cx, ChunkCoord cz, BlockCoord bx, BlockCoord by, BlockCoord bz)
        : BlockInChunk(bx, by, bz), cx(cx), cz(cz) { norm(); }

	BlockInWorld(ChunkCoord cx, ChunkCoord cz)
        : BlockInChunk(0, 0, 0), cx(cx), cz(cz) {}

	BlockInWorld(BlockCoord bx, BlockCoord by, BlockCoord bz)
        : BlockInChunk(bx, by, bz), cx(0), cz(0) { norm(); }

	~BlockInWorld() {}

    BlockInWorld operator + (const BlockInWorld &b)
    {
        BlockInWorld res(cx+b.cx, cz+b.cz, bx+b.bx, by+b.by, bz+b.bz);
		res.norm();
		return res;
	}
    BlockInWorld operator + (const BlockInChunk &b)
    {
        BlockInWorld res(cx, cz, bx+b.bx, by+b.by, bz+b.bz);
		norm();
		return res;
	}

    BlockInWorld getSide(char side) ;
	bool overflow() { return by >= CHUNK_SIZE_Y || by < 0; }

private:
	void norm();
};

class PosInWorld
{
public:
	ChunkCoord cx, cz;
	float bx, by, bz;

	PosInWorld()
		:cx(0), cz(0), bx(0), by(0), bz(0) {};

	PosInWorld(ChunkCoord cx, ChunkCoord cz, float bx, float by, float bz)
		:cx(cx), cz(cz), bx(bx), by(by), bz(bz) { norm(); }

	PosInWorld(ChunkCoord cx, ChunkCoord cz)
		:cx(cx), cz(cz), bx(0), by(0), bz(0) {}

	PosInWorld(float bx, float by, float bz)
		:cx(0), cz(0), bx(bx), by(by), bz(bz) { norm(); }

	~PosInWorld() {}

	PosInWorld operator + (BlockInWorld b) {
		PosInWorld res(cx+b.cx,cz+b.cz,bx+b.bx,by+b.by,bz+b.bz);
		res.norm();
		return res;
	}
	PosInWorld operator + (BlockInChunk b) {
        PosInWorld res(cx,cz,bx+b.bx,by+b.by,bz+b.bz);
		norm();
		return res;
	}

	PosInWorld operator + (PosInWorld b) {
		PosInWorld res(cx+b.cx,cz+b.cz,bx+b.bx,by+b.by,bz+b.bz);
		res.norm();
		return res;
	}

	PosInWorld operator * (int i) {
		PosInWorld res(cx*i,cz*i,bx*i,by*i,bz*i);
		res.norm();
		return res;
	}

	PosInWorld inv() {
		PosInWorld res(-cx,-cz,-bx,-by,-bz);
		res.norm();
		return res;
	}

	//BlockInWorld getSide(char side);
	bool overflow() { return by >= CHUNK_SIZE_Y || by < 0; }
	BlockInWorld toBlockInChunk();

private:
	void norm();
};
