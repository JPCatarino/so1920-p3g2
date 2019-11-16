#include "grp_fileblocks.h"

#include "dal.h"
#include "core.h"
#include "bin_fileblocks.h"

#include <errno.h>

namespace sofs19
{
    /* ********************************************************* */


    static uint32_t grpGetIndirectFileBlock(SOInode * ip, uint32_t fbn);
    static uint32_t grpGetDoubleIndirectFileBlock(SOInode * ip, uint32_t fbn);


    /* ********************************************************* */

    uint32_t grpGetFileBlock(int ih, uint32_t fbn)
    {
        soProbe(301, "%s(%d, %u)\n", __FUNCTION__, ih, fbn);

        /* change the following line by your code */
        //return binGetFileBlock(ih, fbn);
          SOInode *ip = soGetInodePointer(ih);
            if (fbn < 4) return ip -> d[fbn];
            else if (fbn <= ((2 * RPB) + 3))
            {
              return grpGetIndirectFileBlock(ip,(fbn - 4));
            }
            else
            {
              return grpGetDoubleIndirectFileBlock(ip,(fbn - 260));
            }
    }

    /* ********************************************************* */

    static uint32_t grpGetIndirectFileBlock(SOInode * ip, uint32_t afbn)
    {
        soProbe(301, "%s(%d, ...)\n", __FUNCTION__, afbn);

        /* change the following two lines by your code */
        //throw SOException(ENOSYS, __FUNCTION__); 
        //return 0;
        uint32_t ref[RPB];
        soReadDataBlock(ip -> i1[(afbn/RPB)],ref);
        return ref[afbn % RPB];
    }


    /* ********************************************************* */


    static uint32_t grpGetDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn)
    {
        soProbe(301, "%s(%d, ...)\n", __FUNCTION__, afbn);

        /* change the following two lines by your code */
        //throw SOException(ENOSYS, __FUNCTION__); 
        //return 0;
        uint32_t ref[RPB];
        uint32_t ref1[RPB];
        soReadDataBlock(ip -> i2[afbn/(RPB*RPB)],ref);
        uint32_t temp = afbn;
        if (afbn > 16383)
        {
            temp = afbn - 16384;
        }
        soReadDataBlock(ref[temp/RPB],ref1);
        return ref1[afbn%RPB];
    }

};

