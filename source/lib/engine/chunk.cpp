#include "chunk.h"

#include <fstream>
#include <sstream>

#include "world.h"
#include "light.h"


Chunk::Chunk(const ChunkInWorld &pos, World &world)
    : ChunkInWorld(pos), m_World(world)
{
    logger.info() << "Loading chunk" << pos;

    m_pBlocks = new Block[CHUNK_INDEX_MAX];
    m_SkyLight = new unsigned char[CHUNK_INDEX_MAX];
    m_TorchLight = new unsigned char[CHUNK_INDEX_MAX];

    m_pDisplayedTiles = new std::list<Block*>[6];
    m_pDisplayedWaterTiles = new std::list<Block*>[6];

    for (int i = 0; i < CHUNK_INDEX_MAX; i++)
    {
        m_SkyLight[i] = 0;
        m_TorchLight[i] = 0;
        m_pBlocks[i].material = MAT_NO;
        for (int side = 0; side < 6; side++)
        {
            m_pBlocks[i].visible &= ~(1 << side);
        }
    }
    m_NeedToRender[0] = RENDER_NO_NEED;
    m_NeedToRender[1] = RENDER_NO_NEED;

    m_Listgen = false;
    m_LightToUpdate = true;

    // 1 - solid tiles
    // 2 - water tiles
    m_RenderList = glGenLists(2);
}

Chunk::~Chunk()
{
    logger.info() << "Unloading chunk" << *this;

    unsigned int index = 0;
    BlockCoord chunk_x, chunk_y, chunk_z;

    while (index < CHUNK_INDEX_MAX)
    {
        Chunk::getBlockPositionByIndex(index, &chunk_x, &chunk_y, &chunk_z);

        if (chunk_x > 0 && chunk_x < CHUNK_SIZE_XZ - 1 && chunk_z > 0 && chunk_z < CHUNK_SIZE_XZ - 1)
        {
            index++;
            continue;
        }

        BlockInWorld block_pos(*this, chunk_x, chunk_y, chunk_z);
        Chunk *temp_chunk;
        unsigned int temp_index;
        if (findBlock(block_pos.getSide(RIGHT), temp_chunk, temp_index)) temp_chunk->hideTile(temp_index, LEFT);
        if (findBlock(block_pos.getSide(LEFT), temp_chunk, temp_index)) temp_chunk->hideTile(temp_index, RIGHT);
        if (findBlock(block_pos.getSide(BACK), temp_chunk, temp_index)) temp_chunk->hideTile(temp_index, FRONT);
        if (findBlock(block_pos.getSide(FRONT), temp_chunk, temp_index)) temp_chunk->hideTile(temp_index, BACK);
        index++;
    }

    delete[] m_pBlocks;
    delete[] m_SkyLight;
    delete[] m_TorchLight;

    delete[] m_pDisplayedTiles;
    delete[] m_pDisplayedWaterTiles;

    glDeleteLists(m_RenderList, 2);
}

unsigned int Chunk::addBlock(BlockCoord x, BlockCoord y, BlockCoord z, char mat)
{
    char ex_mat = getBlockMaterial(x, y, z);
    if (ex_mat != MAT_NO || ex_mat == -1)
    {
        return CHUNK_INDEX_MAX;
    }

    unsigned int index = setBlockMaterial(x, y, z, mat);
    if (index < CHUNK_INDEX_MAX)
    {
        m_pBlocks[index].visible = 0;
        m_LightToUpdate = true;
    }
    return index;
}

unsigned int Chunk::removeBlock(BlockCoord x, BlockCoord y, BlockCoord z)
{
    char ex_mat = getBlockMaterial(x, y, z);
    if (ex_mat == MAT_NO || ex_mat == -1 )
    {
        return CHUNK_INDEX_MAX;
    }

    unsigned int index = setBlockMaterial(x, y, z, MAT_NO);
    if (index < CHUNK_INDEX_MAX)
    {
        m_pBlocks[index].visible = 0;
        m_LightToUpdate = true;
    }
    return index;
}

