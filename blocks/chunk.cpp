#include "chunk.h"

#include <fstream>
#include <sstream>

#include "world.h"
#include "light.h"


Chunk::Chunk(ChunkCoord x, ChunkCoord z, World& wrld)
    : wWorld(wrld)
{
    Chunk::x = x; Chunk::z = z;

    bBlocks = new Block[CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y];
    SkyLight = new char[CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y];
    TorchLight = new char[CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y];

    DisplayedTiles = new std::list<Block *>[6];
    DisplayedWaterTiles = new std::list<Block *>[6];

    for (int i = 0; i < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y; i++)
    {
        SkyLight[i] = 0;
        TorchLight[i] = 0;
        bBlocks[i].cMaterial = MAT_NO;
        for (int side = 0; side < 6; side++)
        {
            bBlocks[i].bVisible &= ~(1 << side);
        }
    }
    NeedToRender[0] = RENDER_NO_NEED;
    NeedToRender[1] = RENDER_NO_NEED;

    listgen = false;
    m_LightToUpdate = true;

    //	mutex = CreateMutex(NULL, false, NULL);
    //	loadmutex = CreateMutex(NULL, false, NULL);
}

Chunk::~Chunk()
{
    delete[] bBlocks;
    delete[] SkyLight;
    delete[] TorchLight;

    delete[] DisplayedTiles;
    delete[] DisplayedWaterTiles;

    glDeleteLists(RenderList, 2);
}

int Chunk::addBlock(BlockCoord x, BlockCoord y, BlockCoord z, char mat)
{
    x = x%CHUNK_SIZE_XZ;
    y = y%CHUNK_SIZE_Y;
    z = z%CHUNK_SIZE_XZ;
    if ((getBlockMaterial(x, y, z) != MAT_NO)||(getBlockMaterial(x, y, z) == -1)) return -1;

    BlockCoord index = setBlockMaterial(x, y, z, mat);
    bBlocks[index].bVisible = 0;
    m_LightToUpdate = true;

    return index;
}

int Chunk::removeBlock(BlockCoord x, BlockCoord y, BlockCoord z)
{
    if ((getBlockMaterial(x, y, z) == MAT_NO)||(getBlockMaterial(x, y, z) == -1)) return -1;

    BlockCoord index = setBlockMaterial(x, y, z, MAT_NO);
    bBlocks[index].bVisible = 0;
    m_LightToUpdate = true;

    return index;
}

void Chunk::showTile(Block *bBlock, char side)
{
    if (!bBlock) return;
    if (bBlock->cMaterial == MAT_NO) return;
    if (bBlock->bVisible & (1 << side)) return;

    std::list<Block *> *Tiles;
    if (bBlock->cMaterial == MAT_WATER)
    {
        Tiles = &DisplayedWaterTiles[side];
    }
    else
    {
        Tiles = &DisplayedTiles[side];
    }
    Tiles->push_back(bBlock);

    bBlock->bVisible |= (1 << side);
}

void Chunk::hideTile(Block *bBlock, char side)
{
    if (!bBlock) return;
    if (bBlock->cMaterial == MAT_NO) return;
    if (!(bBlock->bVisible & (1 << side))) return;

    std::list<Block *> *Tiles;
    if (bBlock->cMaterial == MAT_WATER) {
        Tiles = &DisplayedWaterTiles[side];
    }
    else
    {
        Tiles = &DisplayedTiles[side];
    }
    auto it = Tiles->begin();

    while (it != Tiles->end())
    {
        if (*it == bBlock) break;
        ++it;
    }
    if (it == Tiles->end()) return;

    (*it)->bVisible &= ~(1 << side);
    Tiles->erase(it);
}

char Chunk::getBlockMaterial(BlockCoord x, BlockCoord y, BlockCoord z)
{
    if ((x < 0)||(z < 0)||(y < 0)||(x >= CHUNK_SIZE_XZ)||(z >= CHUNK_SIZE_XZ)||(y >= CHUNK_SIZE_Y))
    {
        return -1;
    }
    return bBlocks[x*CHUNK_SIZE_XZ + z + y*CHUNK_SIZE_XZ*CHUNK_SIZE_XZ].cMaterial;
}

int Chunk::setBlockMaterial(BlockCoord x, BlockCoord y, BlockCoord z, char cMat)
{
    if ((x >= CHUNK_SIZE_XZ)||(z >= CHUNK_SIZE_XZ)||(y >= CHUNK_SIZE_Y))
    {
        return -1;
    }
    int index = getIndexByPosition(x, y, z);
    bBlocks[index].cMaterial = cMat;
    return index;
}

