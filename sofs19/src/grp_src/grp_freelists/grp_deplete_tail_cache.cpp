/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2019
 */

#include "grp_freelists.h"

#include "core.h"
#include "dal.h"
#include "freelists.h"
#include "bin_freelists.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
using namespace std;

namespace sofs19
{
    /* only fill the current block to its end */
    void grpDepleteTailCache(void)
    {
        soProbe(444, "%s()\n", __FUNCTION__);
        
        SOSuperBlock* soSB = soGetSuperBlockPointer();      // Pointer to superblock object

        if(soSB->tail_cache.idx == TAIL_CACHE_SIZE){        // Check if tail_cache is full and ready for deplete
            uint32_t trBlockNumber = soSB->tail_blk;
            uint32_t trRef = soSB->tail_idx;
            uint32_t tCacheBU [BlockSize];

            // If there's not a trail ref db, create one and write there
            if(trBlockNumber == NullReference){
                trRef = soSB->tail_cache.idx;               // First empty reference of the tail cache.
                memcpy(tCacheBU, soSB->tail_cache.ref, trRef*sizeof(uint32_t));  // Copies references of the tail cache to new block   
                memset(tCacheBU + trRef + 1, NullReference, BlockSize - (trRef*sizeof(uint32_t)));  // Fills rest of new block with null references
                uint32_t nHTRBlock = soAllocDataBlock();    // Allocates new block to become head and tail ref datablock
                soWriteDataBlock(nHTRBlock, &tCacheBU);
                memset(soSB->tail_cache.ref, NullReference, trRef*sizeof(uint32_t));
                soSB->tail_cache.idx = 0;                   // Cache is empty
                soSB->head_blk = nHTRBlock;                 // Head ref block is equal the alloc block, and tail ref is equal head ref
                soSB->tail_blk = soSB->head_blk;
                soSB->tail_idx = trRef + 1;                 // next empty position is cache size + 1
                soSB->head_idx = 1;

            }
            else{
                // If tail ref block is full
                if(trRef == RPB){
                    trRef = soSB->tail_cache.idx;
                    memcpy(tCacheBU, soSB->tail_cache.ref, trRef*sizeof(uint32_t));
                    memset(tCacheBU + trRef + 1, NullReference, BlockSize - (trRef*sizeof(uint32_t)));
                    uint32_t nTRBlock = soAllocDataBlock();
                    soWriteDataBlock(nTRBlock, &tCacheBU);
                    memset(soSB->tail_cache.ref, NullReference, trRef*sizeof(uint32_t));
                    soSB->tail_cache.idx = 0;
                    uint32_t prevRefBlock[BlockSize];
                    soReadDataBlock(trBlockNumber, &prevRefBlock);
                    memset(prevRefBlock, nTRBlock, sizeof(uint32_t));      // sets reference position of previous tail ref datablock to new tail ref datablock
                    soWriteDataBlock(trBlockNumber, &prevRefBlock);
                    soSB->tail_blk = nTRBlock;                             // assigns new block as the new tail reference block
                    soSB->tail_idx = trRef + 1;
                }
                // There's still space on the tail ref block
                else{
                    soReadDataBlock(trBlockNumber,&tCacheBU);
                    uint32_t freeRefPos = RPB - trRef;
                    // Check if there's more free ref than cache
                    if(soSB->tail_cache.idx < freeRefPos){
                        freeRefPos = soSB->tail_cache.idx;
                    }
                    memcpy(tCacheBU + trRef, soSB->tail_cache.ref, freeRefPos*sizeof(uint32_t));        // Copies from cache all possible positions
                    soWriteDataBlock(trBlockNumber, &tCacheBU);
                    uint32_t depletedBlocks = soSB->tail_cache.idx - freeRefPos;                        // calculate how many blocks were left over
                    memcpy(soSB->tail_cache.ref, soSB->tail_cache.ref + freeRefPos, depletedBlocks*sizeof(uint32_t));   // Moves the blocks up
                    memset(soSB->tail_cache.ref + depletedBlocks, NullReference, freeRefPos*sizeof(uint32_t));          // Changes moved blocks to null ref
                    soSB->tail_cache.idx = depletedBlocks;
                    soSB->tail_idx += freeRefPos;
                }
            }
            soSaveSuperBlock();
        }    
        return;    
    }
};