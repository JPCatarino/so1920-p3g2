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
    void grpFreeInode(uint32_t in)
    {
        soProbe(402, "%s(%u)\n", __FUNCTION__, in);

        //Ponteiro do SuperBlock
		SOSuperBlock* sbp = soGetSuperBlockPointer();
         
        //Caso a tail cache esteja cheia
		if(sbp -> tail_cache.idx == TAIL_CACHE_SIZE){
			sofs19::soDepleteTailCache();
		}
		
		//índice do inode que é para libertar é o que terá 'tail_cahce.idx++' index
		sbp -> tail_cache.ref[sbp -> tail_cache.idx++] = in;
		
		int ih = soOpenInode(in);
		SOInode* node = soGetInodePointer(ih);
		
		//dados do inode a libertar
		node -> mode = INODE_FREE;
		node -> lnkcnt = 0;
		node -> owner = 0;
		node -> group = 0;
		node -> size = 0;
		node -> blkcnt = 0;
		
		// atualiza o ponteiro do superBlock para ifree++ (inodes free)  
		sbp -> ifree++;
        
		soSaveSuperBlock();
		soSaveInode(ih);
		soCloseInode(ih);	
        


        /* change the following line by your code */
        //binFreeInode(in);
    }
};

