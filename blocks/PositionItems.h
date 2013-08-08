#pragma once

typedef signed short	BlockCoord;
typedef signed short	ChunkCoord;

class BlockInChunk {
public:
	BlockInChunk() {};
	BlockInChunk(BlockCoord x, BlockCoord y, BlockCoord z) 
		:x(x), y(y), z(z) {};
	~BlockInChunk() {};

	BlockCoord x;
	BlockCoord y;
	BlockCoord z;
};

class BlockInWorld
{
public:
	ChunkCoord cx, cz;
	BlockCoord bx, by, bz;

	BlockInWorld()
		:cx(0), cz(0), bx(0), by(0), bz(0) {};

	BlockInWorld(ChunkCoord cx, ChunkCoord cz, BlockCoord bx, BlockCoord by, BlockCoord bz)
		:cx(cx), cz(cz), bx(bx), by(by), bz(bz) { norm(); }

	BlockInWorld(BlockCoord bx, BlockCoord by, BlockCoord bz)
		:cx(0), cz(0), bx(bx), by(by), bz(bz) { norm(); }

	~BlockInWorld() {}

	BlockInWorld operator + (BlockInWorld b) {
		cx += b.cx;
		cz += b.cz;
		bx += b.bx;
		by += b.by;
		bz += b.bz;
		norm();
		return *this;
	}
	BlockInWorld operator + (BlockInChunk b) {
		bx += b.x;
		by += b.y;
		bz += b.z;
		norm();
		return *this;
	}

	BlockInWorld getSide(char side);

private:
	void norm();
};

class PosInWorld
{
public:
	PosInWorld() {}
	~PosInWorld() {}

	ChunkCoord cx, cz;
	float bx, by, bz;
};