bool Chunk::findBlock(const BlockInWorld &pos, Chunk *&temp_chunk, unsigned int &index) const
{
    if (pos.overflow()) return false;
    bool res;
    if (pos.cx == cx && pos.cz == cz)
    {
        temp_chunk = (Chunk*) this;
        res = getBlockMaterial(pos.bx, pos.by, pos.bz) != MAT_NO;
    }
    else
    {
        res = m_World.findBlock(pos, temp_chunk);
    }
    index = pos.bx*CHUNK_SIZE_XZ + pos.bz + pos.by*CHUNK_SIZE_XZ*CHUNK_SIZE_XZ;
    return res;
}

bool Chunk::placeBlock(const BlockInChunk &pos, char mat)
{
    unsigned int index = addBlock(pos.bx, pos.by, pos.bz, mat);
    if (index >= CHUNK_INDEX_MAX)
    {
        return false;
    }
    Chunk *temp_chunk = nullptr;
    unsigned int temp_index;
    Block *p_block = m_pBlocks + index;
    BlockInWorld wpos(*this, pos);

    if (m_pBlocks[index].material == MAT_WATER)
    {
        if (!findBlock(wpos.getSide(TOP), temp_chunk, temp_index)) showTile(p_block, TOP);
        else if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) temp_chunk->hideTile(temp_index, BOTTOM);
        if (!findBlock(wpos.getSide(BOTTOM), temp_chunk, temp_index)) { if (temp_chunk) showTile(p_block, BOTTOM); }
        else if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) temp_chunk->hideTile(temp_index, TOP);
        if (!findBlock(wpos.getSide(RIGHT), temp_chunk, temp_index)) { if (temp_chunk) showTile(p_block, RIGHT); }
        else if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) temp_chunk->hideTile(temp_index, LEFT);
        if (!findBlock(wpos.getSide(LEFT), temp_chunk, temp_index)) { if (temp_chunk) showTile(p_block, LEFT); }
        else if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) temp_chunk->hideTile(temp_index, RIGHT);
        if (!findBlock(wpos.getSide(BACK), temp_chunk, temp_index)) { if (temp_chunk) showTile(p_block, BACK); }
        else if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) temp_chunk->hideTile(temp_index, FRONT);
        if (!findBlock(wpos.getSide(FRONT), temp_chunk, temp_index)) { if (temp_chunk) showTile(p_block, FRONT); }
        else if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) temp_chunk->hideTile(temp_index, BACK);
    }
    else
    {
        if (!findBlock(wpos.getSide(TOP), temp_chunk, temp_index)) showTile(p_block, TOP);
        else { temp_chunk->hideTile(temp_index, BOTTOM); if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) showTile(p_block, TOP); }
        if (!findBlock(wpos.getSide(BOTTOM), temp_chunk, temp_index)) { if (temp_chunk) showTile(p_block, BOTTOM); }
        else { if (temp_chunk && temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) showTile(p_block, BOTTOM); else temp_chunk->hideTile(temp_index, TOP); }
        if (!findBlock(wpos.getSide(RIGHT), temp_chunk, temp_index)) { if (temp_chunk) showTile(p_block, RIGHT); }
        else {temp_chunk->hideTile(temp_index, LEFT); if (temp_chunk && temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) showTile(p_block, RIGHT); }
        if (!findBlock(wpos.getSide(LEFT), temp_chunk, temp_index)) { if (temp_chunk) showTile(p_block, LEFT); }
        else { temp_chunk->hideTile(temp_index, RIGHT); if (temp_chunk && temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) showTile(p_block, LEFT); }
        if (!findBlock(wpos.getSide(BACK), temp_chunk, temp_index)) { if (temp_chunk) showTile(p_block, BACK); }
        else { temp_chunk->hideTile(temp_index, FRONT); if (temp_chunk && temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) showTile(p_block, BACK); }
        if (!findBlock(wpos.getSide(FRONT), temp_chunk, temp_index)) { if (temp_chunk) showTile(p_block, FRONT); }
        else { temp_chunk->hideTile(temp_index, BACK); if (temp_chunk && temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) showTile(p_block, FRONT); }
    }

    m_NeedToRender[0] = RENDER_NEED;
    m_NeedToRender[1] = RENDER_NEED;

    return true;
}

