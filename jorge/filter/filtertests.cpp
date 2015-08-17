
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


    // scintillator 0x4300
    vector<uint16_t> feras(16,0);
    feras[1] = 0x0016; 

    for (int i=0; i<16; i++) {
      string msg = "FERA[" + to_string(i) + "]";
      CPPUNIT_ASSERT_EQUAL_MESSAGE(
          msg.c_str(),
          feras[i], event.fera[i]);
    }

    // ion chamber 0x7164
    vector<uint16_t> ionChamber7164 = {
      0xfff7, 0x001c, 0x002d, 0x0027,
      0x0000, 0x001f, 0x0035, 0x0031, 
      0x003e, 0x0019, 0x0025, 0x0029, 
      0x0036, 0x0032, 0x0030, 0x0027, 
      0x0035};

    for (int i=0; i<17; i++) {
      string msg = "IonChamber[" + to_string(i) + "]";
      CPPUNIT_ASSERT_EQUAL_MESSAGE(
          msg.c_str(),
          ionChamber7164.at(i), 
          event.phillips[0][i]);
    }

    // hodoscope pattern 0x4448
    vector<uint16_t> hodoPattern(2, 0);
    hodoPattern.at(0) = 0;
    hodoPattern.at(1) = 0;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Hodoscope hit pattern 0",
                                 hodoPattern.at(0), 
                                 event.coinc[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Hodoscope hit pattern 1",
                                 hodoPattern.at(1), 
                                 event.coinc[1]);

    // hodo 0-15 0x7165
    vector<uint16_t> hodo0(17, 0);
    hodo0.at(0) = 0;  // both are empty
    for (int i=0; i<17; i++) {
      string msg = "Hodoscope[" + to_string(i) + "]";
      CPPUNIT_ASSERT_EQUAL_MESSAGE(
          msg.c_str(),
          hodo0.at(i), 
          event.phillips[1][i]);
    }

    // hodo 16-32 0x7166
    vector<uint16_t> hodo1(17, 0);
    hodo1.at(0) = 0; 
    for (int i=0; i<16; i++) {
      string msg = "Hodoscope[" + to_string(i+16) + "]";
      CPPUNIT_ASSERT_EQUAL_MESSAGE(
          msg.c_str(),
          hodo1.at(i), 
          event.phillips[2][i]);
    }
  
   // s800 adc 0x7167
   vector<uint16_t> s800Adc(17, 0);
   s800Adc.at(0) = 0x0000;
   for (int i=0; i<17; i++) {
     string msg = "S800ADC[" + to_string(i) + "]";
     CPPUNIT_ASSERT_EQUAL_MESSAGE(
         msg.c_str(),
         s800Adc.at(i), 
         event.phillips[3][i]);
   }

   // s800 tdc 0x7186
   vector<uint16_t> s800TDC(17, 0);
   s800TDC.at(0) = 0x1800;
   s800TDC.at(1) = 0xb5c9;
   s800TDC.at(2) = 0xc4f2;
   for (int i=0; i<17; i++) {
     string msg = "s800TDC[" + to_string(i) + "]";
     CPPUNIT_ASSERT_EQUAL_MESSAGE(
         msg.c_str(),
         s800TDC.at(i), 
         event.phillips[4][i]);
   }

   // s800 labr tdc 0x7187
   vector<uint16_t> laBr(17, 0);
   laBr.at(0) = 0;
   for (int i=0; i<17; i++) {
     string msg = "laBr[" + to_string(i) + "]";
     CPPUNIT_ASSERT_EQUAL_MESSAGE(
         msg.c_str(),
         laBr.at(i), 
         event.phillips[5][i]);
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
