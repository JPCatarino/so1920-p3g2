#include "direntries.h"

#include "core.h"
#include "dal.h"
#include "fileblocks.h"
#include "direntries.h"
#include "bin_direntries.h"

#include <errno.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

namespace sofs19
{
    uint32_t grpTraversePath(char *path)
    {
        soProbe(221, "%s(%s)\n", __FUNCTION__, path);

        // Check if path is absolute
        if (path[0] != '/'){
            throw SOException(EINVAL, __FUNCTION__);
        }

        char *parentDir = dirname(strdupa(path));
        char *baseName = basename(strdupa(path));

        // If we trasversed whole path, return 0
        if (strcmp(parentDir, baseName) == 0){
            return 0; 
        }

        // Recursively trasverse the path, so we can get to the root
        uint32_t parentInode = soTraversePath(parentDir);
        int ih = soOpenInode(parentInode);

        // Checks if we have traverse permissions 
        if(soCheckInodeAccess(ih, 1)){
            uint32_t cDirInode = soGetDirEntry(ih, baseName);
            soCloseInode(ih);
            // Checks if directory actually exists and/or is actually a directory, if so return inode value
            if(cDirInode != NullReference){
                return cDirInode;
            }
            throw SOException(ENOTDIR, __FUNCTION__);
        }
        throw SOException(EACCES, __FUNCTION__);

    }
};

