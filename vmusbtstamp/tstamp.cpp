


#include <DataFormat.h>
#include <cstdint>
#include <cstring>

extern "C" {

  uint64_t getEventTimestamp(void* pBuffer) 
  {
	std::uint16_t evthdr=0;
	std::uint64_t tstamp=0;

	const char* pData = reinterpret_cast<const char*>(pBuffer);

	// read in the event hdr
        memcpy(&evthdr, pData, sizeof(evthdr));

        // check that there are at least 5 16-bit words to follow
        // 1 marker and 4 timestamp words
	if ((evthdr&0x0fff)>=5) {
		pData += sizeof(std::uint16_t); // skip over evt header
                pData += sizeof(std::uint16_t); // skip over marker

		// read in tstamp
		memcpy(&tstamp, pData, sizeof(tstamp)); 
	}

        
	return tstamp;
  }

}
