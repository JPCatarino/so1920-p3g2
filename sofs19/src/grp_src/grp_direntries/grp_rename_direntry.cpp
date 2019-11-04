#include "direntries.h"

#include "core.h"
#include "dal.h"
#include "fileblocks.h"
#include "bin_direntries.h"

#include <string.h>
#include <errno.h>
#include <sys/stat.h>

namespace sofs19
{
    void grpRenameDirEntry(int pih, const char *name, const char *newName)
    {
        soProbe(204, "%s(%d, %s, %s)\n", __FUNCTION__, pih, name, newName);

         if(strlen(newName)>SOFS19_MAX_NAME){						//verify if the "newName" given does not exceed the maximum length	
            	throw SOException(ENAMETOOLONG, __FUNCTION__);
            	return;
            }

            SOInode* inode = sofs19::soGetInodePointer(pih);
            SODirEntry dir_block[DPB];
            uint32_t block, dirent, rn_block, rn_dirent, name_found=0;

            for(block=0; block<=(*inode).size/BlockSize; block++){		//iterate through the direntry blocks of the parent inode
            	sofs19::soReadFileBlock(pih, block, dir_block);			//read a block of direntries

            	for(dirent=0; dirent<DPB; dirent++){		//iterate inside each block of direntries
            		if(!strcmp(dir_block[dirent].name, newName)){		//verify if an entry with "newName" already exists
            			throw SOException(EEXIST, __FUNCTION__);
            			return;
            		}
            		if(!strcmp(dir_block[dirent].name, name)){			//find direntry with "name"
            			rn_block = block;								//save its block number
            			rn_dirent = dirent;								//save its direntry number
            			name_found = 1;
            		}
            	}
            }															//if the outter for cycle finishes, no "newName" entry exists

            if(!name_found){											//verify if the "name" entry was found
            	throw SOException(ENOENT, __FUNCTION__);
            }
            															    //at this point, we are safe to rename the entry, as requested
            sofs19::soReadFileBlock(pih, rn_block, dir_block);			    //read the block containing the "name" entry
            strncpy(dir_block[rn_dirent].name, newName, SOFS19_MAX_NAME);   //change the "name" direntry to "newName"
            sofs19::soWriteFileBlock(pih, rn_block, dir_block);			    //write the change made
    

        /* change the following line by your code */
        binRenameDirEntry(pih, name, newName);
    }
};

