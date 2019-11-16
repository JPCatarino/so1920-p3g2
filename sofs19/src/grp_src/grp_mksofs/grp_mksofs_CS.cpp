#include "grp_mksofs.h"

#include "core.h"
#include "bin_mksofs.h"

#include <inttypes.h>
#include <iostream>

namespace sofs19
{
    void grpComputeStructure(uint32_t ntotal, uint32_t & itotal, uint32_t & nbref)
    {
        soProbe(601, "%s(%u, %u, ...)\n", __FUNCTION__, ntotal, itotal);

        /* change the following line by your code */
        //binComputeStructure(ntotal, itotal, nbref);

        if(itotal > ntotal / IPB || itotal < IPB){
            itotal = ntotal / 16;
        }

        if(itotal % IPB == 0){
            itotal /= IPB;
        }
        else{
            itotal = (itotal / IPB + 1) * IPB;
        }

        uint32_t inodeBlocks = itotal / IPB;

        if(1 + inodeBlocks + 1 + HEAD_CACHE_SIZE >= ntotal){
            nbref = 0;
        }
        else{
            uint32_t dataBlocks = ntotal - 1 - inodeBlocks - 1 - HEAD_CACHE_SIZE;
            uint32_t rem = dataBlocks % RPB;
            if(rem == 1){
                nbref = (dataBlocks - 1) / RPB;
            }
            else
            {
                nbref = dataBlocks / RPB + 1;
            }
            
        }
    }
};

