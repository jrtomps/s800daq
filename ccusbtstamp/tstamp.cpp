


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

        // check that there are at least 5 4-bit words to follow
        //  - event header
        //  - ccusb marker
        //  - ccusb internal scalers
        //  - ulm marker
	if ((evthdr&0x0fff)>=11) {
		pData += sizeof(std::uint16_t); // skip over evt header
                pData += sizeof(std::uint16_t); // skip over marker
                pData += 2*sizeof(std::uint32_t); // skip over event count
                pData += sizeof(std::uint16_t); // skip over ulm marker

		// read in tstamp
		memcpy(&tstamp, pData, sizeof(tstamp)); 
	}

        
	return tstamp;
  }

}
