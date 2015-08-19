#include <RingItemFromFile.h>

#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

namespace Utility 
{

void fillBodyFromFile(CRingItem* pItem, std::string fname)
{
  uint8_t* pBody = reinterpret_cast<uint8_t*>(pItem->getItemPointer());    
  pBody += 2*sizeof(uint32_t);

  uint32_t bodyHeaderSize = 20;
  pBody = copyIn(pBody, bodyHeaderSize);

  ifstream file(fname.c_str());

  uint64_t tstamp;
  file >> hex >> tstamp;
  pBody = copyIn(pBody, tstamp);

  uint32_t id;
  file >> hex >> id;

  pBody = copyIn(pBody, id);

  uint32_t barrier;
  file >> hex >> barrier;

  pBody = copyIn(pBody, barrier);

  while (1) {
    uint16_t temp;
    file >> hex >> temp;
    if ( ! file.good() ) {
      break;
    } else {
      pBody = copyIn(pBody, temp);  
    }

  }
  pItem->setBodyCursor(pBody);
  pItem->updateSize();
}

} // end Utility
