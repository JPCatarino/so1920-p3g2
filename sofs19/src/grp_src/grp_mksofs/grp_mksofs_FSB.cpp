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
        sb.it_size = itotal / IPB; // how many blocks for inodes
        sb.ifree = itotal - 1; // how many free inodes
        sb.ihead = 1; // first free inode
        sb.itail = itotal - 1; // last free inode
        sb.dz_start = sb.it_size + 1; // physical block where data blocks begin
        sb.dz_total = ntotal - sb.it_size - 1; // -1 == superblock // total number of data blocks
        sb.dz_free = sb.dz_total - 1;
        uint32_t refblocks = ceil(sb.dz_total / RPB); // how many blocks we need to store data blocks references
        sb.head_blk = sb.it_size + 2; // 0 is for superblock + 1 for root
        sb.tail_blk = refblocks+1; // accounting for the root






        /* change the following line by your code */
        binFillSuperBlock(name, ntotal, itotal, nbref);
    }
};

