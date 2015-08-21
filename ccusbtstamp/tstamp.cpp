


#include <DataFormat.h>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iomanip>

using namespace std;

uint64_t last_timestamp = 0;


extern "C" {

  
  uint64_t getEventTimestamp(void* pBuffer) 
  {
        int i;
	std::uint16_t evthdr=0;
	std::uint64_t temp=0;
	std::uint64_t tstamp=0;

	const char* pData = reinterpret_cast<const char*>(pBuffer);

	/* read in the event hdr */
        memcpy(&evthdr, pData, sizeof(evthdr));

        /* 
	   Check that there are at least 16 16-bit words to follow
	   the stack header:
		
	   1 x 16-bit CC-USB tag
           2 x 32-bit CC-USB scalers A and B
           1 16-bit ULM trigger tag
	   2 16-bit ULM trigger bits
	   4 x 32-bit (128-bit timestamp string)      
	*/

	if ((evthdr&0x0fff)>=16) {
		pData += sizeof(std::uint16_t); // skip over evt header
                pData += sizeof(std::uint16_t); // skip over CC-USB marker (0xC800)
                pData += 2*sizeof(std::uint32_t); // skip over CC-USB scalers
                pData += sizeof(std::uint16_t); // skip over ULM marker
                pData += 2*sizeof(std::uint16_t); // skip over ULM trigger bits
		/* read in tstamp */
		//memcpy(&tstamp, pData, sizeof(tstamp)); 

		for (i=0; i<4; i++) {
		  temp = 0;
		  memcpy(&temp, pData, sizeof(uint32_t)); 
		  
      		  tstamp = tstamp + ((temp & 0xffff)<<(16*i));
		  pData += sizeof(std::uint32_t);

		}
                
                if (tstamp <= last_timestamp) {
		  std::cout << "CCUSB timestamp jumped BACKWARDS from: " 
			    << "\nhex: " << std::hex << std::setw(12) << last_timestamp << " --> " << std::setw(12) << tstamp
		            << "\ndec:" << std::dec << std::setw(12) << last_timestamp << " --> " << std::setw(12) << tstamp 
			    << std::endl;
		}
                last_timestamp = tstamp;		
	}

        
	return tstamp;
  }






  uint64_t getScalerTimestamp(void* pBuffer) {

    return last_timestamp; //use ULM timestamp from last physicis event

  }
}
