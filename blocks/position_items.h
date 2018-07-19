#pragma once

#include "definitions.h"


struct PosInWorld;
typedef signed short	BlockCoord;
typedef signed short	ChunkCoord;

struct BlockInChunk
{
    BlockInChunk() {}
    BlockInChunk(BlockCoord x, BlockCoord y, BlockCoord z) : bx(x), by(y), bz(z) {}
    BlockCoord bx {}, by {}, bz {};
};

struct ChunkInWorld
{
    ChunkInWorld() {}
    ChunkInWorld(ChunkCoord x, ChunkCoord z) : cx(x), cz(z) {}
    ChunkCoord cx {}, cz {};
};

struct BlockInWorld : public BlockInChunk, public ChunkInWorld
{
    BlockInWorld() : BlockInChunk(), ChunkInWorld() {}

    BlockInWorld(const PosInWorld &pos);

    BlockInWorld(ChunkCoord cx, ChunkCoord cz, BlockCoord bx, BlockCoord by, BlockCoord bz)
        : BlockInChunk(bx, by, bz), ChunkInWorld(cx, cz) { norm(); }

    BlockInWorld(ChunkCoord cx, ChunkCoord cz)
        : ChunkInWorld(cx, cz) {}

    BlockInWorld(BlockCoord bx, BlockCoord by, BlockCoord bz)
        : BlockInChunk(bx, by, bz) { norm(); }

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
        res.norm();
        return res;
    }

    BlockInWorld getSide(char side) const ;
    bool overflow() { return by >= CHUNK_SIZE_Y || by < 0; }

private:
    void norm();
};

struct PosInWorld : public ChunkInWorld
{
    float bx {}, by {}, bz {};

    PosInWorld() {}

    PosInWorld(ChunkCoord cx, ChunkCoord cz, float bx, float by, float bz)
        : ChunkInWorld(cx, cz), bx(bx), by(by), bz(bz) { norm(); }

    PosInWorld(ChunkCoord cx, ChunkCoord cz)
        : ChunkInWorld(cx, cz) {}

    PosInWorld(float bx, float by, float bz)
        : bx(bx), by(by), bz(bz) { norm(); }

    PosInWorld operator + (const BlockInWorld &b)
    {
        PosInWorld res(cx + b.cx, cz + b.cz, bx + b.bx, by + b.by, bz + b.bz);
        res.norm();
        return res;
    }

    PosInWorld operator + (const BlockInChunk &b)
    {
        PosInWorld res(cx, cz, bx + b.bx, by + b.by, bz + b.bz);
        res.norm();
        return res;
    }

    PosInWorld operator + (const PosInWorld & b)
    {
        PosInWorld res(cx + b.cx, cz + b.cz, bx + b.bx, by + b.by, bz + b.bz);
        res.norm();
        return res;
    }

    PosInWorld operator * (int i)
    {
        PosInWorld res(cx*i, cz*i, bx*i,by*i, bz*i);
        res.norm();
        return res;
    }

    PosInWorld inv()
    {
        PosInWorld res(-cx, -cz, -bx, -by, -bz);
        res.norm();
        return res;
    }

    //BlockInWorld getSide(char side);
    bool overflow() { return by >= CHUNK_SIZE_Y || by < 0; }
    BlockInWorld toBlockInChunk();

private:
    void norm();
};
