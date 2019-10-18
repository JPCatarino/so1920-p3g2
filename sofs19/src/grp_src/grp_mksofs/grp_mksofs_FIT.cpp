#include "grp_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "bin_mksofs.h"

#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <inttypes.h>

#include <math.h>

namespace sofs19
{
    void grpFillInodeTable(uint32_t itotal, bool set_date)
    {
        soProbe(604, "%s(%u)\n", __FUNCTION__, itotal);

        /* change the following line by your code */
        //return binFillInodeTable(itotal, set_date);
        
        struct SOInode in[IPB];

        uint32_t total_blocks = ceil(itotal/IPB);		// Number of blocks of the inode table
        uint32_t block = 1;								/* Variable used to contain the physical number of 
        													the block to be written into; starts on block = 0
        													and ends when block = total_blocks */

        time_t current_time = time(NULL);		// Returns the current calendar time
        time_t time;

        // Structure used to hold the time and date
        struct tm t;
        t.tm_sec = 0;					// seconds, range 0 to 59
        t.tm_min = 0;					// minutes, range 0 to 59
        t.tm_hour = 0;					// hours, range 0 to 23
        t.tm_mday = 1;					// day of month, range 1 to 31
        t.tm_mon = 0;					// month, range 0 to 11
        t.tm_year = 2019 - 1900;		// the number of years since 1900
        t.tm_wday = 0;					// day of the week, range 0 to 6
        t.tm_yday = 0;					// day in the year, range 0 to 365
        t.tm_isdst = 0;					// daylight saving time

        time = mktime(&t);

        uint32_t i, j, k;
        for(i = 0; i < total_blocks; ++i){				// For each block in the inode table
        	for(j = 0; j < IPB; ++j){					// For each inode in each block
        		if(i == 0 && j == 0){					// If it is the first inode (root)
        			in[j].mode = S_IFDIR | 0755;
        			in[j].lnkcnt = 2;
        			in[j].owner = 1000;
        			in[j].group = 1000;
        			in[j].size = BlockSize;
        			in[j].blkcnt = 1;

        			in[j].atime = current_time;			

        			in[j].mtime = current_time;			
        			in[j].ctime = current_time;			
        			for(k = 0; k < N_DIRECT; k++) 
        				in[j].d[k] = NullReference;
        			for(k = 0; k < N_INDIRECT; k++)
        				in[j].i1[k] = NullReference;
        			for(k = 0; k < N_DOUBLE_INDIRECT; k++)
        				in[j].i2[k] = NullReference;
        		}
        		else if((i == total_blocks - 1) && (j == IPB - 1)){		// If it is the last inode
        			in[j].mode = INODE_FREE;
        			in[j].lnkcnt = 0;
        			in[j].owner = 0;
        			in[j].group = 0;
        			in[j].size = 0;
        			in[j].blkcnt = 0;

        			in[j].next = NullReference;		// Points to null reference, there's no one after

        			in[j].mtime = time;				
        			in[j].ctime = time;				
        			for(k = 0; k < N_DIRECT; k++) 
        				in[j].d[k] = NullReference;
        			for(k = 0; k < N_INDIRECT; k++)
        				in[j].i1[k] = NullReference;
        			for(k = 0; k < N_DOUBLE_INDIRECT; k++)
        				in[j].i2[k] = NullReference;
        		}
        		else{									// the remaining inodes...			
        			in[j].mode = INODE_FREE;
        			in[j].lnkcnt = 0;
        			in[j].owner = 0;
        			in[j].group = 0;
        			in[j].size = 0;
        			in[j].blkcnt = 0;

        			in[j].next = i * IPB + j + 1;		// Calculates the next free inode; block number * Inodes per block + inode number + 1	

        			in[j].mtime = time;			
        			in[j].ctime = time;				
        			for(k = 0; k < N_DIRECT; k++) 
        				in[j].d[k] = NullReference;
        			for(k = 0; k < N_INDIRECT; k++)
        				in[j].i1[k] = NullReference;
        			for(k = 0; k < N_DOUBLE_INDIRECT; k++)
        				in[j].i2[k] = NullReference;
        		}
        	}	
        	soWriteRawBlock(block, in);
        	block++;				
        }
    }
};