bool Chunk::unplaceBlock(const BlockInChunk &pos)
{
    unsigned int index = getIndexByPosition(pos);
    if (index >= CHUNK_INDEX_MAX)
    {
        return false;
    }
    Chunk *temp_chunk = nullptr;
    unsigned int temp_index;
    Block *p_block = m_pBlocks + index;
    BlockInWorld wpos(*this, pos);

    if (!findBlock(wpos.getSide(TOP), temp_chunk, temp_index)|| m_pBlocks[index].material == MAT_WATER) hideTile(p_block, TOP);
    else { temp_chunk->showTile(temp_index, BOTTOM); if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) hideTile(p_block, TOP); }
    if (!findBlock(wpos.getSide(BOTTOM), temp_chunk, temp_index)) hideTile(p_block, BOTTOM);
    else { temp_chunk->showTile(temp_index, TOP); if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) hideTile(p_block, BOTTOM); }
    if (!findBlock(wpos.getSide(RIGHT), temp_chunk, temp_index)) hideTile(p_block, RIGHT);
    else { temp_chunk->showTile(temp_index, LEFT); if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) hideTile(p_block, RIGHT); }
    if (!findBlock(wpos.getSide(LEFT), temp_chunk, temp_index)) hideTile(p_block, LEFT);
    else { temp_chunk->showTile(temp_index, RIGHT); if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) hideTile(p_block, LEFT); }
    if (!findBlock(wpos.getSide(BACK), temp_chunk, temp_index)) hideTile(p_block, BACK);
    else { temp_chunk->showTile(temp_index, FRONT); if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) hideTile(p_block, BACK); }
    if (!findBlock(wpos.getSide(FRONT), temp_chunk, temp_index)) hideTile(p_block, FRONT);
    else { temp_chunk->showTile(temp_index, BACK); if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) hideTile(p_block, FRONT); }

    removeBlock(wpos.bx, wpos.by, wpos.bz);

    m_NeedToRender[0] = RENDER_NEED;
    m_NeedToRender[1] = RENDER_NEED;

    return true;
}

void Chunk::showTile(Block *p_block, unsigned char side)
{
    if (!p_block || p_block->material == MAT_NO || p_block->visible & (1 << side)) return;

    std::list<Block*> *p_tiles;
    if (p_block->material == MAT_WATER)
    {
        p_tiles = &m_pDisplayedWaterTiles[side];
    }
    else
    {
        p_tiles = &m_pDisplayedTiles[side];
    }
    p_tiles->push_back(p_block);

    p_block->visible |= (1 << side);
}

void Chunk::hideTile(Block *p_block, unsigned char side)
{
    if (!p_block || p_block->material == MAT_NO || !(p_block->visible & (1 << side))) return;

    std::list<Block*> *p_tiles;
    if (p_block->material == MAT_WATER)
    {
        p_tiles = &m_pDisplayedWaterTiles[side];
    }
    else
    {
        p_tiles = &m_pDisplayedTiles[side];
    }
    auto it = p_tiles->begin();

    while (it != p_tiles->end())
    {
        if (*it == p_block) break;
        ++it;
    }
    if (it == p_tiles->end()) return;

    (*it)->visible &= ~(1 << side);
    p_tiles->erase(it);
}

void Chunk::showTile(unsigned int index, unsigned char side)
{
    showTile(m_pBlocks + index, side);
}

void Chunk::hideTile(unsigned int index, unsigned char side)
{
    hideTile(m_pBlocks + index, side);
}

unsigned int Chunk::setBlockMaterial(BlockCoord x, BlockCoord y, BlockCoord z, char mat)
{
    if (x < 0 || z < 0 || y < 0 || x >= CHUNK_SIZE_XZ || z >= CHUNK_SIZE_XZ || y >= CHUNK_SIZE_Y)
    {
        return CHUNK_INDEX_MAX;
    }
    unsigned int index = getIndexByPosition(x, y, z);
    m_pBlocks[index].material = mat;
    return index;
}

