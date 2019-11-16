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
#include <sys/stat.h>
#include <string.h>

#include <iostream>

#include "core.h"
#include "dal.h"
#include "freelists.h"
#include "bin_freelists.h"

namespace sofs19
{
    uint32_t grpAllocInode(uint32_t type, uint32_t perm)
    {   
        soProbe(401, "%s(0x%x, 0%03o)\n", __FUNCTION__, type, perm);
        
        soOpenSuperBlock();
		SOSuperBlock* sp = soGetSuperBlockPointer();
        uint32_t res;
        if(type != S_IFREG || type != S_IFDIR || type != S_IFLNK){
            throw SOException(EINVAL, __FUNCTION__);
        }

        if(perm > 0777){
            throw SOException(EINVAL, __FUNCTION__);
        }

        if(sp->ifree == 0){
            throw SOException(ENOSPC, __FUNCTION__);
        }

        /* Abrir o Inode da cabeça */
        res = sp->ihead;
        int iopen = soOpenInode(res);
        SOInode *inode = soGetInodePointer(iopen);

        sp->ifree = sp->ifree-1;
        sp->ihead = inode->next;
         if (sp->ifree == 0) {
            sp->itail = NullReference;
        }
        soSaveSuperBlock();
        memset(inode,0x00, sizeof(SOInode));
        inode->mode = type | perm;
        inode->owner = getuid();
        inode->group = getgid();
        inode->atime = inode->ctime = inode->mtime = time(NULL);


    	for(uint32_t i = 0; i<N_DIRECT; i++){
				inode->d[i] = NullReference;
			}
        for(uint32_t i = 0; i<N_INDIRECT; i++){
            inode->i1[i] = NullReference;
        }
        for(uint32_t i = 0; i<N_DOUBLE_INDIRECT; i++){
            inode->i2[i] = NullReference;
        }

        soSaveInode(iopen);
        soCloseInode(iopen);
        return res;
    }
}
