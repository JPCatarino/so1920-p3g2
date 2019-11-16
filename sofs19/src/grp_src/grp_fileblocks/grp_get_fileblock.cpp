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
            if (fbn < N_DIRECT) return ip -> d[fbn];
            else if (fbn <N_DIRECT+RPB)
            {
              return grpGetIndirectFileBlock(ip,fbn);
            }
            else
            {
              return grpGetDoubleIndirectFileBlock(ip,fbn);
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
        uint32_t temp;
        uint32_t ref[RPB];

        if(ip -> i2[afbn/(RPB*RPB)] == NullReference){
            return NullReference;
        }
        else{
            soReadDataBlock((ip -> i2[afbn/(RPB*RPB)]), ref);
            temp = ref[afbn/(RPB - (afbn / RPB))];
            if (temp == NullReference){
                return NullReference;
            } else {
                soReadDataBlock(temp, ref);
                return ref[afbn%RPB];
            }
        }
    }