#include "grp_fileblocks.h"

#include "freelists.h"
#include "dal.h"
#include "core.h"
#include "bin_fileblocks.h"

#include <inttypes.h>
#include <errno.h>
#include <assert.h>

namespace sofs19
{

    /* free all blocks between positions ffbn and RPB - 1
     * existing in the block of references given by i1.
     * Return true if, after the operation, all references become NullReference.
     * It assumes i1 is valid.
     */
    static bool grpFreeIndirectFileBlocks(SOInode * ip, uint32_t i1, uint32_t ffbn);

    /* free all blocks between positions ffbn and RPB**2 - 1
     * existing in the block of indirect references given by i2.
     * Return true if, after the operation, all references become NullReference.
     * It assumes i2 is valid.
     */
    static bool grpFreeDoubleIndirectFileBlocks(SOInode * ip, uint32_t i2, uint32_t ffbn);

    // RPB ^ 2
    static uint32_t pwRPB = RPB * RPB;

    // Max FB number in sofs19
    static uint32_t maxFBN = N_DIRECT + (N_INDIRECT * RPB) + (N_DOUBLE_INDIRECT * pwRPB);


    /* ********************************************************* */

    /* Helpful formulas
    * i1 index = fbn / RPB
    * i1 Ref Pos = fbn % RPB
    * i2 index = fbn / RPB ^ 2
    * i2 Ref Pos = fbn / RPB  - (i2index * RPB)
    */

    void grpFreeFileBlocks(int ih, uint32_t ffbn)
    {
        soProbe(303, "%s(%d, %u)\n", __FUNCTION__, ih, ffbn);

        /* change the following line by your code */
        binFreeFileBlocks(ih, ffbn);
    }

    /* ********************************************************* */


    static bool grpFreeIndirectFileBlocks(SOInode * ip, uint32_t i1, uint32_t ffbn)
    {
        soProbe(303, "%s(..., %u, %u)\n", __FUNCTION__, i1, ffbn);

        // Assume the block will be completely empty
        bool isEmpty = true;

        // Read i1 datablock to array
        uint32_t i1RefBlock[RPB];
        soReadDataBlock(i1, &i1RefBlock);
        
        for (uint32_t i = 0; i < RPB; i++){
            // Only change blocks to null after the ffbn (first fb number)
            if(i>=ffbn){
                if(i1RefBlock[i] != NullReference){
                    soFreeDataBlock(i1RefBlock[i]);     // Free datablock used by fileblock
                    i1RefBlock[i] = NullReference;      // Set the reference to null
                }
            }
            // If not inside the interval, check if the block is null
            else{
                if(i1RefBlock[i] != NullReference) isEmpty = false;
            }
        }

        soWriteDataBlock(i1, &i1RefBlock);              // Write ref block after cleaning to i1
        return isEmpty;
    }


    /* ********************************************************* */


    static bool grpFreeDoubleIndirectFileBlocks(SOInode * ip, uint32_t i2, uint32_t ffbn)
    {
        soProbe(303, "%s(..., %u, %u)\n", __FUNCTION__, i2, ffbn);
        // Assume the block will be completely empty
        bool isEmpty = true;

        // Read i2 datablock to array
        uint32_t i2RefBlock[RPB];
        soReadDataBlock(i2, &i2RefBlock);

        // Get starting index
        uint32_t ffbnIndexI2 = ffbn / pwRPB;

        for(uint32_t i = 0; i < RPB; i++){
            // Verifies if i is inside interval of [ffbnI2, RPB**2 - 1]
            if(i >= ffbnIndexI2){
                if(i2RefBlock[i] != NullReference){
                    // Get the position to be removed from referenced data block
                    uint32_t ffbnRefPosI2 = (ffbn/RPB) - (i*RPB);
                    // if i1 block is empty, free the datablock
                    if(grpFreeIndirectFileBlocks(ip, i2RefBlock[i], ffbnRefPosI2)) 
                        soFreeDataBlock(i2RefBlock[i]);
                    i2RefBlock[i] = NullReference;
                }
            }
            else{
                if(i2RefBlock[i] != NullReference) isEmpty = false;
            }
        }
    return  isEmpty;
    }


    /* ********************************************************* */
};

