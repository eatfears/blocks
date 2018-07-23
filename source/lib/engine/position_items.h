#pragma once

#include "common_include/definitions.h"


typedef signed short BlockCoord;
typedef float PointCoord;

template<typename T>
struct TypeInChunk
{
    TypeInChunk() {}
    TypeInChunk(T x, T y, T z) : bx(x), by(y), bz(z) {}
    T bx {}, by {}, bz {};
};

typedef TypeInChunk<BlockCoord> BlockInChunk;
typedef TypeInChunk<PointCoord> PosInChunk;

typedef signed short ChunkCoord;

struct ChunkInWorld
{
    ChunkInWorld() {}
    ChunkInWorld(ChunkCoord x, ChunkCoord z) : cx(x), cz(z) {}
    ChunkCoord cx {}, cz {};
};

inline bool operator < (const ChunkInWorld& a, const ChunkInWorld& b)
{
    if (a.cx!= b.cx) return a.cx< b.cx;
    else return a.cz < b.cz;
}

template<typename T>
struct TypeInWorld : public ChunkInWorld, public TypeInChunk<T>
{
    TypeInWorld() {}

    TypeInWorld(ChunkCoord cx, ChunkCoord cz)
        : ChunkInWorld(cx, cz) {}

    TypeInWorld(T bx, T by, T bz)
        : TypeInChunk<T>(bx, by, bz) { norm(); }

    TypeInWorld(ChunkCoord cx, ChunkCoord cz, T bx, T by, T bz)
        : ChunkInWorld(cx, cz), TypeInChunk<T>(bx, by, bz) { norm(); }

    TypeInWorld(const ChunkInWorld &pos, T bx, T by, T bz)
        : ChunkInWorld(pos), TypeInChunk<T>(bx, by, bz) { norm(); }

    TypeInWorld operator + (const TypeInWorld &operand) const
    {
        return TypeInWorld(cx+operand.cx, cz+operand.cz, this->bx+operand.bx, this->by+operand.by, this->bz+operand.bz);
    }

    TypeInWorld operator + (const TypeInChunk<T> &operand) const
    {
        return TypeInWorld(cx, cz, this->bx+operand.bx, this->by+operand.by, this->bz+operand.bz);
    }

private:
    void norm()
    {
        while (this->bx >= CHUNK_SIZE_XZ)
        {
            this->bx -= CHUNK_SIZE_XZ;
            cx++;
        }
        while (this->bz >= CHUNK_SIZE_XZ)
        {
            this->bz -= CHUNK_SIZE_XZ;
            cz++;
        }
        while (this->bx < 0)
        {
            this->bx += CHUNK_SIZE_XZ;
            cx--;
        }
        while (this->bz < 0)
        {
            this->bz += CHUNK_SIZE_XZ;
            cz--;
        }
    }
};

class PointInWorld : public TypeInWorld<PointCoord>
{
public:
    PointInWorld() {}
    using TypeInWorld<PointCoord>::TypeInWorld;
    PointInWorld(const TypeInWorld<PointCoord> &p) : TypeInWorld<PointCoord>(p) {}

    PointInWorld inv()
    {
        return PointInWorld(-cx, -cz, -this->bx, -this->by, -this->bz);
    }
};

class BlockInWorld : public TypeInWorld<BlockCoord>
{
public:
    BlockInWorld() {}
    using TypeInWorld<BlockCoord>::TypeInWorld;
    BlockInWorld(const TypeInWorld<BlockCoord> &p) : TypeInWorld<BlockCoord>(p) {}

    BlockInWorld(const PointInWorld &pos);

    BlockInWorld getSide(char side) const;
    bool overflow() const { return this->by >= CHUNK_SIZE_Y || this->by < 0; }
};
