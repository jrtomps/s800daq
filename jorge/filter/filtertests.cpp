
#include <cppunit/extensions/HelperMacros.h>

#include <CPhysicsEventItem.h>
#include <CRingItemFactory.h>
#include <DataFormat.h>

#define private public
#include <CS800Filter.h>
#undef private

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;

class S800FilterTest : public CppUnit::TestFixture 
{
  CPPUNIT_TEST_SUITE(S800FilterTest);
  CPPUNIT_TEST(test);
  CPPUNIT_TEST(parseCCUSB_0);
  CPPUNIT_TEST_SUITE_END();

  private:
  CPhysicsEventItem* pItem;
  CPhysicsEventItem* pResultItem;
  CS800Filter* pFilter;

  public:
  void setUp() {
    pItem = new CPhysicsEventItem;
    pResultItem = new CPhysicsEventItem;
    pFilter = new CS800Filter(true);
  }

  void tearDown() {
    delete pItem;
    delete pResultItem;
    delete pFilter;
  }

  void test() {
    fillBodyFromFile(pItem, "test_0_in.dat");
    fillBodyFromFile(pResultItem, "test_0_out.dat");

    CRingItem* pFiltered = pFilter->handlePhysicsEventItem(pItem);

    uint8_t* pExpBytes = reinterpret_cast<uint8_t*>(pResultItem->getItemPointer());
    size_t   nExpectedBytes = pResultItem->getBodySize() + sizeof(BodyHeader) + sizeof(RingItemHeader);
    uint8_t* pFiltBytes = reinterpret_cast<uint8_t*>(pFiltered->getItemPointer());
    size_t   nFilteredBytes = pFiltered->getBodySize() + sizeof(BodyHeader) + sizeof(RingItemHeader);

//    cout << pFiltered->toString() << endl;
//    cout << pResultItem->toString() << endl;

    size_t nToCompare = min(nExpectedBytes, nFilteredBytes);
    CPPUNIT_ASSERT_MESSAGE(
  "Filter output must match that produced by old S800",
  equal(pExpBytes, pExpBytes + nToCompare, pFiltBytes));

    delete pFiltered;
  }

  void parseCCUSB_0() {
    fillBodyFromFile(pItem, "parseCCUSB_0_in.dat");
    cout << pItem->toString() << endl;
  
    EventType event;
    uint16_t* pBody = reinterpret_cast<uint16_t*>(pItem->getBodyPointer());
    int status = pFilter->parseData(pItem->getSourceId(),
            pItem->getBodySize()/sizeof(uint16_t),
            pItem->getEventTimestamp(),
            pBody,
            &event);
      
    CPPUNIT_ASSERT_EQUAL_MESSAGE("ULM Trigger pattern",
                    uint16_t(0x0010), event.trigger_pattern);

    uint16_t feras[] = {
    0x8800, 0x0816, 0x7164, 0xfff7, 0x001c, 0x102d, 0x2027,
    0x401f, 0x5035, 0x6031, 0x703e, 0x8019, 0x9025, 0xa029,
    0xb036, 0xc032, 0xd030, 0xe027, 0xf035};
    
    for (int i=0; i<16; i++) {
       string msg = "FERA[" + to_string(i) + "]";
       CPPUNIT_ASSERT_EQUAL_MESSAGE(
                       msg.c_str(),
                       feras[i], event.fera[i]);
    }

  }

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
    cout << "Tstamp = " << tstamp << dec << endl;

    uint32_t id;
    file >> hex >> id;
        cout << hex << id << dec << endl;

    pBody = copyIn(pBody, id);

    uint32_t barrier;
    file >> hex >> barrier;
        cout << hex << barrier << dec << endl;

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

  template<class T> uint8_t* copyIn(uint8_t* pDest, T value)
  {
    char* pBytes = reinterpret_cast<char*>(&value);
    return copy(pBytes, pBytes+sizeof(value), pDest);
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION(S800FilterTest);
