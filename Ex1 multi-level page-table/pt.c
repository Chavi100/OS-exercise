
#include "os.h"


void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn)
{
	uint64_t arr[5];
	uint64_t user=vpn>>12;
	uint64_t mask=0x1ff;
	for(int i=0;i<5;i++)
	{
		arr[4-i]=user&mask;
	 	user=user>>9;
	}
	
	uint64_t*ptr;
	uint64_t destroy=~0x1;
	if (ppn==NO_MAPPING)
	{
		ptr=phys_to_virt(pt<<12);
		for(int i=0;i<5;i++)
		{ 
			if (i==4)
			{
				ptr[arr[i]]=ptr[arr[i]]&destroy;
			}
		  pt=ptr[arr[i]];
		  ptr=phys_to_virt(pt&destroy); 
		  
		}
	}
	else
	{
	  ptr=phys_to_virt(pt<<12);
	  for(int i=0;i<4;i++)
	  {
	  	
	  	if(ptr[arr[i]]==0)
	  	{
	      ptr[arr[i]]=alloc_page_frame();
	      ptr[arr[i]]=(ptr[arr[i]]<<12)|0x1;
	    }
	    ptr=phys_to_virt(ptr[arr[i]]&destroy);
	  
	  }
	   ptr[arr[4]]=(ppn<<12)|0x1;
	}

}



uint64_t page_table_query(uint64_t pt, uint64_t vpn)
{
	uint64_t arr[5];
	uint64_t user=vpn>>12;
	uint64_t mask=0x1ff;
	for(int i=0;i<5;i++)
	{
		arr[4-i]=user&mask;
	 	user=user>>9;
	}

	uint64_t * ptr;
	uint64_t destroy=~0x1;
	ptr=phys_to_virt(pt<<12);
	for (int i=0;i<5;i++)
	{
		if( (ptr[arr[i]]&0x1)==0)
		{
			return NO_MAPPING;

		}
		else
		{
			if (i!=4)
			{
				ptr=phys_to_virt(ptr[arr[i]]&destroy);
			}	
		
		}
	
	
	}
	
	return ptr[arr[4]]>>12;



}






















