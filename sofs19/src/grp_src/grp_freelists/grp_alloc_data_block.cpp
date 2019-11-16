/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2009-2019
 */

#include "grp_freelists.h"

#include <stdio.h>
#include <errno.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#include "core.h"
#include "dal.h"
#include "freelists.h"
#include "bin_freelists.h"

namespace sofs19
{
    uint32_t grpAllocDataBlock()
    {
        soProbe(441, "%s()\n", __FUNCTION__);

        /* change the following line by your code */
        //return binAllocDataBlock();

        SOSuperBlock* soSB = soGetSuperBlockPointer(); //ponteiro para o superbloco

        //Se a head cache esta vazia, chama a replenish
        if(soSB->head_idx == HEAD_CACHE_SIZE){
            soReplenishHeadCache();
        }

        //vai a head cache e retira o primeiro elemento que lá está e atualiza o indice

        soSB->head_cache.ref[HEAD_CACHE_SIZE] = NullReference;
        soSB->dz_free -=1;
        soSB->head_cache.idx +=1;


        soSaveSuperBlock();

        return soSB->head_cache.ref[HEAD_CACHE_SIZE];
    }
};

