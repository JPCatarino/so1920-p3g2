#include "grp_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "bin_mksofs.h"

#include <string.h>
#include <inttypes.h>

namespace sofs19
{
    /*
       filling in the contents of the root directory:
       the first 2 entries are filled in with "." and ".." references
       the other entries are empty.
       */
    void grpFillRootDir(uint32_t itotal){

        soProbe(606, "%s(%u)\n", __FUNCTION__, itotal);
    
        SODirEntry dir[DPB];

        //first two entries
        dir[0].in=0x00000000;
        dir[0].name[0]='.';
        dir[1].in=0x00000000;
        dir[1].name[0]='..';

        //rest of the blocks
        for(uint32_t i=2; i<DPB; i++) {

            dir[i].in = NullReference;
            for (int j = 0; j <= SOFS19_MAX_NAME; j++){
                dir[i].name[j]='0'; 
            }
        }

        uint32_t usedBlocks = (itotal/IPB)+1;
        soWriteRawBlock(usedBlocks,dir);

        /* change the following line by your code */
        return binFillRootDir(itotal);
    }
};

