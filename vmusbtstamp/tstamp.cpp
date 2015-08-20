// Modified June 2015, JP


#include <DataFormat.h>
#include <cstdint>
#include <cstring>

extern "C" {

  uint64_t getEventTimestamp(void *pBuffer) 
  {
	std::uint16_t evthdr=0;
	std::uint64_t tstamp=0;

	const char* pData = reinterpret_cast<const char*>(pBuffer);

	// read in the event hdr
        memcpy(&evthdr, pData, sizeof(evthdr));

        // check that there are at least 10 16-bit words to follow after
	// the stack header:
	//	
	// 1 x 16-bit VM-USB tag
        // 2 x 32-bit VM-USB scalers A and B
        // 1 x 16-bit Timestamp start flag
	// 64-bit Timestamp
	if ((evthdr&0x0fff)>=10) {
		pData += sizeof(std::uint16_t); // skip over evt header
                pData += sizeof(std::uint16_t); // skip over VM-USB marker
                pData += 2*sizeof(std::uint32_t); // skip over VM-USB scalers
                pData += sizeof(std::uint16_t); // skip over timestamp marker
		// read in tstamp
		memcpy(&tstamp, pData, sizeof(tstamp)); 
	}

        
	return tstamp;
  }

  uint64_t getScalerTimestamp(void *pBuffer) {
	return 0;
  }

}
