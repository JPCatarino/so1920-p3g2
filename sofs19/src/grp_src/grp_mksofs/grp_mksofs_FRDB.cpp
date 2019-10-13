#include "grp_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "bin_mksofs.h"

#include <inttypes.h>
#include <string.h>

namespace sofs19
{
    void grpFillReferenceDataBlocks(uint32_t ntotal, uint32_t itotal, uint32_t nbref)
    {
        soProbe(605, "%s(%u, %u, %u)\n", __FUNCTION__, ntotal, itotal, nbref);

        if(nbref > 0){
            uint32_t usedBlocks = (itotal / IPB) + 1;
            uint32_t totalDBlocks = ntotal - usedBlocks; // Calcuate the total free datablocks
            //uint32_t freeDBlocks = totalDBlocks - nbref; // Calculate the free unused data blocks (Total - InodeBlocks - RefBlocks - root)
            uint32_t currentRef = HEAD_CACHE_SIZE + nbref + 1; // Calculate the next reference to be written
            
            // Iterates each available reference block, to fill it.
            for(uint32_t i = 1; i <= nbref; i++){
                uint32_t dBlock[RPB];

                if(i < nbref)
                    dBlock[0] = i + 1; // Reference to next block on the first space of each reference block.
                else
                    dBlock[0] = NullReference; // Only applies to the last reference block.
                
                // Fills the current block. If currentRef is higher than available free blocks, fills the remaining available spaces with NullReferences.
                for(uint32_t z = 1; z <= RPB - 1; z++){
                    if(currentRef < totalDBlocks){
                        dBlock[z] = currentRef;
                        currentRef++;
                    }
                    else{
                        dBlock[z] = NullReference;
                    }
                }
                soWriteRawBlock(usedBlocks + i, &dBlock);
            }
        }
        return;
    }
};

