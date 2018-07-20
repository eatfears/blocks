#include "chunk.h"

#include <fstream>
#include <sstream>

#include "world.h"
#include "light.h"


Chunk::Chunk(ChunkCoord x, ChunkCoord z, World& wrld)
    : m_World(wrld), m_X(x), m_Z(z)
{
    m_pBlocks = new Block[CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y];
    m_SkyLight = new char[CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y];
    m_TorchLight = new char[CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y];

    m_pDisplayedTiles = new std::list<Block *>[6];
    m_pDisplayedWaterTiles = new std::list<Block *>[6];

    for (int i = 0; i < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y; i++)
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

    //	mutex = CreateMutex(NULL, false, NULL);
    //	loadmutex = CreateMutex(NULL, false, NULL);
}

Chunk::~Chunk()
{
    delete[] m_pBlocks;
    delete[] m_SkyLight;
    delete[] m_TorchLight;

    delete[] m_pDisplayedTiles;
    delete[] m_pDisplayedWaterTiles;

    glDeleteLists(m_RenderList, 2);
}

int Chunk::addBlock(BlockCoord x, BlockCoord y, BlockCoord z, char mat)
{
    x = x % CHUNK_SIZE_XZ;
    y = y % CHUNK_SIZE_Y;
    z = z % CHUNK_SIZE_XZ;
    if ((getBlockMaterial(x, y, z) != MAT_NO)||(getBlockMaterial(x, y, z) == -1)) return -1;

    BlockCoord index = setBlockMaterial(x, y, z, mat);
    m_pBlocks[index].visible = 0;
    m_LightToUpdate = true;

    return index;
}

int Chunk::removeBlock(BlockCoord x, BlockCoord y, BlockCoord z)
{
    if ((getBlockMaterial(x, y, z) == MAT_NO)||(getBlockMaterial(x, y, z) == -1)) return -1;

    BlockCoord index = setBlockMaterial(x, y, z, MAT_NO);
    m_pBlocks[index].visible = 0;
    m_LightToUpdate = true;

    return index;
}

