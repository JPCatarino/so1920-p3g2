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
    uint32_t grpDeleteDirEntry(int pih, const char *name)
    {
        soProbe(203, "%s(%d, %s)\n", __FUNCTION__, pih, name);

        /* change the following line by your code */
        SODirEntry dir[DPB];
            SOInode *ip = soGetInodePointer(pih);
            uint32_t dst = NullReference;
            for (uint32_t fbn = 0; fbn < (ip->size)/BlockSize;fbn++)
            {
              soReadFileBlock(pih,fbn,dir);
              for (uint32_t i = 0; i < DPB; i++)
              {
                if (strcmp(dir[i].name,name) == 0)
                {
                  memset(dir[i].name,'\0',SOFS19_MAX_NAME);
                  dst = dir[i].in;
                  dir[i].in = NullReference;
                  soWriteFileBlock(pih,fbn,dir);
                  return dst;
                }
              }
            }
            throw SOException(ENOENT, __FUNCTION__);
            return dst;
    }
};

