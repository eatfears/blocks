#include "position_items.h"

#include <cmath>

#include "primes.h"


BlockInWorld::BlockInWorld(const PointInWorld &pos)
    : TypeInWorld<BlockCoord>(pos.cx, pos.cz, Primes::round(pos.bx), floor(pos.by), Primes::round(pos.bz)) {}

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
