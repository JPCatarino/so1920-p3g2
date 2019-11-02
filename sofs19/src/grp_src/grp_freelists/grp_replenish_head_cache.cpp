/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2019
 */

#include "grp_freelists.h"

#include <string.h>
#include <errno.h>
#include <iostream>

#include "core.h"
#include "dal.h"
#include "freelists.h"
#include "bin_freelists.h"

namespace sofs19
{
    void grpReplenishHeadCache(void)
    {
        soProbe(443, "%s()\n", __FUNCTION__);

        /* change the following line by your code */
        //binReplenishHeadCache();
        uint32_t block[RPB];

        SOSuperBlock* sb = soGetSuperBlockPointer(); // get superblock

        // check if head cache is not empty
        if((sb->head_cache).idx != HEAD_CACHE_SIZE){
            return; // if it's not empty return
        }

        if(sb->head_blk != NullReference){ // a reference block exists
            uint32_t bn = sb->head_blk;
            soReadDataBlock(bn, block); // read reference block into the array
            if(sb->tail_blk != sb->head_blk){ // there is more than one reference block
                uint32_t refsinblock = (RPB) - sb->head_idx; // how many refs in data block
                if(refsinblock>HEAD_CACHE_SIZE) {
                    int j = 0;
                    for (uint32_t i = sb->head_idx; i < sb->head_idx+HEAD_CACHE_SIZE; i++) { //move HEAD CACHE SIZE number of refs to the cache
                        sb->head_cache.ref[j] = block[i];
                        block[i] = NullReference; // remove the refs from the refs data block
                        j++;
                    }
                    soWriteDataBlock(bn, block);
                    sb->head_cache.idx=0;
                    sb->head_idx=sb->head_idx+HEAD_CACHE_SIZE;

                }else{ // head block doesnt have enough refs
                    int startofblock = sb->head_idx; // where the first ref is
                    int idx = HEAD_CACHE_SIZE - refsinblock;
                    for (int i = idx; i < HEAD_CACHE_SIZE; ++i) {
                        sb->head_cache.ref[i] = block[startofblock];
                        block[startofblock] = NullReference;
                        startofblock++;
                    }
                    sb->head_cache.idx=idx; //pointing to new index
                    // now we have to free the block
                    int nextblock = block[0];
                    sb->head_blk = nextblock; // head moves to the next reference data block
                    sb->head_idx = 1; // now we need to point to the first position of the new block
                    soFreeDataBlock(bn);

                }
                soSaveSuperBlock(); //saving because head block changed

            }else{ // there is only one block total
                uint32_t refsinblock = sb->tail_idx - sb->head_idx; // how many refs in the block
                if(refsinblock > HEAD_CACHE_SIZE){ // there are more refs than the cache can handle
                    int j = 0;
                    for (uint32_t i = sb->head_idx; i < sb->head_idx + HEAD_CACHE_SIZE; ++i) {
                        sb->head_cache.ref[j] = block[i];
                        block[i] = NullReference; // remove the refs from the refs data block
                        j++;
                    }
                    soWriteDataBlock(bn, block);
                    sb->head_idx = sb->head_idx + HEAD_CACHE_SIZE;
                    sb->head_cache.idx = 0;
                }else{ // watch out less refs
                    int idx = HEAD_CACHE_SIZE - refsinblock; // where to start
                    int temp = idx;
                    for (uint32_t i = sb->head_idx; i < sb->head_idx + refsinblock; ++i) {
                        sb->head_cache.ref[temp] = block[i];
                        block[i] = NullReference;
                        temp++;
                    }
                    // we have to free the block but there is no other reference block
                    sb->head_cache.idx=idx;
                    sb->head_blk = NullReference;
                    sb->tail_blk = NullReference;
                    sb->head_idx = NullReference;
                    sb->tail_idx = NullReference;
                    soWriteDataBlock(bn, block);
                    soFreeDataBlock(bn);
                }
            }


        }else{ // we have to get references from the tail cache

            uint32_t refsincache = sb->tail_cache.idx; // how many refs in tail cache

            if(refsincache >= HEAD_CACHE_SIZE) { // if more than Head cache size refs in tail cache
                uint32_t tailidx = sb->tail_cache.idx - 1;
                for (int i = HEAD_CACHE_SIZE - 1; i >= 0; i--) {
                    sb->head_cache.ref[i] = sb->tail_cache.ref[tailidx];
                    sb->tail_cache.ref[tailidx] = NullReference;
                    tailidx--;
                }
                sb->tail_cache.idx = sb->tail_cache.idx - HEAD_CACHE_SIZE;
                sb->head_cache.idx = 0;
            }else{
                uint32_t idx = HEAD_CACHE_SIZE - refsincache;
                uint32_t tailidx = refsincache - 1;
                for(uint32_t i = HEAD_CACHE_SIZE - 1; i >= idx; i--){
                    sb->head_cache.ref[i] = sb->tail_cache.ref[tailidx];
                    sb->tail_cache.ref[tailidx] = NullReference;
                    tailidx--;
                }
                sb->head_cache.idx = idx;
                sb->tail_cache.idx = 0;
            }
        }
        soSaveSuperBlock();
    }

};

