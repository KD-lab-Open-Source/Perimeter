#include "StdAfx.h"

#include <process.h>
#include <crtdbg.h>
#include "Statistics.h"

///////////////////////////////////////////////////////////////////////
//	Statics
///////////////////////////////////////////////////////////////////////

AllocationStatistics AllocationStatistics::dbg_hook_data;
static AllocationStatistics allocation_data_stamp;

static int total_memory_stamp = 0;

#ifdef _DEBUG
struct MemoryStatisticsAutoStart
{
	MemoryStatisticsAutoStart(){ allocation_tracking("MemoryStatisticsAutoStart"); }
} memoryStatisticsAutoStart;
#endif

///////////////////////////////////////////////////////////////////////
//	Memory Hook
///////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define nNoMansLandSize 4
typedef struct _CrtMemBlockHeader
{
        struct _CrtMemBlockHeader * pBlockHeaderNext;
        struct _CrtMemBlockHeader * pBlockHeaderPrev;
        char *                      szFileName;
        int                         nLine;
        size_t                      nDataSize;
        int                         nBlockUse;
        long                        lRequest;
        unsigned char               gap[nNoMansLandSize];
        /* followed by:
         *  unsigned char           data[nDataSize];
         *  unsigned char           anotherGap[nNoMansLandSize];
         */
} _CrtMemBlockHeader;

#define pHdr(pbData) (((_CrtMemBlockHeader *)pbData)-1)

#if _MSC_VER == 1100
int __cdecl AllocationTrackingHook(  int  nAllocType,  void   * pvData,  unsigned int   nSize,  int      nBlockUse,  long     lRequest,  const char * szFileName,  int      nLine  )
#else
int __cdecl AllocationTrackingHook(  int  nAllocType,  void   * pvData,  unsigned int   nSize,  int      nBlockUse,  long     lRequest,  const unsigned char * szFileName,  int      nLine  )
#endif
{
	AllocationStatistics::dbg_hook_data.operations++;
	switch(nAllocType){
		case _HOOK_REALLOC:
			{
				_CrtMemBlockHeader *pHead=pHdr(pvData);
				int dSize=(int)nSize-pHead->nDataSize;
				AllocationStatistics::dbg_hook_data.size += dSize;
				break;
			}
		case _HOOK_ALLOC:   
			{
				AllocationStatistics::dbg_hook_data.size += nSize;
				AllocationStatistics::dbg_hook_data.blocks++;
				break;
			}
		case _HOOK_FREE:   
			{
				_CrtMemBlockHeader *pHead = pHdr(pvData);
				nSize = pHead->nDataSize;
				AllocationStatistics::dbg_hook_data.size -= nSize;
				AllocationStatistics::dbg_hook_data.blocks--;
				break;
			}
		default:{
			_CrtMemBlockHeader *pHead=pHdr(pvData);
			break;
			}

   }

   return 1;         // Allow the memory operation to proceed
}
#endif //_DEBUG

///////////////////////////////////////////////////////////////////////
//	Count by pages
///////////////////////////////////////////////////////////////////////

int total_memory_used()
{
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	int size = 0;
	MEMORY_BASIC_INFORMATION Buffer;
	VirtualQuery(SystemInfo.lpMinimumApplicationAddress, &Buffer,  sizeof(Buffer) );
	while(Buffer.BaseAddress < SystemInfo.lpMaximumApplicationAddress){
		if(Buffer.State == MEM_COMMIT && !(Buffer.Type & MEM_MAPPED) && Buffer.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READ) )
			size += Buffer.RegionSize;
		void* prev_address = Buffer.BaseAddress; 
		VirtualQuery((char*)Buffer.BaseAddress + Buffer.RegionSize, &Buffer,  sizeof(Buffer) );
		if(prev_address == Buffer.BaseAddress)
			break;
		}
	return size;
}


///////////////////////////////////////////////////////////////////////
//	Tracking System
///////////////////////////////////////////////////////////////////////
void allocation_tracking(char* title)
{
	allocation_tracking(title, 0);
}

void allocation_tracking(char* title, AllocationAccumulator* accumulator)
{
	static ofstream out;
	static int opened, disabled;
	if(disabled)
		return;
	if(!opened){
		opened = 1;

		disabled = 1;
		for(int i = 1;i < __argc;i++)
			if(strstr(__argv[i], "memory"))
				disabled = 0;
		if(disabled)
			return;

		#ifdef _DEBUG
		_CrtSetAllocHook( &AllocationTrackingHook );
		#endif
		out.open("memory");
		out << "\n\tMemory allocation listing.\n\t";
		time_t aclock;
		time( &aclock );                 // Get time in seconds
		out << asctime( localtime( &aclock ) ) << "\n\n";
		out << "--------------------------------------------------------------------------------------------------------------------------------------\n";
		out << "|                           |         |       Actual Application Size        |  Total malloc (if DEBUG)  |  Title malloc (if DEBUG)  | \n";
		out << "|        Title              |   Time  |                                      |                           |                           |\n";
		out << "|                           |         |    Total       Title     Accumulated |     Size   Blocks  BSize  |     Size   Blocks  BSize  |\n";
		out << "--------------------------------------------------------------------------------------------------------------------------------------\n";
		}						  

	int total_memory = total_memory_used();
	int total_size = total_memory - total_memory_stamp;
	int accumulated_size = 0;
	int size = AllocationStatistics::dbg_hook_data.size - allocation_data_stamp.size;
	int cnt = AllocationStatistics::dbg_hook_data.blocks - allocation_data_stamp.blocks;

	if(total_memory != total_memory_stamp || size || cnt){
		if(accumulator){
			size = accumulator -> size += size;
			cnt = accumulator -> blocks += cnt;
			accumulated_size = accumulator -> total_size += total_size;
			}

		char buf[10000];
		sprintf(buf, "| %-25.25s | %7.3f | %10i | %10i | %10i | %10i  %5i  %6i | %10i  %5i  %6i |", title, clockf()/1000.,
			total_memory, total_size,  accumulated_size,
			AllocationStatistics::dbg_hook_data.size, AllocationStatistics::dbg_hook_data.blocks, (AllocationStatistics::dbg_hook_data.blocks ? AllocationStatistics::dbg_hook_data.size/AllocationStatistics::dbg_hook_data.blocks : 0), 
			size, cnt, (cnt ? size/cnt : 0) );

		out << buf << "\n";

		allocation_data_stamp.size = AllocationStatistics::dbg_hook_data.size; 
		allocation_data_stamp.blocks = AllocationStatistics::dbg_hook_data.blocks;
		total_memory_stamp = total_memory;
		}
}



