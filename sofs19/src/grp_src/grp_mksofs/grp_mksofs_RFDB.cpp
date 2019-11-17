#include "grp_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "bin_mksofs.h"

#include <string.h>
#include <inttypes.h>

namespace sofs19
{
     void grpResetFreeDataBlocks(uint32_t ntotal, uint32_t itotal, uint32_t nbref)
    {
        soProbe(607, "%s(%u, %u, %u)\n", __FUNCTION__, ntotal, itotal, nbref);

        /* change the following line by your code */
        //binResetFreeDataBlocks(ntotal, itotal, nbref);
            uint32_t usedBlocks = (itotal / IPB) + 1;
            uint32_t totalBlocks = nbref + usedBlocks + 1;

            char reset[BlockSize];   

            for(uint32_t i = 0; i< BlockSize; i++){
                reset[i] = 0;  //coloca a 0 
            }

            while( totalBlocks < ntotal){
                soWriteRawBlock(totalBlocks++, reset);
            }



    }
};

