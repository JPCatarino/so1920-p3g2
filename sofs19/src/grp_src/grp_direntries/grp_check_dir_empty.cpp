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
    bool grpCheckDirEmpty(int ih)
    {
        soProbe(205, "%s(%d)\n", __FUNCTION__, ih);
        SOInode* in =  soGetInodePointer(ih);
        SODirEntry direntries[DPB];

        for(uint32_t i = 0; i<in->size; i++){
            sofs19::soReadFileBlock(ih, i, direntries);
            for(uint32_t j = 0; j < (sizeof direntries/sizeof direntries[0]); j++){
                if((strcmp(direntries[j].name, ".") !=0 || strcmp(direntries[j].name, "..") != 0) && strcmp(direntries[j].name, "\0") !=0){
                    return false;
                }

            }
        }
        return true;
        /* change the following line by your code */
        //return binCheckDirEmpty(ih);


    }
};

