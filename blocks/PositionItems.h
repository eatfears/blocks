#pragma once

class PosInWorld;
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

	BlockInWorld(PosInWorld pos);

	BlockInWorld(ChunkCoord cx, ChunkCoord cz, BlockCoord bx, BlockCoord by, BlockCoord bz)
		:cx(cx), cz(cz), bx(bx), by(by), bz(bz) { norm(); }

	BlockInWorld(ChunkCoord cx, ChunkCoord cz)
		:cx(cx), cz(cz), bx(0), by(0), bz(0) {}

	BlockInWorld(BlockCoord bx, BlockCoord by, BlockCoord bz)
		:cx(0), cz(0), bx(bx), by(by), bz(bz) { norm(); }

	~BlockInWorld() {}

	BlockInWorld operator + (BlockInWorld b) {
		BlockInWorld res(cx+b.cx,cz+b.cz,bx+b.bx,by+b.by,bz+b.bz);
		res.norm();
		return res;
	}
	BlockInWorld operator + (BlockInChunk b) {
		BlockInWorld res(cx,cz,bx+b.x,by+b.y,bz+b.z);
		norm();
		return res;
	}

	BlockInWorld getSide(char side);
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
		PosInWorld res(cx,cz,bx+b.x,by+b.y,bz+b.z);
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