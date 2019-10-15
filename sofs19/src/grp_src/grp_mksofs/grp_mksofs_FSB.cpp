#include "grp_mksofs.h"
#include "rawdisk.h"
#include "core.h"
#include "bin_mksofs.h"
#include <string.h>
#include <inttypes.h>
#include <math.h>

namespace sofs19
{
    void grpFillSuperBlock(const char *name, uint32_t ntotal, uint32_t itotal, uint32_t nbref)
    {
        soProbe(602, "%s(%s, %u, %u, %u)\n", __FUNCTION__, name, ntotal, itotal, nbref);

        SOSuperBlock sb;
        memset(&sb, 0, BlockSize);
        sb.magic = 0xFFFF; //magic number
        sb.version = VERSION_NUMBER; // version number
        strcpy(sb.name, name); //copy the name to the field
        sb.mntstat = 1; // mount status
        sb.mntcnt = 0; // how many times has it been mounted
        sb.ntotal = ntotal; // total number of blocks in device
        sb.itotal = itotal; // total number of inodes
        sb.it_size = ceil(itotal / IPB); // how many blocks for inodes
        sb.ifree = itotal - 1; // how many free inodes
        sb.ihead = 1; // first free inode
        sb.itail = sb.ifree; // last free inode
        sb.dz_start = sb.it_size + 1; // physical block where data blocks begin (number of inode blocks + superblock)
        sb.dz_total = ntotal - sb.it_size - 1; // -1 == superblock // total number of data blocks
        sb.dz_free = sb.dz_total - nbref - 1;   // from the total number of datablocks remove how many blocks needed for references and one for root block

        if(sb.dz_free<=HEAD_CACHE_SIZE){  // if free data blocks is less than head cache size
            sb.head_blk = NullReference;
            sb.tail_blk = NullReference;
            sb.head_idx = NullReference;
            sb.tail_idx = NullReference; // there are no reference blocks because everything is in cache
            sb.head_cache.idx = HEAD_CACHE_SIZE - sb.dz_free; // where does the cache begin
            uint32_t j = 1;
            for(uint32_t i = 0; i<= HEAD_CACHE_SIZE; i++){
                if(i >= sb.head_cache.idx){
                    sb.head_cache.ref[i] = j;
                    j++;
                }
                else{
                    sb.head_cache.ref[i] = NullReference; // needed because everything is 0 unless i force it to be NullReference
                }

            }
        }else{
            sb.tail_blk = nbref;
            sb.head_blk = 1;
            sb.head_idx = 1;
            uint32_t totalrefs = (sb.dz_free - HEAD_CACHE_SIZE);
            uint32_t refs_up_to_lastBlock = (RPB - 1) * (nbref - 1);
            uint32_t to_fill = totalrefs - refs_up_to_lastBlock;
            sb.tail_idx = to_fill + 1;


            sb.head_cache.idx = 0;
            for(uint32_t i = 0; i <= HEAD_CACHE_SIZE; i++){
                sb.head_cache.ref[i] = (i+1) + nbref;
            }
        }


        sb.tail_cache.idx = 0;
        for (uint32_t i = 0; i <= TAIL_CACHE_SIZE; i++){
            sb.tail_cache.ref[i] = NullReference;
        }

        soWriteRawBlock(0, &sb);


        /* change the following line by your code */
        //binFillSuperBlock(name, ntotal, itotal, nbref);
    }

};