void Chunk::showTile(Block *p_block, char side)
{
    if (!p_block) return;
    if (p_block->material == MAT_NO) return;
    if (p_block->visible & (1 << side)) return;

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

void Chunk::hideTile(Block *p_block, char side)
{
    if (!p_block) return;
    if (p_block->material == MAT_NO) return;
    if (!(p_block->visible & (1 << side))) return;

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

char Chunk::getBlockMaterial(BlockCoord x, BlockCoord y, BlockCoord z)
{
    if ((x < 0)||(z < 0)||(y < 0)||(x >= CHUNK_SIZE_XZ)||(z >= CHUNK_SIZE_XZ)||(y >= CHUNK_SIZE_Y))
    {
        return -1;
    }
    return m_pBlocks[x*CHUNK_SIZE_XZ + z + y*CHUNK_SIZE_XZ*CHUNK_SIZE_XZ].material;
}

int Chunk::setBlockMaterial(BlockCoord x, BlockCoord y, BlockCoord z, char cMat)
{
    if ((x >= CHUNK_SIZE_XZ)||(z >= CHUNK_SIZE_XZ)||(y >= CHUNK_SIZE_Y))
    {
        return -1;
    }
    int index = getIndexByPosition(x, y, z);
    m_pBlocks[index].material = cMat;
    return index;
}

int Chunk::getBlockPositionByPointer(Block *tCurrentBlock, BlockCoord *x, BlockCoord *y, BlockCoord *z) const
{
    int t = tCurrentBlock - m_pBlocks;
    if (getBlockPositionByIndex(t, x, y, z) == -1)
    {
        return -1;
    }
    return 0;
}

int Chunk::getBlockPositionByIndex(int index, BlockCoord *x, BlockCoord *y, BlockCoord *z)
{
    if ((index < 0)||(index >= CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y))
    {
        return -1;
    }
    *z  = index%CHUNK_SIZE_XZ;
    index /= CHUNK_SIZE_XZ;
    *x = index%CHUNK_SIZE_XZ;
    index /= CHUNK_SIZE_XZ;
    *y = index;
    return 0;
}

int Chunk::getIndexByPosition(BlockCoord x, BlockCoord y, BlockCoord z)
{
    return x*CHUNK_SIZE_XZ + z + y*CHUNK_SIZE_XZ*CHUNK_SIZE_XZ;
}

void Chunk::drawLoadedBlocks()
{
    int index = 0;
    BlockCoord x, y, z;

    while (index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
    {
        getBlockPositionByPointer(m_pBlocks + index, &x, &y, &z);

        if (m_pBlocks[index].material != MAT_NO)
        {
            if (m_pBlocks[index].material == MAT_WATER)
            {
                if ((getBlockMaterial(x, y + 1, z) == MAT_NO)||(y == CHUNK_SIZE_Y - 1)) showTile(m_pBlocks + index, TOP);
                if (getBlockMaterial(x, y - 1, z) == MAT_NO) showTile(m_pBlocks + index, BOTTOM);
                if (getBlockMaterial(x + 1, y, z) == MAT_NO) showTile(m_pBlocks + index, RIGHT);
                if (getBlockMaterial(x - 1, y, z) == MAT_NO) showTile(m_pBlocks + index, LEFT);
                if (getBlockMaterial(x, y, z + 1) == MAT_NO) showTile(m_pBlocks + index, BACK);
                if (getBlockMaterial(x, y, z - 1) == MAT_NO) showTile(m_pBlocks + index, FRONT);
            }
            else
            {
                if ((getBlockMaterial(x, y + 1, z) == MAT_NO)||(getBlockMaterial(x, y + 1, z) == MAT_WATER)||(y == CHUNK_SIZE_Y - 1))
                    showTile(m_pBlocks + index, TOP);
                if ((getBlockMaterial(x, y - 1, z) == MAT_NO)||(getBlockMaterial(x, y - 1, z) == MAT_WATER))
                    showTile(m_pBlocks + index, BOTTOM);
                if ((getBlockMaterial(x + 1, y, z) == MAT_NO)||(getBlockMaterial(x + 1, y, z) == MAT_WATER))
                    showTile(m_pBlocks + index, RIGHT);
                if ((getBlockMaterial(x - 1, y, z) == MAT_NO)||(getBlockMaterial(x - 1, y, z) == MAT_WATER))
                    showTile(m_pBlocks + index, LEFT);
                if ((getBlockMaterial(x, y, z + 1) == MAT_NO)||(getBlockMaterial(x, y, z + 1) == MAT_WATER))
                    showTile(m_pBlocks + index, BACK);
                if ((getBlockMaterial(x, y, z - 1) == MAT_NO)||(getBlockMaterial(x, y, z - 1) == MAT_WATER))
                    showTile(m_pBlocks + index, FRONT);
            }
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

    temp << "save//" << m_X << "_" << m_Z << ".mp";
    filename = temp.str();

    savefile.open(filename, std::fstream::in | std::fstream::binary);
    //	if (savefile.is_open()
    //  {
    //		WaitForSingleObject(loadmutex, INFINITE);
    //		loaded = wWorld.lLandscape.Load(*this, savefile);
    //		ReleaseMutex(loadmutex);
    //		savefile.close();
    //	}

    if (!loaded)
    {
        m_World.m_Landscape.generate(*this);
        //wWorld.lLandscape.Fill(*this, 0, 0.999, 64);
        //wWorld.lLandscape.Fill(*this, MAT_DIRT, 1, 64);
    }
}

void Chunk::save() const
{
    std::fstream savefile;
    std::stringstream temp;
    std::string filename;

    temp << "save//" << m_X << "_" << m_Z << ".mp";
    filename = temp.str();

    savefile.open (filename, std::fstream::out | std::fstream::binary);
    if (savefile.is_open())
    {
        m_World.m_Landscape.save(*this, savefile);
        savefile.close();
    }
}

void Chunk::render(char mat, int *rendered)
{
    glPushMatrix();
    glTranslated((m_X-m_World.m_Player.m_Position.cx)*CHUNK_SIZE_XZ, 0, (m_Z-m_World.m_Player.m_Position.cz)*CHUNK_SIZE_XZ);

    GLenum mode = GL_EXECUTE;

    if (!m_Listgen)
    {
        // 1 - solid tiles
        // 2 - water tiles
        m_RenderList = glGenLists(2);
        m_Listgen = true;
    }

    int pointertorender = 0;
    if (mat == MAT_WATER)
    {
        pointertorender = 1;
    }

    if (m_NeedToRender[pointertorender] == RENDER_NEED)
    {
        mode = GL_COMPILE;
    }

    if (m_NeedToRender[pointertorender] == RENDER_MAYBE)
    {
        int prob = 1000/(*rendered*5 + 1);
        int r = rand()%1000;

        if (r <= prob)
            mode = GL_COMPILE;
    }

    // todo: GL_RENDER dont compile chunk
    Chunk *chunk = m_World.m_Player.m_pChunk;
    if (chunk)
    {
        if (chunk->m_X >= m_X - 1 && chunk->m_Z >= m_Z - 1 && chunk->m_X <= m_X + 1 && chunk->m_Z <= m_Z + 1)
        {
            m_NeedToRender[pointertorender] = RENDER_NEED;
            mode = GL_RENDER;
        }
    }

    if (mode == GL_COMPILE || mode == GL_COMPILE_AND_EXECUTE || mode == GL_RENDER)
    {
        if (mode != GL_RENDER)
        {
            glNewList(m_RenderList + pointertorender, mode);
        }

        std::list<Block *> *Tiles;
        static BlockCoord cx, cy, cz;
        static BlockInWorld temp;
        static BlockInWorld blckw;

        //1-sided tiles
        if (mat == MAT_WATER)
        {
            glTranslated(0.0, -0.95/8, 0.0); glDisable(GL_CULL_FACE);
        }
        else
        {
            glEnable(GL_CULL_FACE);
        }

        glBegin(GL_QUADS);

        for (int i = 0; i < 6; i++)
        {
            blckw = BlockInWorld(m_X, m_Z);

            if (mat == MAT_WATER) {
                Tiles = &m_pDisplayedWaterTiles[i];
            } else {
                Tiles = &m_pDisplayedTiles[i];
            }
            auto it = Tiles->begin();

            while (it != Tiles->end())
            {
                getBlockPositionByPointer(*it, &cx, &cy, &cz);

                Light::blockLight(m_World, *this, i, cx, cy, cz);
                temp = blckw + BlockInWorld(cx,cy,cz);
                drawTile(temp, *it, i);
                ++it;
            }
        }
        glEnd();
        if (mat == MAT_WATER)
        {
            glTranslated(0.0, 0.95/8, 0.0);
        }
        if (m_NeedToRender[pointertorender] == RENDER_MAYBE)
        {
            (*rendered) ++;
        }
        if (mode != GL_RENDER)
        {
            glEndList();
            m_NeedToRender[pointertorender] = RENDER_NO_NEED;
        }
    }

    if ((mode != GL_COMPILE_AND_EXECUTE)&&(mode != GL_RENDER))
    {
        glCallList(m_RenderList + pointertorender);
    }

    glPopMatrix();
}

void Chunk::drawTile(const BlockInWorld &tile_pos, Block* block, char side) const
{
    GLdouble
            x_coord = tile_pos.bx - 0.5,
            y_coord = tile_pos.by,
            z_coord = tile_pos.bz - 0.5;

    static double space = 0.0002;
    static double offset_x = 0;
    static double offset_y = 0;

    char covered = block->visible;
    char material = block->material;

    m_World.m_MaterialLib.getTextureOffsets(offset_x, offset_y, material, covered, side);

    switch(side)
    {
    case TOP:
    {
        Light::softLight(m_World, tile_pos, side, 0);
        glTexCoord2d(0.0625 - space + offset_x, 0.0 + space + offset_y);
        glVertex3d (x_coord, y_coord + 1, z_coord);

        Light::softLight(m_World, tile_pos, side, 1);
        glTexCoord2d(0.0 + space + offset_x, 0.0 + space + offset_y);
        glVertex3d (x_coord, y_coord + 1, z_coord + 1);

        Light::softLight(m_World, tile_pos, side, 2);
        glTexCoord2d(0.0 + space + offset_x, 0.0625 - space + offset_y);
        glVertex3d (x_coord + 1, y_coord + 1, z_coord + 1);

        Light::softLight(m_World, tile_pos, side, 3);
        glTexCoord2d(0.0625 - space + offset_x, 0.0625 - space + offset_y);
        glVertex3d (x_coord + 1, y_coord + 1, z_coord);
    }
        break;
    case BOTTOM:
    {
        Light::softLight(m_World, tile_pos, side, 4);
        glTexCoord2d(0.0625 - space + offset_x, 0.0 + space + offset_y);
        glVertex3d (x_coord, y_coord, z_coord);

        Light::softLight(m_World, tile_pos, side, 7);
        glTexCoord2d(0.0625 - space + offset_x, 0.0625 - space + offset_y);
        glVertex3d (x_coord + 1, y_coord, z_coord);

        Light::softLight(m_World, tile_pos, side, 6);
        glTexCoord2d(0.0 + space + offset_x, 0.0625 - space + offset_y);
        glVertex3d (x_coord + 1, y_coord, z_coord + 1);

        Light::softLight(m_World, tile_pos, side, 5);
        glTexCoord2d(0.0 + space + offset_x, 0.0 + space + offset_y);
        glVertex3d (x_coord, y_coord, z_coord + 1);
    }
        break;
    case RIGHT:
    {
        Light::softLight(m_World, tile_pos, side, 7);
        glTexCoord2d(0.0625 - space + offset_x, 0.0625 - space + offset_y);
        glVertex3d (x_coord + 1, y_coord, z_coord);

        Light::softLight(m_World, tile_pos, side, 3);
        glTexCoord2d(0.0625 - space + offset_x, 0.0 + space + offset_y);
        glVertex3d (x_coord + 1, y_coord + 1, z_coord);

        Light::softLight(m_World, tile_pos, side, 2);
        glTexCoord2d(0.0 + space + offset_x, 0.0 + space + offset_y);
        glVertex3d (x_coord + 1, y_coord + 1, z_coord + 1);

        Light::softLight(m_World, tile_pos, side, 6);
        glTexCoord2d(0.0 + space + offset_x, 0.0625 - space + offset_y);
        glVertex3d (x_coord + 1, y_coord, z_coord + 1);
    }
        break;
    case LEFT:
    {
        Light::softLight(m_World, tile_pos, side, 4);
        glTexCoord2d(0.0 + space + offset_x, 0.0625 - space + offset_y);
        glVertex3d (x_coord, y_coord, z_coord);

        Light::softLight(m_World, tile_pos, side, 5);
        glTexCoord2d(0.0625 - space + offset_x, 0.0625 - space + offset_y);
        glVertex3d (x_coord, y_coord, z_coord + 1);

        Light::softLight(m_World, tile_pos, side, 1);
        glTexCoord2d(0.0625 - space + offset_x, 0.0 + space + offset_y);
        glVertex3d (x_coord, y_coord + 1, z_coord + 1);

        Light::softLight(m_World, tile_pos, side, 0);
        glTexCoord2d(0.0 + space + offset_x, 0.0 + space + offset_y);
        glVertex3d (x_coord, y_coord + 1, z_coord);
    }
        break;
    case BACK:
    {
        Light::softLight(m_World, tile_pos, side, 5);
        glTexCoord2d(0.0 + space + offset_x, 0.0625 - space + offset_y);
        glVertex3d (x_coord, y_coord, z_coord + 1);

        Light::softLight(m_World, tile_pos, side, 6);
        glTexCoord2d(0.0625 - space + offset_x, 0.0625 - space + offset_y);
        glVertex3d (x_coord + 1, y_coord, z_coord + 1);

        Light::softLight(m_World, tile_pos, side, 2);
        glTexCoord2d(0.0625 - space + offset_x, 0.0 + space + offset_y);
        glVertex3d (x_coord + 1, y_coord + 1, z_coord + 1);

        Light::softLight(m_World, tile_pos, side, 1);
        glTexCoord2d(0.0 + space + offset_x, 0.0 + space + offset_y);
        glVertex3d (x_coord, y_coord + 1, z_coord + 1);
    }
        break;
    case FRONT:
    {
        Light::softLight(m_World, tile_pos, side, 4);
        glTexCoord2d(0.0625 - space + offset_x, 0.0625 - space + offset_y);
        glVertex3d (x_coord, y_coord, z_coord);

        Light::softLight(m_World, tile_pos, side, 0);
        glTexCoord2d(0.0625 - space + offset_x, 0.0 + space + offset_y);
        glVertex3d (x_coord, y_coord + 1, z_coord);

        Light::softLight(m_World, tile_pos, side, 3);
        glTexCoord2d(0.0 + space + offset_x, 0.0 + space + offset_y);
        glVertex3d (x_coord + 1, y_coord + 1, z_coord);

        Light::softLight(m_World, tile_pos, side, 7);
        glTexCoord2d(0.0 + space + offset_x, 0.0625 - space + offset_y);
        glVertex3d (x_coord + 1, y_coord, z_coord);
    }
        break;
    }
}
