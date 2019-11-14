#include "direntries.h"

#include "core.h"
#include "dal.h"
#include "fileblocks.h"
#include "bin_direntries.h"

#include <errno.h>
#include <string.h>
#include <sys/stat.h>

namespace sofs19
{
    uint32_t grpGetDirEntry(int pih, const char *name)
    {
        soProbe(201, "%s(%d, %s)\n", __FUNCTION__, pih, name);

        /* change the following line by your code */
        //return binGetDirEntry(pih, name);
        
        SOInode *in = soGetInodePointer(pih);
        SODirEntry l_dir[DPB];		// List of direntries

        if(strcmp(name, "") == 0){
        	throw SOException(EINVAL, __FUNCTION__);
        }

        if(S_ISDIR(in->mode) == 0){
        	throw SOException(ENOTDIR, __FUNCTION__);
        }
 
    	for(uint32_t i = 0; i < in->size; i++){
        	soReadFileBlock(pih, i, l_dir);
        	for(uint32_t j = 0; j < DPB; j++){
        		if(l_dir[j].name[0] == '/'){
		        	throw SOException(EINVAL, __FUNCTION__);
		        }
        		if(strcmp(l_dir[j].name, name) == 0){
        			return l_dir[j].in; 	
    			}
           	}
    	}	
        return -1;
    }; 	
};