void Chunk::drawLoadedBlocks()
{
    unsigned int index = 0;
    BlockCoord x, y, z;
    Block *p_block;
    char tmp;

    while (index < CHUNK_INDEX_MAX)
    {
        getBlockPositionByIndex(index, &x, &y, &z);
        p_block = m_pBlocks + index;

        if (m_pBlocks[index].material != MAT_NO)
        {
            if (m_pBlocks[index].material == MAT_WATER)
            {
                if (getBlockMaterial(x, y + 1, z) == MAT_NO || y == CHUNK_SIZE_Y - 1) showTile(p_block, TOP);
                if (getBlockMaterial(x, y - 1, z) == MAT_NO) showTile(p_block, BOTTOM);
                if (getBlockMaterial(x + 1, y, z) == MAT_NO) showTile(p_block, RIGHT);
                if (getBlockMaterial(x - 1, y, z) == MAT_NO) showTile(p_block, LEFT);
                if (getBlockMaterial(x, y, z + 1) == MAT_NO) showTile(p_block, BACK);
                if (getBlockMaterial(x, y, z - 1) == MAT_NO) showTile(p_block, FRONT);
            }
            else
            {
                if ((tmp = getBlockMaterial(x, y + 1, z)) == MAT_NO || tmp == MAT_WATER || y == CHUNK_SIZE_Y - 1) showTile(p_block, TOP);
                if ((tmp = getBlockMaterial(x, y - 1, z)) == MAT_NO || tmp == MAT_WATER) showTile(p_block, BOTTOM);
                if ((tmp = getBlockMaterial(x + 1, y, z)) == MAT_NO || tmp == MAT_WATER) showTile(p_block, RIGHT);
                if ((tmp = getBlockMaterial(x - 1, y, z)) == MAT_NO || tmp == MAT_WATER) showTile(p_block, LEFT);
                if ((tmp = getBlockMaterial(x, y, z + 1)) == MAT_NO || tmp == MAT_WATER) showTile(p_block, BACK);
                if ((tmp = getBlockMaterial(x, y, z - 1)) == MAT_NO || tmp == MAT_WATER) showTile(p_block, FRONT);
            }
        }
        index++;
    }

    drawNeighboringChunks();
}

