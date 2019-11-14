#include "grp_fileblocks.h"

#include "dal.h"
#include "core.h"
#include "fileblocks.h"
#include "bin_fileblocks.h"

#include <string.h>
#include <inttypes.h>

namespace sofs19
{
    void grpWriteFileBlock(int ih, uint32_t fbn, void *buf)
    {
        soProbe(332, "%s(%d, %u, %p)\n", __FUNCTION__, ih, fbn, buf);

        /* change the following line by your code */
        //binWriteFileBlock(ih, fbn, buf);

		uint32_t blk = soGetFileBlock(ih, fbn); 

        // Allocates the block if it has not been allocated yet
        if(blk == NullReference){
        	blk = soAllocFileBlock(ih, fbn);
        }
        // writes a file block
        soWriteDataBlock(blk, buf);	
    }
};

