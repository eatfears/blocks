#include <process.h>
#include "Blocks_Definitions.h"
#include "World.h"
#include "Threads.h"
#include "Light.h"
#include <math.h>

World::World()
	:player(*this)
{
	Chunks = new std::list<Chunk *>[HASH_SIZE];

	parget = CreateEvent(NULL, false, false, NULL);
	parget2 = CreateEvent(NULL, false, false, NULL);
	mutex = CreateMutex(NULL, false, NULL);
	semaphore = CreateSemaphore(NULL, 4, 4, NULL);
	SoftLight = true;

	SkyBright = 1.0f;
	LightToRefresh = true;
}

World::~World()
{
	for(int i = 0; i < HASH_SIZE; i++) {
		Chunks[i].clear();
	}
	delete[] Chunks;
}

void World::BuildWorld()
{
	Param par = {0, 0, this};
	_beginthread(LoadNGenerate, 0, &par);

	WaitForSingleObject(parget2, INFINITE);
	ResetEvent(parget2);
}

Chunk* World::GetChunkByPosition(ChunkCoord Cx, ChunkCoord Cz)
{
	unsigned long bin = Hash(Cx, Cz);
	auto chunk = Chunks[bin].begin();

	while(chunk != Chunks[bin].end()) {
		if(((*chunk)->x == Cx)&&((*chunk)->z == Cz)) break;
		++chunk;
	}
	if(chunk == Chunks[bin].end()) {
		return NULL;
	}

	return *chunk;
}