void Chunk::drawNeighboringChunks()
{
    unsigned int temp_index;
    BlockCoord chunk_x, chunk_y, chunk_z;
    Chunk *temp_chunk;
    Block *p_block;

    Chunk *rc, *lc, *bc, *fc;

    rc = m_World.getChunkByPosition(ChunkInWorld(cx + 1, cz));
    lc = m_World.getChunkByPosition(ChunkInWorld(cx - 1, cz));
    bc = m_World.getChunkByPosition(ChunkInWorld(cx, cz + 1));
    fc = m_World.getChunkByPosition(ChunkInWorld(cx, cz - 1));

    unsigned int index = 0;
    while (index < CHUNK_INDEX_MAX)
    {
        getBlockPositionByIndex(index, &chunk_x, &chunk_y, &chunk_z);

        if (chunk_x > 0 && chunk_x < CHUNK_SIZE_XZ - 1 && chunk_z > 0 && chunk_z < CHUNK_SIZE_XZ - 1)
        {
            index++;
            continue;
        }

        BlockInWorld pos(*this, chunk_x, chunk_y, chunk_z);

        if (m_pBlocks[index].material != MAT_NO)
        {
            p_block = m_pBlocks + index;

            if (m_pBlocks[index].material == MAT_WATER)
            {
                if (rc && !rc->findBlock(pos.getSide(RIGHT), temp_chunk, temp_index)) { if (temp_chunk) showTile(p_block, RIGHT); }
                if (lc && !lc->findBlock(pos.getSide(LEFT), temp_chunk, temp_index)) { if (temp_chunk) showTile(p_block, LEFT); }
                if (bc && !bc->findBlock(pos.getSide(BACK), temp_chunk, temp_index)) { if (temp_chunk) showTile(p_block, BACK); }
                if (fc && !fc->findBlock(pos.getSide(FRONT), temp_chunk, temp_index)) { if (temp_chunk) showTile(p_block, FRONT); }
            }
            else
            {
                if (rc && (!rc->findBlock(pos.getSide(RIGHT), temp_chunk, temp_index) || temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) {
                    if (temp_chunk) showTile(p_block, RIGHT); }
                if (lc && (!lc->findBlock(pos.getSide(LEFT), temp_chunk, temp_index) || temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) {
                    if (temp_chunk) showTile(p_block, LEFT); }
                if (bc && (!bc->findBlock(pos.getSide(BACK), temp_chunk, temp_index) || temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) {
                    if (temp_chunk) showTile(p_block, BACK); }
                if (fc && (!fc->findBlock(pos.getSide(FRONT), temp_chunk, temp_index) || temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) {
                    if (temp_chunk) showTile(p_block, FRONT); }
            }
        }

        // draw neighboring tiles
        if (m_pBlocks[index].material == MAT_NO || m_pBlocks[index].material == MAT_WATER)
        {
            if (rc && rc->findBlock(pos.getSide(RIGHT), temp_chunk, temp_index) && temp_chunk->m_pBlocks[temp_index].material != MAT_WATER) if (temp_chunk->m_pBlocks != m_pBlocks)
                temp_chunk->showTile(temp_index, LEFT);
            if (lc && lc->findBlock(pos.getSide(LEFT), temp_chunk, temp_index) && temp_chunk->m_pBlocks[temp_index].material != MAT_WATER) if (temp_chunk->m_pBlocks != m_pBlocks)
                temp_chunk->showTile(temp_index, RIGHT);
            if (bc && bc->findBlock(pos.getSide(BACK), temp_chunk, temp_index) && temp_chunk->m_pBlocks[temp_index].material != MAT_WATER) if (temp_chunk->m_pBlocks != m_pBlocks)
                temp_chunk->showTile(temp_index, FRONT);
            if (fc && fc->findBlock(pos.getSide(FRONT), temp_chunk, temp_index) && temp_chunk->m_pBlocks[temp_index].material != MAT_WATER) if (temp_chunk->m_pBlocks != m_pBlocks)
                temp_chunk->showTile(temp_index, BACK);
        }
        index++;
    }
}

void Chunk::open()
{
    bool loaded = false;
    std::fstream savefile;
    std::stringstream temp;
    std::string filename;

    temp << "save//" << cx << "_" << cz << ".mp";
    filename = temp.str();

    savefile.open(filename, std::fstream::in | std::fstream::binary);
    if (savefile.is_open())
    {
        loaded = m_World.m_Landscape.load(*this, savefile);
        savefile.close();
    }

    if (!loaded)
    {
        m_World.m_Landscape.generate(*this);
        //m_World.m_Landscape.fill(*this, 0, 0.9, 64);
        //m_World.m_Landscape.fill(*this, MAT_DIRT, 1, 64);
    }
}

void Chunk::save() const
{
    std::fstream savefile;
    std::stringstream temp;
    std::string filename;

    temp << "save//" << cx << "_" << cz << ".mp";
    filename = temp.str();

    savefile.open (filename, std::fstream::out | std::fstream::binary);
    if (savefile.is_open())
    {
        m_World.m_Landscape.save(*this, savefile);
        savefile.close();
    }
}

void Chunk::render(char material, int *rendered) /*const*/
{
    glPushMatrix();
    glTranslated((cx-m_World.m_Player.m_Position.cx)*CHUNK_SIZE_XZ, 0, (cz-m_World.m_Player.m_Position.cz)*CHUNK_SIZE_XZ);

    GLenum mode = GL_EXECUTE;
    int list_to_render = 0;

    if (material == MAT_WATER)
    {
        list_to_render = 1;
    }

    if (m_NeedToRender[list_to_render] == RENDER_NEED)
    {
        mode = GL_COMPILE;
    }
    else if (m_NeedToRender[list_to_render] == RENDER_MAYBE)
    {
        int prob = 1000/(*rendered*5 + 1);
        int r = rand()%1000;

        if (r <= prob /*|| 1*/)
            mode = GL_COMPILE;
    }

    // todo: GL_RENDER dont compile chunk
    Chunk *chunk = m_World.m_Player.m_pChunk;
    if (chunk)
    {
        if (chunk->cx >= cx - 1 && chunk->cz >= cz - 1 && chunk->cx <= cx + 1 && chunk->cz <= cz + 1)
        {
            m_NeedToRender[list_to_render] = RENDER_NEED;
            mode = GL_RENDER;
        }
    }

    if (mode == GL_COMPILE || mode == GL_COMPILE_AND_EXECUTE || mode == GL_RENDER)
    {
        if (mode != GL_RENDER)
        {
            glNewList(m_RenderList + list_to_render, mode);
        }

        //1-sided tiles
        if (material == MAT_WATER)
        {
            glTranslated(0.0, -0.95/8, 0.0);
            glDisable(GL_CULL_FACE);
        }
        else
        {
            glEnable(GL_CULL_FACE);
        }

        glBegin(GL_QUADS);

        std::list<Block*> *p_tiles;
        BlockCoord x, y, z;
        BlockInWorld temp, this_pos = BlockInWorld(cx, cz);

        for (int side = 0; side < 6; side++)
        {
            if (material == MAT_WATER)
            {
                p_tiles = &m_pDisplayedWaterTiles[side];
            }
            else
            {
                p_tiles = &m_pDisplayedTiles[side];
            }

            auto it = p_tiles->begin();
            while (it != p_tiles->end())
            {
                if (getBlockPositionByPointer(*it, &x, &y, &z))
                {
                    Light::blockLight(m_World, *this, side, x, y, z);
                    temp = this_pos + BlockInWorld(x, y, z);
                    drawTile(temp, *it, side);

                }
                ++it;
            }
        }
        glEnd();
        if (material == MAT_WATER)
        {
            glTranslated(0.0, 0.95/8, 0.0);
        }
        if (m_NeedToRender[list_to_render] == RENDER_MAYBE)
        {
            (*rendered) ++;
        }
        if (mode != GL_RENDER)
        {
            glEndList();
            m_NeedToRender[list_to_render] = RENDER_NO_NEED;
        }
    }

    if (mode != GL_COMPILE_AND_EXECUTE && mode != GL_RENDER)
    {
        glCallList(m_RenderList + list_to_render);
    }

    glPopMatrix();
}

void Chunk::drawTile(const BlockInWorld &pos, Block* block, char side) const
{
    GLdouble
            x_coord = pos.bx - 0.5,
            y_coord = pos.by,
            z_coord = pos.bz - 0.5;

    double offset_x, offset_y;
    m_World.m_MaterialLib.getTextureOffsets(offset_x, offset_y, block->material, block->visible, side);

    static const double space = 0.0002;
    switch(side)
    {
    case TOP:
    {
        Light::softLight(m_World, *this, pos, side, 0);
        glTexCoord2d(0.0625 - space + offset_x, 0.0 + space + offset_y);
        glVertex3d(x_coord, y_coord + 1, z_coord);

        Light::softLight(m_World, *this, pos, side, 1);
        glTexCoord2d(0.0 + space + offset_x, 0.0 + space + offset_y);
        glVertex3d(x_coord, y_coord + 1, z_coord + 1);

        Light::softLight(m_World, *this, pos, side, 2);
        glTexCoord2d(0.0 + space + offset_x, 0.0625 - space + offset_y);
        glVertex3d(x_coord + 1, y_coord + 1, z_coord + 1);

        Light::softLight(m_World, *this, pos, side, 3);
        glTexCoord2d(0.0625 - space + offset_x, 0.0625 - space + offset_y);
        glVertex3d(x_coord + 1, y_coord + 1, z_coord);
    }
        break;
    case BOTTOM:
    {
        Light::softLight(m_World, *this, pos, side, 4);
        glTexCoord2d(0.0625 - space + offset_x, 0.0 + space + offset_y);
        glVertex3d(x_coord, y_coord, z_coord);

        Light::softLight(m_World, *this, pos, side, 7);
        glTexCoord2d(0.0625 - space + offset_x, 0.0625 - space + offset_y);
        glVertex3d(x_coord + 1, y_coord, z_coord);

        Light::softLight(m_World, *this, pos, side, 6);
        glTexCoord2d(0.0 + space + offset_x, 0.0625 - space + offset_y);
        glVertex3d(x_coord + 1, y_coord, z_coord + 1);

        Light::softLight(m_World, *this, pos, side, 5);
        glTexCoord2d(0.0 + space + offset_x, 0.0 + space + offset_y);
        glVertex3d(x_coord, y_coord, z_coord + 1);
    }
        break;
    case RIGHT:
    {
        Light::softLight(m_World, *this, pos, side, 7);
        glTexCoord2d(0.0625 - space + offset_x, 0.0625 - space + offset_y);
        glVertex3d(x_coord + 1, y_coord, z_coord);

        Light::softLight(m_World, *this, pos, side, 3);
        glTexCoord2d(0.0625 - space + offset_x, 0.0 + space + offset_y);
        glVertex3d(x_coord + 1, y_coord + 1, z_coord);

        Light::softLight(m_World, *this, pos, side, 2);
        glTexCoord2d(0.0 + space + offset_x, 0.0 + space + offset_y);
        glVertex3d(x_coord + 1, y_coord + 1, z_coord + 1);

        Light::softLight(m_World, *this, pos, side, 6);
        glTexCoord2d(0.0 + space + offset_x, 0.0625 - space + offset_y);
        glVertex3d(x_coord + 1, y_coord, z_coord + 1);
    }
        break;
    case LEFT:
    {
        Light::softLight(m_World, *this, pos, side, 4);
        glTexCoord2d(0.0 + space + offset_x, 0.0625 - space + offset_y);
        glVertex3d(x_coord, y_coord, z_coord);

        Light::softLight(m_World, *this, pos, side, 5);
        glTexCoord2d(0.0625 - space + offset_x, 0.0625 - space + offset_y);
        glVertex3d(x_coord, y_coord, z_coord + 1);

        Light::softLight(m_World, *this, pos, side, 1);
        glTexCoord2d(0.0625 - space + offset_x, 0.0 + space + offset_y);
        glVertex3d(x_coord, y_coord + 1, z_coord + 1);

        Light::softLight(m_World, *this, pos, side, 0);
        glTexCoord2d(0.0 + space + offset_x, 0.0 + space + offset_y);
        glVertex3d(x_coord, y_coord + 1, z_coord);
    }
        break;
    case BACK:
    {
        Light::softLight(m_World, *this, pos, side, 5);
        glTexCoord2d(0.0 + space + offset_x, 0.0625 - space + offset_y);
        glVertex3d(x_coord, y_coord, z_coord + 1);

        Light::softLight(m_World, *this, pos, side, 6);
        glTexCoord2d(0.0625 - space + offset_x, 0.0625 - space + offset_y);
        glVertex3d(x_coord + 1, y_coord, z_coord + 1);

        Light::softLight(m_World, *this, pos, side, 2);
        glTexCoord2d(0.0625 - space + offset_x, 0.0 + space + offset_y);
        glVertex3d(x_coord + 1, y_coord + 1, z_coord + 1);

        Light::softLight(m_World, *this, pos, side, 1);
        glTexCoord2d(0.0 + space + offset_x, 0.0 + space + offset_y);
        glVertex3d(x_coord, y_coord + 1, z_coord + 1);
    }
        break;
    case FRONT:
    {
        Light::softLight(m_World, *this, pos, side, 4);
        glTexCoord2d(0.0625 - space + offset_x, 0.0625 - space + offset_y);
        glVertex3d(x_coord, y_coord, z_coord);

        Light::softLight(m_World, *this, pos, side, 0);
        glTexCoord2d(0.0625 - space + offset_x, 0.0 + space + offset_y);
        glVertex3d(x_coord, y_coord + 1, z_coord);

        Light::softLight(m_World, *this, pos, side, 3);
        glTexCoord2d(0.0 + space + offset_x, 0.0 + space + offset_y);
        glVertex3d(x_coord + 1, y_coord + 1, z_coord);

        Light::softLight(m_World, *this, pos, side, 7);
        glTexCoord2d(0.0 + space + offset_x, 0.0625 - space + offset_y);
        glVertex3d(x_coord + 1, y_coord, z_coord);
    }
        break;
    }
}
