#include "grp_fileblocks.h"

#include "freelists.h"
#include "dal.h"
#include "core.h"
#include "bin_fileblocks.h"

#include <errno.h>

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

        SOInode* iPointer = sofs19::soGetInodePointer(ih);
        
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
        if(fbn>=N_DIRECT && fbn< i2FirstVal){
            block = grpAllocIndirectFileBlock(iPointer, fbn-N_DIRECT);
        }

        //case I2
        if(fbn >= i2FirstVal && fbn < i2Finish){
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

        uint32_t indirectIndex = (afbn/RPB) % RPB;
        uint32_t ref = afbn % RPB;
        uint32_t db[RPB]; //criar um bloco de referencias
        uint32_t block;

        //se a zona indireta está vazia, deve-se criar um bloco e alocar esse bloco na zona indireta
        if(ip->i1[indirectIndex] == NullReference){
            block = soAllocDataBlock();
            for(uint32_t i = 0; i<RPB; i++){
                db[i] = NullReference;
            }
            soWriteDataBlock(block, db);
            ip->i1[indirectIndex] = block;
            soReadDataBlock(ip->i1[indirectIndex], &db);
            block= soAllocDataBlock();
            db[ref] = block;
            ip->blkcnt +=2;

        } 

        else{
            soReadDataBlock(ip->i1[indirectIndex], &db);
            block= soAllocDataBlock();
            ip->i1[indirectIndex] = block;
            ip->blkcnt +=1;

        }
        return block;
    }


    /* ********************************************************* */

#
    /*
    */
    static uint32_t grpAllocDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn)
    {
        soProbe(302, "%s(%d, ...)\n", __FUNCTION__, afbn);

        /* change the following two lines by your code */
        //throw SOException(ENOSYS, __FUNCTION__); 
        //return 0;

        uint32_t doubleIndirectIndex = afbn/RPB/RPB;
        uint32_t db[RPB];
        uint32_t block;

        //se a zona duplamente indireta está vazia, deve-se criar um bloco e alocar esse bloco na zona duplamente indireta
        if(ip->i2[doubleIndirectIndex] == NullReference){
            block = soAllocDataBlock();
            for(uint32_t i = 0; i<RPB; i++){
                db[i] = NullReference;
            }
            soWriteDataBlock(block, db);
          
            ip->i2[doubleIndirectIndex] = block;
            soReadDataBlock(ip->i2[doubleIndirectIndex], &db);
            block= grpAllocIndirectFileBlock(ip, afbn);            
            ip->blkcnt +=1;

        } 

        else{
            soReadDataBlock(ip->i2[doubleIndirectIndex], &db);  
            block= grpAllocIndirectFileBlock(ip, afbn);
            
        }
        return block;



    }

};