void World::DrawLoadedBlocksFinish(Chunk &chunk)
{
	int index = 0;
 	DWORD dwWaitResult;
 	dwWaitResult = WaitForSingleObject(chunk.mutex, INFINITE);
	BlockCoord chnkx, chnky, chnkz;
	BlockInWorld pos;

	while(index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y) {
		chunk.GetBlockPositionByPointer(chunk.bBlocks + index, &chnkx, &chnky, &chnkz);

		if((chnkx > 0)&&(chnkx < CHUNK_SIZE_XZ - 1)&&(chnkz > 0)&&(chnkz < CHUNK_SIZE_XZ - 1)) {
			index++;
			continue;
		}

		BlockInWorld pos(chunk.x, chunk.z, chnkx, chnky, chnkz);

		if(chunk.bBlocks[index].cMaterial != MAT_NO) {
			Chunk *TempChunk;
			int TempIndex;

			if(chunk.bBlocks[index].cMaterial == MAT_WATER) {
				if(!FindBlock(pos.getSide(TOP), &TempChunk, &TempIndex)) ShowTile(&chunk, index, TOP);
				if(!FindBlock(pos.getSide(BOTTOM), &TempChunk, &TempIndex)) { if(TempChunk) ShowTile(&chunk, index, BOTTOM); }
				if(!FindBlock(pos.getSide(RIGHT), &TempChunk, &TempIndex)) { if(TempChunk) ShowTile(&chunk, index, RIGHT); }
				if(!FindBlock(pos.getSide(LEFT), &TempChunk, &TempIndex)) { if(TempChunk) ShowTile(&chunk, index, LEFT); }
				if(!FindBlock(pos.getSide(BACK), &TempChunk, &TempIndex)) { if(TempChunk) ShowTile(&chunk, index, BACK); }
				if(!FindBlock(pos.getSide(FRONT), &TempChunk, &TempIndex)) { if(TempChunk) ShowTile(&chunk, index, FRONT); }
			} else {
				if(!FindBlock(pos.getSide(TOP), &TempChunk, &TempIndex)||(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) {
					ShowTile(&chunk, index, TOP);}
				if(!FindBlock(pos.getSide(BOTTOM), &TempChunk, &TempIndex)||(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) {
					if(TempChunk) ShowTile(&chunk, index, BOTTOM);}
				if(!FindBlock(pos.getSide(RIGHT), &TempChunk, &TempIndex)||(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) {
					if(TempChunk) ShowTile(&chunk, index, RIGHT);}
				if(!FindBlock(pos.getSide(LEFT), &TempChunk, &TempIndex)||(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) {
					if(TempChunk) ShowTile(&chunk, index, LEFT);}
				if(!FindBlock(pos.getSide(BACK), &TempChunk, &TempIndex)||(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) {
					if(TempChunk) ShowTile(&chunk, index, BACK);}
				if(!FindBlock(pos.getSide(FRONT), &TempChunk, &TempIndex)||(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) {
					if(TempChunk) ShowTile(&chunk, index, FRONT);}
			}
		}
		index++;
	}
	ReleaseMutex(chunk.mutex);

	// draw boundary tiles
	index = 0;
	while(index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y) {
		chunk.GetBlockPositionByPointer(chunk.bBlocks + index, &chnkx, &chnky, &chnkz);

		if((chnkx > 0)&&(chnkx < CHUNK_SIZE_XZ - 1)&&(chnkz > 0)&&(chnkz < CHUNK_SIZE_XZ - 1)) {
			index++;
			continue;
		}

		BlockInWorld pos(chunk.x, chunk.z, chnkx, chnky, chnkz);

		if((chunk.bBlocks[index].cMaterial == MAT_NO)||(chunk.bBlocks[index].cMaterial == MAT_WATER)) {
			Chunk *tempLoc;
			Chunk *tCurLoc = &chunk;
			int tempIndex;
			if(FindBlock(pos.getSide(RIGHT), &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if(tempLoc->bBlocks != tCurLoc->bBlocks)
				ShowTile(&*tempLoc, tempIndex, LEFT);
			if(FindBlock(pos.getSide(LEFT), &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if(tempLoc->bBlocks != tCurLoc->bBlocks)
				ShowTile(&*tempLoc, tempIndex, RIGHT);
			if(FindBlock(pos.getSide(BACK), &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if(tempLoc->bBlocks != tCurLoc->bBlocks)
				ShowTile(&*tempLoc, tempIndex, FRONT);
			if(FindBlock(pos.getSide(FRONT), &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if(tempLoc->bBlocks != tCurLoc->bBlocks)
				ShowTile(&*tempLoc, tempIndex, BACK);
		}
		index++;
	}
}

void World::DrawUnLoadedBlocks(ChunkCoord Cx, ChunkCoord Cz)
{
	int index = 0;

	BlockCoord chnkx, chnky, chnkz;

	while(index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y) {
		Chunk::GetBlockPositionByIndex(index, &chnkx, &chnky, &chnkz);

		if((chnkx > 0)&&(chnkx < CHUNK_SIZE_XZ - 1)&&(chnkz > 0)&&(chnkz < CHUNK_SIZE_XZ - 1)) {
			index++;
			continue;
		}

		BlockInWorld pos(Cx, Cz, chnkx, chnky, chnkz);

		Chunk *TempChunk;
		int TempIndex;
		if(FindBlock(pos.getSide(RIGHT), &TempChunk, &TempIndex)) HideTile(&*TempChunk, TempIndex, LEFT);
		if(FindBlock(pos.getSide(LEFT), &TempChunk, &TempIndex)) HideTile(&*TempChunk, TempIndex, RIGHT);
		if(FindBlock(pos.getSide(BACK), &TempChunk, &TempIndex)) HideTile(&*TempChunk, TempIndex, FRONT);
		if(FindBlock(pos.getSide(FRONT), &TempChunk, &TempIndex)) HideTile(&*TempChunk, TempIndex, BACK);
		index++;
	}
}

int World::AddBlock(BlockInWorld pos, char mat, bool show)
{
	if((pos.by < 0)||(pos.by >= CHUNK_SIZE_Y)) return 0;
	if(FindBlock(pos)) return 0;

	Chunk *chunk = GetChunkByPosition(pos.cx, pos.cz);

	if(chunk == NULL) return 0;

	DWORD dwWaitResult;
	dwWaitResult = WaitForSingleObject(chunk->mutex, INFINITE);
	int index = chunk->AddBlock(pos.bx, pos.by, pos.bz, mat);
	ReleaseMutex(chunk->mutex);

	Chunk *TempChunk = 0;
	int TempIndex;

	if(chunk->bBlocks[index].cMaterial == MAT_WATER) {
		if(!FindBlock(pos.getSide(TOP), &TempChunk, &TempIndex)) ShowTile(chunk, index, TOP);
		else if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(TempChunk, TempIndex, BOTTOM);
		if(!FindBlock(pos.getSide(BOTTOM), &TempChunk, &TempIndex)) {if(TempChunk) ShowTile(chunk, index, BOTTOM);}
		else if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(TempChunk, TempIndex, TOP);
		if(!FindBlock(pos.getSide(RIGHT), &TempChunk, &TempIndex)) {if(TempChunk) ShowTile(chunk, index, RIGHT);}
		else if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(TempChunk, TempIndex, LEFT);
		if(!FindBlock(pos.getSide(LEFT), &TempChunk, &TempIndex)) {if(TempChunk) ShowTile(chunk, index, LEFT);}
		else if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(TempChunk, TempIndex, RIGHT);
		if(!FindBlock(pos.getSide(BACK), &TempChunk, &TempIndex)) {if(TempChunk) ShowTile(chunk, index, BACK);}
		else if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(TempChunk, TempIndex, FRONT);
		if(!FindBlock(pos.getSide(FRONT), &TempChunk, &TempIndex)) {if(TempChunk) ShowTile(chunk, index, FRONT);}
		else if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(TempChunk, TempIndex, BACK);
	} else {
		if(!FindBlock(pos.getSide(TOP), &TempChunk, &TempIndex))
			ShowTile(chunk, index, TOP);
		else {HideTile(TempChunk, TempIndex, BOTTOM); if (TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) ShowTile(chunk, index, TOP);}
		if(!FindBlock(pos.getSide(BOTTOM), &TempChunk, &TempIndex)) {
			if(TempChunk) ShowTile(chunk, index, BOTTOM);}
		else {if ((TempChunk)&&(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) ShowTile(chunk, index, BOTTOM); else HideTile(TempChunk, TempIndex, TOP); }
		if(!FindBlock(pos.getSide(RIGHT), &TempChunk, &TempIndex)) {
			if(TempChunk) ShowTile(chunk, index, RIGHT);}
		else {HideTile(TempChunk, TempIndex, LEFT); if ((TempChunk)&&(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) ShowTile(chunk, index, RIGHT);}
		if(!FindBlock(pos.getSide(LEFT), &TempChunk, &TempIndex)) {
			if(TempChunk) ShowTile(chunk, index, LEFT);}
		else {HideTile(TempChunk, TempIndex, RIGHT); if ((TempChunk)&&(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) ShowTile(chunk, index, LEFT);}
		if(!FindBlock(pos.getSide(BACK), &TempChunk, &TempIndex)) {
			if(TempChunk) ShowTile(chunk, index, BACK);}
		else {HideTile(TempChunk, TempIndex, FRONT); if ((TempChunk)&&(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) ShowTile(chunk, index, BACK);}
		if(!FindBlock(pos.getSide(FRONT), &TempChunk, &TempIndex)) {
			if(TempChunk) ShowTile(chunk, index, FRONT);}
		else {HideTile(TempChunk, TempIndex, BACK); if ((TempChunk)&&(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) ShowTile(chunk, index, FRONT);}
	}

	return 1;
}

int World::RemoveBlock(BlockInWorld pos, bool show)
{
	if((pos.by < 0)||(pos.by >= CHUNK_SIZE_Y)) return 0;
	if(!FindBlock(pos)) return 0;

	Chunk *chunk = GetChunkByPosition(pos.cx, pos.cz);
	int index;

	if(chunk == NULL) return 0;

	index = chunk->GetIndexByPosition(pos.bx, pos.by, pos.bz);
	Chunk *TempChunk = 0;
	int TempIndex;

	if(!FindBlock(pos + BlockInChunk(0,1,0), &TempChunk, &TempIndex)||chunk->bBlocks[index].cMaterial == MAT_WATER) HideTile(chunk, index, TOP);
	else {ShowTile(TempChunk, TempIndex, BOTTOM); if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(chunk, index, TOP);}
	if(!FindBlock(pos + BlockInChunk(0,-1,0), &TempChunk, &TempIndex)) HideTile(chunk, index, BOTTOM);
	else {ShowTile(TempChunk, TempIndex, TOP); if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(chunk, index, BOTTOM);}
	if(!FindBlock(pos + BlockInChunk(1,0,0), &TempChunk, &TempIndex)) HideTile(chunk, index, RIGHT);
	else {ShowTile(TempChunk, TempIndex, LEFT); if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(chunk, index, RIGHT);}
	if(!FindBlock(pos + BlockInChunk(-1,0,0), &TempChunk, &TempIndex)) HideTile(chunk, index, LEFT);
	else {ShowTile(TempChunk, TempIndex, RIGHT); if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(chunk, index, LEFT);}
	if(!FindBlock(pos + BlockInChunk(0,0,1), &TempChunk, &TempIndex)) HideTile(chunk, index, BACK);
	else {ShowTile(TempChunk, TempIndex, FRONT); if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(chunk, index, BACK);}
	if(!FindBlock(pos + BlockInChunk(0,0,-1), &TempChunk, &TempIndex)) HideTile(chunk, index, FRONT);
	else {ShowTile(TempChunk, TempIndex, BACK); if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(chunk, index, FRONT);}

	DWORD dwWaitResult;
	dwWaitResult = WaitForSingleObject(chunk->mutex, INFINITE);
	chunk->RemoveBlock(pos.bx, pos.by, pos.bz);
	ReleaseMutex(chunk->mutex);

	return 1;
}

void World::ShowTile(Chunk *chunk, int index, char side)
{
	if(!(chunk->bBlocks[index].bVisible & (1 << side))) {
		DWORD dwWaitResult;
		dwWaitResult = WaitForSingleObject(chunk->mutex, INFINITE);

		chunk->ShowTile(chunk->bBlocks + index, side);

		chunk->NeedToRender[0] = RENDER_NEED;
		chunk->NeedToRender[1] = RENDER_NEED;
		ReleaseMutex(chunk->mutex);
	}
}

void World::HideTile(Chunk *chunk, int index, char side)
{
	if(chunk->bBlocks[index].bVisible & (1 << side)) {
		DWORD dwWaitResult;
		dwWaitResult = WaitForSingleObject(chunk->mutex, INFINITE);

		chunk->HideTile(chunk->bBlocks + index, side);

		chunk->NeedToRender[0] = RENDER_NEED;
		chunk->NeedToRender[1] = RENDER_NEED;
		ReleaseMutex(chunk->mutex);
	}
}

int World::FindBlock(BlockInWorld pos, Chunk **chunk, int *index)
{
	if((pos.by < 0)||(pos.by >= CHUNK_SIZE_Y)) { *chunk = NULL; *index = 0; return 0;}
	(*chunk) = GetChunkByPosition(pos.cx, pos.cz);
	if((*chunk) == NULL) { *index = 0; return 0;}
	*index = pos.bx*CHUNK_SIZE_XZ + pos.bz + pos.by*CHUNK_SIZE_XZ*CHUNK_SIZE_XZ;
	if((*chunk)->GetBlockMaterial(pos.bx, pos.by, pos.bz) == MAT_NO)
		return 0;
	return 1;
}

int World::FindBlock(BlockInWorld pos)
{
	if(pos.by < 0) return 0;
	Chunk *chunk = GetChunkByPosition(pos.cx, pos.cz);
	if(chunk == NULL) return 0;
	BlockCoord chnkx, chnky, chnkz;
	if(chunk->GetBlockMaterial(pos.bx, pos.by, pos.bz) == MAT_NO)
		return 0;
	return 1;
}

void World::LoadChunk(ChunkCoord x, ChunkCoord z)
{
	Param par = {x, z, this};
	HANDLE hThread;
	hThread = (HANDLE) _beginthread(LoadChunkThread, 0, &par);
	WaitForSingleObject(parget, INFINITE);
	ResetEvent(parget);
}

void World::UnLoadChunk(ChunkCoord x, ChunkCoord z)
{
	Param par = {x, z, this};
	HANDLE hThread;
	hThread = (HANDLE) _beginthread(UnLoadChunkThread, 0, &par);
	WaitForSingleObject(parget, INFINITE);
	ResetEvent(parget);
}

void World::UpdateLight(Chunk& chunk)
{
	Chunk *ChunkArray[5][5];
	ChunkArray[2][2] = &chunk;

	for(int i = 0; i < 5; i++) {
		for(int j = 0; j < 5; j++) {
			if((i!=2)||(j!=2)) {
				if((i == 0)&&(j == 0) || (i == 4)&&(j == 0) || (i == 0)&&(j == 4) || (i == 4)&&(j == 4)) {
					ChunkArray[i][j] = NULL;
				} else {
					ChunkArray[i][j] = GetChunkByPosition((chunk.x + i - 2), (chunk.z + j - 2));
				}
			}
		}
	}

	Light lightfill(ChunkArray, true);
	lightfill.UpdateLight();
	lightfill.skylight = false;
	lightfill.UpdateLight();

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			if(ChunkArray[i][j]) {
				if(ChunkArray[i][j]->NeedToRender[0] != RENDER_NEED)
					ChunkArray[i][j]->NeedToRender[0] = RENDER_MAYBE;
				if(ChunkArray[i][j]->NeedToRender[1] != RENDER_NEED)
					ChunkArray[i][j]->NeedToRender[1] = RENDER_MAYBE;
			}
		}
	}
}

void World::SaveChunks()
{
	for(int i = 0; i < HASH_SIZE; i++) {
		auto chunk = Chunks[i].begin();

		while(chunk != Chunks[i].end()) {
			(*chunk)->Save();
			++chunk;
		}
	}
}
