/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2019
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
    void grpFreeDataBlock(uint32_t bn)
    {
        soProbe(442, "%s(%u)\n", __FUNCTION__, bn);

        /* change the following line by your code */
        //binFreeDataBlock(bn);

        SOSuperBlock *sbp;
        sbp = soGetSuperBlockPointer();

        /* 
        	if the number of data blocks to be freed is bigger than the 
			total number of data blocks, throw an exception
        */
        if(sbp->dz_total <= bn){
        	throw SOException(EINVAL, __FUNCTION__);
        }

        // if cache is full, deplete cache before the insertion takes place
        if((sbp->tail_cache).idx == TAIL_CACHE_SIZE){
        	soDepleteTailCache();
        }

        (sbp->tail_cache).idx++;		// tail cache index increments
        (sbp->dz_free)++;				// number of free blocks in data zone increases
        (sbp->tail_cache).ref[(sbp->tail_cache).idx-1] = bn;	

        soSaveSuperBlock();
    }
};

