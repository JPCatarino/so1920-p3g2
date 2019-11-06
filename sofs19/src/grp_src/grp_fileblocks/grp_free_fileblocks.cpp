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

    // Max value of i1 / first value of i2
    static uint32_t i2FirstValue = N_DIRECT * (N_INDIRECT * RPB);


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

        SOInode* ip = soGetInodePointer(ih);
        
        // Check if ffbn is invalid
        if(ffbn < 0 || ffbn >= maxFBN)
            throw SOException(EINVAL, __FUNCTION__);
        // if ffbn is in direct
        if (ffbn < N_DIRECT){                 
            for(; ffbn < N_DIRECT; ffbn++){
                if(ip->d[ffbn] != NullReference){
                    soFreeDataBlock(ip->d[ffbn]);
                    ip->d[ffbn] = NullReference;
                }
            }
        }
        // if ffbn is indirect
        if(ffbn >= N_DIRECT && ffbn < i2FirstValue){
            uint32_t i1index = ((ffbn - N_DIRECT) / RPB);
            for(uint32_t i = i1index; i < N_INDIRECT; i++){
                if(i == i1index){
                    if(grpFreeIndirectFileBlocks(ip, i, ffbn - N_DIRECT)){
                        soFreeDataBlock(ip->i1[i]);
                        ip->i1[i] = NullReference;
                    }
                }
                else{
                    if(grpFreeIndirectFileBlocks(ip, i, 0)){
                        soFreeDataBlock(ip->i1[i]);
                        ip->i1[i] = NullReference;
                    }
                }
            }
            ffbn = i2FirstValue;
        }
        
        // if ffbn is double indirect
        if(ffbn >= i2FirstValue && ffbn < maxFBN){
            uint32_t i2index = (ffbn - i2FirstValue) / pwRPB;
            for(uint32_t i = i2index; i < N_DOUBLE_INDIRECT; i++){
                if(i == i2index){
                    if(grpFreeDoubleIndirectFileBlocks(ip, i, ffbn - i2FirstValue)){
                        soFreeDataBlock(ip->i2[i]);
                        ip->i2[i] = NullReference;
                    }
                }
                else{
                    if(grpFreeDoubleIndirectFileBlocks(ip, i, 0)){
                        soFreeDataBlock(ip->i2[i]);
                        ip->i2[i] = NullReference;
                    }
                }
            } 
        }

        soSaveInode(ih);
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

        ffbn = ffbn % RPB;

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
        uint32_t ffbnIndexI2 = ffbn / RPB;

        // Delete positions after starting index
        for(uint32_t i = ffbnIndexI2; i < RPB; i++){
            if(i2RefBlock[i] != NullReference){
                // Get the position to be removed from referenced data block
                uint32_t ffbnRefPosI2;
                if (ffbn == 0)
                    ffbnRefPosI2 = 0;
                else
                    ffbnRefPosI2 = ffbn - (i*RPB);
                if(ffbnRefPosI2 == 0 || i != ffbnIndexI2){
                    if(grpFreeIndirectFileBlocks(ip, i2RefBlock[i], 0))
                        soFreeDataBlock(i2RefBlock[i]);
                }
                else if(grpFreeIndirectFileBlocks(ip, i2RefBlock[i], ffbnRefPosI2))
                    soFreeDataBlock(i2RefBlock[i]);
            }
        }

        // Check if block is empty
        for(uint32_t i = 0; i < RPB; i++){
            if(i2RefBlock[i] != NullReference)
                isEmpty = false;
        }

        soWriteDataBlock(i2, &i2RefBlock);
        return isEmpty;
    }


    /* ********************************************************* */
};

