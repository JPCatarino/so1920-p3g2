#include "grp_fileblocks.h"

#include "dal.h"
#include "core.h"
#include "fileblocks.h"
#include "bin_fileblocks.h"

#include <string.h>
#include <inttypes.h>

namespace sofs19
{

    /*Read a file block.
    Data is read from a specific data block which is supposed to 
    belong to an inode associated to a file (a regular file, a directory or a symbolic link)*/
    void grpReadFileBlock(int ih, uint32_t fbn, void *buf){

        soProbe(331, "%s(%d, %u, %p)\n", __FUNCTION__, ih, fbn, buf);

         uint32_t bn = soGetFileBlock(ih, fbn);
            if(bn == NullReference) {
				uint32_t index;
				for(index=0; index<BlockSize; index++) {
					memset(buf, '\0', BlockSize);
				}
			}
			else {
			    soReadDataBlock(bn, buf);
			}

        /* change the following line by your code */
        ///binReadFileBlock(ih, fbn, buf);
    }
};

