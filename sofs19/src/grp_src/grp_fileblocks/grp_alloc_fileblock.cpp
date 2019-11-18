#include "grp_fileblocks.h"

#include "freelists.h"
#include "dal.h"
#include "core.h"
#include "bin_fileblocks.h"

#include <errno.h>
#include <string.h>
#include <math.h>
#include <iostream>

namespace sofs19
{

 
    static uint32_t grpAllocIndirectFileBlock(SOInode * ip, uint32_t afbn);
    static uint32_t grpAllocDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn);


    /* ********************************************************* */

    uint32_t grpAllocFileBlock(int ih, uint32_t fbn)
    {
        soProbe(302, "%s(%d, %u)\n", __FUNCTION__, ih, fbn);

        /* change the following two lines by your code */
        //return binAllocFileBlock(ih, fbn);

        SOInode* iPointer = soGetInodePointer(ih);
        
        uint32_t i2FirstVal =N_INDIRECT * RPB + N_DIRECT;
        uint32_t i2Finish = i2FirstVal + (N_DOUBLE_INDIRECT * (RPB * RPB));
        uint32_t block;

        if(fbn < 0 || fbn >= i2Finish){
            throw SOException(EINVAL, __FUNCTION__);
        }

        //case N_Direct
        if(fbn < N_DIRECT){
            block = soAllocDataBlock();
            iPointer->d[fbn] = block;
            iPointer->blkcnt +=1;    
        }

        //case I1
        else if(fbn>=N_DIRECT && fbn< i2FirstVal){
            block = grpAllocIndirectFileBlock(iPointer, fbn-N_DIRECT);
        }

        //case I2
        else if(fbn >= i2FirstVal && fbn < i2Finish){
            block = grpAllocDoubleIndirectFileBlock(iPointer, fbn- i2FirstVal);
        }

        soSaveInode(ih);
        return block;

    }

    /* ********************************************************* */


    /*
    */
    static uint32_t grpAllocIndirectFileBlock(SOInode * ip, uint32_t afbn)
    {
        soProbe(302, "%s(%d, ...)\n", __FUNCTION__, afbn);

        /* change the following two lines by your code */
        //throw SOException(ENOSYS, __FUNCTION__); 
        //return 0;

        uint32_t pos = afbn/RPB;
        uint32_t i1RefBlock[RPB];
        uint32_t i1DBlock;

        //se a zona indireta está vazia, deve-se criar um bloco e alocar esse bloco na zona indireta
        if (ip->i1[pos]==NullReference){
            i1DBlock = soAllocDataBlock();
            ip->blkcnt++;
            ip->i1[pos]=i1DBlock;
            memset(&i1RefBlock, NullReference, RPB*sizeof(uint32_t));
            soWriteDataBlock(i1DBlock, &i1RefBlock);
        }
        else{
            i1DBlock=ip->i1[pos];
            soReadDataBlock(i1DBlock, &i1RefBlock);
        }
            
        // Alocar block em i1
        uint32_t posI1db = afbn - RPB*pos;
        uint32_t newDB = soAllocDataBlock();
        i1RefBlock[posI1db] = newDB;
        soWriteDataBlock(i1DBlock, &i1RefBlock);
        ip->blkcnt++;

        return newDB;
    }


    /* ********************************************************* */


    /*
    */
    static uint32_t grpAllocDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn)
    {
        soProbe(302, "%s(%d, ...)\n", __FUNCTION__, afbn);

        /* change the following two lines by your code */
        //throw SOException(ENOSYS, __FUNCTION__); 
        //return 0;

        uint32_t i2Pos = afbn/pow(RPB, 2);
        uint32_t i1DBlock;
        uint32_t i2RefBlock[RPB];

        //se a zona duplamente indireta está vazia, deve-se criar um bloco e alocar esse bloco na zona duplamente indireta
        if (ip->i2[i2Pos]==NullReference){
            i1DBlock = soAllocDataBlock();
            ip->blkcnt++;
            ip->i2[i2Pos] = i1DBlock;
            memset(&i2RefBlock, NullReference, RPB*sizeof(uint32_t));
            soWriteDataBlock(i1DBlock, &i2RefBlock);
        }
        else{
            i1DBlock = ip->i2[i2Pos];
            soReadDataBlock(i1DBlock, &i2RefBlock);
        }
            
        // Alocar bloco em i2
        uint32_t i1Pos = afbn/RPB - i2Pos*RPB;
        uint32_t i1RefBlock[RPB];
        uint32_t blk_refs;
        if (i2RefBlock[i1Pos] == NullReference){
            blk_refs = soAllocDataBlock();
            ip->blkcnt++;
            i2RefBlock[i1Pos] = blk_refs;
            memset(&i1RefBlock, NullReference, RPB*sizeof(uint32_t));
            soWriteDataBlock(blk_refs, &i1RefBlock);
        }
        else{
            blk_refs = i2RefBlock[i1Pos];
            soReadDataBlock(blk_refs, &i1RefBlock);
        }
            
        // Alocar blk na refs de i2
        uint32_t i2RefPos = afbn - i2Pos*RPB - i1Pos*RPB;
        uint32_t newDB = soAllocDataBlock();
        i1RefBlock[i2RefPos] = newDB;
        ip->blkcnt++;
        soWriteDataBlock(blk_refs, &i1RefBlock);
        soWriteDataBlock(i1DBlock, &i2RefBlock);

        return newDB;
    }

};