int Chunk::getBlockPositionByPointer(Block *tCurrentBlock, BlockCoord *x, BlockCoord *y, BlockCoord *z)
const
{
    int t = tCurrentBlock - bBlocks;
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
    index/=CHUNK_SIZE_XZ;
    *x = index%CHUNK_SIZE_XZ;
    index/=CHUNK_SIZE_XZ;
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
    BlockCoord xx, yy, zz;

    while (index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
    {
        getBlockPositionByPointer(bBlocks + index, &xx, &yy, &zz);

        if (bBlocks[index].cMaterial != MAT_NO)
        {
            if (bBlocks[index].cMaterial == MAT_WATER)
            {
                if ((getBlockMaterial(xx, yy + 1, zz) == MAT_NO)||(yy == CHUNK_SIZE_Y - 1)) showTile(bBlocks + index, TOP);
                if (getBlockMaterial(xx, yy - 1, zz) == MAT_NO) showTile(bBlocks + index, BOTTOM);
                if (getBlockMaterial(xx + 1, yy, zz) == MAT_NO) showTile(bBlocks + index, RIGHT);
                if (getBlockMaterial(xx - 1, yy, zz) == MAT_NO) showTile(bBlocks + index, LEFT);
                if (getBlockMaterial(xx, yy, zz + 1) == MAT_NO) showTile(bBlocks + index, BACK);
                if (getBlockMaterial(xx, yy, zz - 1) == MAT_NO) showTile(bBlocks + index, FRONT);
            }
            else
            {
                if ((getBlockMaterial(xx, yy + 1, zz) == MAT_NO)||(getBlockMaterial(xx, yy + 1, zz) == MAT_WATER)||(yy == CHUNK_SIZE_Y - 1))
                    showTile(bBlocks + index, TOP);
                if ((getBlockMaterial(xx, yy - 1, zz) == MAT_NO)||(getBlockMaterial(xx, yy - 1, zz) == MAT_WATER))
                    showTile(bBlocks + index, BOTTOM);
                if ((getBlockMaterial(xx + 1, yy, zz) == MAT_NO)||(getBlockMaterial(xx + 1, yy, zz) == MAT_WATER))
                    showTile(bBlocks + index, RIGHT);
                if ((getBlockMaterial(xx - 1, yy, zz) == MAT_NO)||(getBlockMaterial(xx - 1, yy, zz) == MAT_WATER))
                    showTile(bBlocks + index, LEFT);
                if ((getBlockMaterial(xx, yy, zz + 1) == MAT_NO)||(getBlockMaterial(xx, yy, zz + 1) == MAT_WATER))
                    showTile(bBlocks + index, BACK);
                if ((getBlockMaterial(xx, yy, zz - 1) == MAT_NO)||(getBlockMaterial(xx, yy, zz - 1) == MAT_WATER))
                    showTile(bBlocks + index, FRONT);
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

    temp << "save//" << x << "_" << z << ".mp";
    filename = temp.str();

    savefile.open (filename, std::fstream::in | std::fstream::binary);
    //	if (savefile.is_open()
    //  {
    //		WaitForSingleObject(loadmutex, INFINITE);
    //		loaded = wWorld.lLandscape.Load(*this, savefile);
    //		ReleaseMutex(loadmutex);
    //		savefile.close();
    //	}

    if (!loaded)
    {
        wWorld.m_Landscape.generate(*this);
        //wWorld.lLandscape.Fill(*this, 0, 0.999, 64);
        //wWorld.lLandscape.Fill(*this, MAT_DIRT, 1, 64);
    }
}

void Chunk::save()
{
    std::fstream savefile;
    std::stringstream temp;
    std::string filename;

    temp << "save//" << x << "_" << z << ".mp";
    filename = temp.str();

    savefile.open (filename, std::fstream::out | std::fstream::binary);
    if (savefile.is_open())
    {
        wWorld.m_Landscape.save(*this, savefile);
        savefile.close();
    }
}

void Chunk::render(char mat, int *rendered)
{
    glPushMatrix();
    glTranslated((x-wWorld.m_Player.position.cx)*CHUNK_SIZE_XZ, 0, (z-wWorld.m_Player.position.cz)*CHUNK_SIZE_XZ);

    GLenum mode = GL_EXECUTE;

    if (!listgen)
    {
        // 1 - solid tiles
        // 2 - water tiles
        RenderList = glGenLists(2);
        listgen = true;
    }

    int pointertorender = 0;
    if (mat == MAT_WATER)
    {
        pointertorender = 1;
    }

    if (NeedToRender[pointertorender] == RENDER_NEED)
    {
        mode = GL_COMPILE;
    }

    if (NeedToRender[pointertorender] == RENDER_MAYBE)
    {
        int prob = 1000/(*rendered*5 + 1);
        int r = rand()%1000;

        if (r <= prob)
            mode = GL_COMPILE;
    }

    // todo: GL_RENDER dont compile chunk
    Chunk *chunk = wWorld.m_Player.chunk;
    if (chunk)
    {
        if (chunk->x >= x - 1 && chunk->z >= z - 1 && chunk->x <= x + 1 && chunk->z <= z + 1)
        {
            NeedToRender[pointertorender] = RENDER_NEED;
            mode = GL_RENDER;
        }
    }

    if (mode == GL_COMPILE || mode == GL_COMPILE_AND_EXECUTE || mode == GL_RENDER)
    {
        if (mode != GL_RENDER)
        {
            glNewList(RenderList + pointertorender, mode);
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
            blckw = BlockInWorld(x, z);

            if (mat == MAT_WATER) {
                Tiles = &DisplayedWaterTiles[i];
            } else {
                Tiles = &DisplayedTiles[i];
            }
            auto it = Tiles->begin();

            while (it != Tiles->end())
            {
                getBlockPositionByPointer(*it, &cx, &cy, &cz);

                Light::blockLight(wWorld, *this, i, cx, cy, cz);
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
        if (NeedToRender[pointertorender] == RENDER_MAYBE)
        {
            (*rendered) ++;
        }
        if (mode != GL_RENDER)
        {
            glEndList();
            NeedToRender[pointertorender] = RENDER_NO_NEED;
        }
    }

    if ((mode != GL_COMPILE_AND_EXECUTE)&&(mode != GL_RENDER))
    {
        glCallList(RenderList + pointertorender);
    }

    glPopMatrix();
}

void Chunk::drawTile(BlockInWorld tilePos, Block* block, char side)
{
    GLdouble
            dXcoord = tilePos.bx - 0.5,
            dYcoord = tilePos.by,
            dZcoord = tilePos.bz - 0.5;

    static double space = 0.0002;
    static double offsetx = 0;
    static double offsety = 0;

    char covered = block->bVisible;
    char material = block->cMaterial;

    wWorld.MaterialLib.getTextureOffsets(offsetx, offsety, material, covered, side);

    switch(side)
    {
    case TOP:
    {
        Light::softLight(wWorld, tilePos, side, 0);
        glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
        glVertex3d (dXcoord, dYcoord + 1, dZcoord);

        Light::softLight(wWorld, tilePos, side, 1);
        glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
        glVertex3d (dXcoord, dYcoord + 1, dZcoord + 1);

        Light::softLight(wWorld, tilePos, side, 2);
        glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
        glVertex3d (dXcoord + 1, dYcoord + 1, dZcoord + 1);

        Light::softLight(wWorld, tilePos, side, 3);
        glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
        glVertex3d (dXcoord + 1, dYcoord + 1, dZcoord);
    }
        break;
    case BOTTOM:
    {
        Light::softLight(wWorld, tilePos, side, 4);
        glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
        glVertex3d (dXcoord, dYcoord, dZcoord);

        Light::softLight(wWorld, tilePos, side, 7);
        glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
        glVertex3d (dXcoord + 1, dYcoord, dZcoord);

        Light::softLight(wWorld, tilePos, side, 6);
        glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
        glVertex3d (dXcoord + 1, dYcoord, dZcoord + 1);

        Light::softLight(wWorld, tilePos, side, 5);
        glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
        glVertex3d (dXcoord, dYcoord, dZcoord + 1);
    }
        break;
    case RIGHT:
    {
        Light::softLight(wWorld, tilePos, side, 7);
        glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
        glVertex3d (dXcoord + 1, dYcoord, dZcoord);

        Light::softLight(wWorld, tilePos, side, 3);
        glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
        glVertex3d (dXcoord + 1, dYcoord + 1, dZcoord);

        Light::softLight(wWorld, tilePos, side, 2);
        glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
        glVertex3d (dXcoord + 1, dYcoord + 1, dZcoord + 1);

        Light::softLight(wWorld, tilePos, side, 6);
        glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
        glVertex3d (dXcoord + 1, dYcoord, dZcoord + 1);
    }
        break;
    case LEFT:
    {
        Light::softLight(wWorld, tilePos, side, 4);
        glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
        glVertex3d (dXcoord, dYcoord, dZcoord);

        Light::softLight(wWorld, tilePos, side, 5);
        glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
        glVertex3d (dXcoord, dYcoord, dZcoord + 1);

        Light::softLight(wWorld, tilePos, side, 1);
        glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
        glVertex3d (dXcoord, dYcoord + 1, dZcoord + 1);

        Light::softLight(wWorld, tilePos, side, 0);
        glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
        glVertex3d (dXcoord, dYcoord + 1, dZcoord);
    }
        break;
    case BACK:
    {
        Light::softLight(wWorld, tilePos, side, 5);
        glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
        glVertex3d (dXcoord, dYcoord, dZcoord + 1);

        Light::softLight(wWorld, tilePos, side, 6);
        glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
        glVertex3d (dXcoord + 1, dYcoord, dZcoord + 1);

        Light::softLight(wWorld, tilePos, side, 2);
        glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
        glVertex3d (dXcoord + 1, dYcoord + 1, dZcoord + 1);

        Light::softLight(wWorld, tilePos, side, 1);
        glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
        glVertex3d (dXcoord, dYcoord + 1, dZcoord + 1);
    }
        break;
    case FRONT:
    {
        Light::softLight(wWorld, tilePos, side, 4);
        glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
        glVertex3d (dXcoord, dYcoord, dZcoord);

        Light::softLight(wWorld, tilePos, side, 0);
        glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
        glVertex3d (dXcoord, dYcoord + 1, dZcoord);

        Light::softLight(wWorld, tilePos, side, 3);
        glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
        glVertex3d (dXcoord + 1, dYcoord + 1, dZcoord);

        Light::softLight(wWorld, tilePos, side, 7);
        glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
        glVertex3d (dXcoord + 1, dYcoord, dZcoord);
    }
        break;
    }
}
