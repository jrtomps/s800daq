
#ifndef RINGITEMFROMFILE_H
#define RINGITEMFROMFILE_H

#include <CRingItem.h>
#include <string>
#include <cstdint>
#include <algorithm>

namespace Utility 
{

  void fillBodyFromFile(CRingItem* pItem, std::string fname);

  template<class T> std::uint8_t* copyIn(std::uint8_t* pDest, T value)
  {
    char* pBytes = reinterpret_cast<char*>(&value);
    return std::copy(pBytes, pBytes+sizeof(value), pDest);
  }

}
#endif
