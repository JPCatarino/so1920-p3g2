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
    static void grpFreeIndirectFileBlocks(SOInode * ip, uint32_t * i1, uint32_t ffbn);

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
        else if (ffbn < N_DIRECT){                 
            uint32_t delBlockCount = 0;
            for(; ffbn < N_DIRECT; ffbn++){
                if(ip->d[ffbn] != NullReference)
                    delBlockCount++;
                ip->d[ffbn] = NullReference;
            }
            ip->blkcnt -= delBlockCount;
        }
        // if ffbn is indirect
        else if(ffbn >= N_DIRECT && ffbn < i2FirstValue){
            grpFreeIndirectFileBlocks(ip, ip->i1, ffbn);
            ffbn = i2FirstValue;
        }
        // if ffbn is double indirect
        else{
        }

        soSaveInode(ih);
    }

    /* ********************************************************* */


    static void grpFreeIndirectFileBlocks(SOInode * ip, uint32_t* i1, uint32_t ffbn)
    {
        soProbe(303, "%s(..., %u, %u)\n", __FUNCTION__, i1, ffbn);

        uint32_t delBlockCount = 0;

        // Calculate I1 index of ffbn
        uint32_t ffbnIndexI1 = (ffbn / RPB);

        // Calculate I1 Pos
        uint32_t ffbnRefPos = (ffbn % RPB);

        uint32_t dBlock[RPB];
        bool isEmpty = true;

        // Start freeing from i1 index of ffbn to end of the list
        for(uint32_t i = ffbnIndexI1; i < N_INDIRECT; i++){
            if (i1[i] == NullReference)
                continue;

            soReadDataBlock(i1[i], &dBlock);

            isEmpty = true;
            for(uint32_t x = 0; x < RPB; x++){
                // Start freeing from RefPos, and count the number of deleted blocks
                if(x >= ffbnRefPos){
                    if(dBlock[x] != NullReference)
                        delBlockCount++;
                    dBlock[x] = NullReference;
                }
                else{
                    if (dBlock[x] != NullReference)
                        isEmpty = false;
                }
            }

            if(isEmpty){
                soFreeDataBlock(i1[i]);
                i1[i] = NullReference;
                delBlockCount++;
            }
            else
                soWriteDataBlock(i1[i], &dBlock);

            // set reference to 0 so I can delete next blocks on i2 from beginning
            ffbnRefPos = 0;
        }
        ip->blkcnt -= delBlockCount;
    }


    /* ********************************************************* */

#if false
    static bool grpFreeDoubleIndirectFileBlocks(SOInode * ip, uint32_t * i2, uint32_t ffbn)
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
        
        soWriteDataBlock(i2, &i2RefBlock);
        return isEmpty;
    }
#endif

    /* ********************************************************* */
};

