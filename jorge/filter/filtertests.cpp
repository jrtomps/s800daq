
#include <cppunit/extensions/HelperMacros.h>

#include <CPhysicsEventItem.h>
#include <CRingItemFactory.h>
#include <DataFormat.h>
#include <RingItemFromFile.h>

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
  CPPUNIT_TEST(parseVMUSB_0);
  CPPUNIT_TEST(formatData_0);
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
    pFilter->setRegisterIs24Bit(false);
    pFilter->setPhillipsIs24Bit(false);
  }

  void tearDown() {
    delete pItem;
    delete pResultItem;
    delete pFilter;
  }

  //
  void test() {
    Utility::fillBodyFromFile(pItem, "test_0_in.dat");
    Utility::fillBodyFromFile(pResultItem, "test_0_out.dat");

    CRingItem* pFiltered = pFilter->handlePhysicsEventItem(pItem);

    uint8_t* pExpBytes = reinterpret_cast<uint8_t*>(pResultItem->getItemPointer());
    size_t   nExpectedBytes = pResultItem->getBodySize() + sizeof(BodyHeader) + sizeof(RingItemHeader);
    uint8_t* pFiltBytes = reinterpret_cast<uint8_t*>(pFiltered->getItemPointer());
    size_t   nFilteredBytes = pFiltered->getBodySize() + sizeof(BodyHeader) + sizeof(RingItemHeader);
    cout << "Expected " << pResultItem->toString() << endl;
    cout << "Actual " << pFiltered->toString() << endl;

    size_t nToCompare = min(nExpectedBytes, nFilteredBytes);
    CPPUNIT_ASSERT_MESSAGE(
        "Filter output must match that produced by old S800",
        equal(pExpBytes, pExpBytes + nToCompare, pFiltBytes));

    delete pFiltered;
  }


  //
  void parseCCUSB_0() {
    Utility::fillBodyFromFile(pItem, "parseCCUSB_0_in.dat");

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
    comparePhillips( event.phillips[0], ionChamber7164, "IonChamber");

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
    hodo0.at(0) = 0x0102; 
    hodo0.at(2) = 0x000d; 
    hodo0.at(9) = 0x0011; 
    comparePhillips( event.phillips[1], hodo0, "Hodoscope0");

    // hodo 16-32 0x7166
    vector<uint16_t> hodo1(17, 0);
    hodo1.at(0) = 0; 
    comparePhillips( event.phillips[2], hodo1, "Hodoscope1");
  
   // s800 adc 0x7167
   vector<uint16_t> s800Adc(17, 0);
   s800Adc.at(0) = 0x0000;
   comparePhillips( event.phillips[3], s800Adc, "S800ADC");

   // s800 tdc 0x7186
   vector<uint16_t> s800TDC(17, 0);
   s800TDC.at(0) = 0x1800;
   s800TDC.at(12) = 0x05c9;
   s800TDC.at(13) = 0x04f2;
   comparePhillips( event.phillips[4], s800TDC, "S800TDC");

   // s800 labr tdc 0x7187
   vector<uint16_t> laBr(17, 0);
   laBr.at(0) = 0;
   comparePhillips( event.phillips[5], laBr, "LaBr");
  }

  void parseVMUSB_0() {
    Utility::fillBodyFromFile(pItem, "parseVMUSB_0_in.dat");

    EventType event;
    uint16_t* pBody = reinterpret_cast<uint16_t*>(pItem->getBodyPointer());
    int status = pFilter->parseData(pItem->getSourceId(),
                                    pItem->getBodySize()/sizeof(uint16_t),
                                    pItem->getEventTimestamp(),
                                    pBody,
                                    &event);

    
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Number of CRDC1 pads",  
                                  uint32_t(0), event.npads[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Number of CRDC2 pads",  
                                  uint32_t(0), event.npads[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Number of PPAC pads",  
                                  uint32_t(6), event.npads[2]);

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "PPAC #0 d[0]",
        uint16_t(0x006d), event.det_pads[2][0].d[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "PPAC #0 d[1]",
        uint16_t(0x0000), event.det_pads[2][0].d[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "PPAC #0 d[2]",
        uint16_t(0x0000), event.det_pads[2][0].d[2]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "PPAC #0 d[3]",
        uint16_t(0x026a), event.det_pads[2][0].d[3]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "PPAC #0 sch",
        uint16_t(0x05ec), event.det_pads[2][0].sch);
   
    // no need to check every instance 

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "PPAC #2 d[5]",
        uint16_t(0x0067), event.det_pads[2][2].d[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "PPAC #2 d[1]",
        uint16_t(0x0000), event.det_pads[2][2].d[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "PPAC #2 d[2]",
        uint16_t(0x0000), event.det_pads[2][2].d[2]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "PPAC #2 d[3]",
        uint16_t(0x0000), event.det_pads[2][2].d[3]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "PPAC #2 sch",
        uint16_t(0x066c), event.det_pads[2][2].sch);
   
    // no need to check every instance 

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "PPAC #5 d[5]",
        uint16_t(0x0000), event.det_pads[2][5].d[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "PPAC #5 d[1]",
        uint16_t(0x00b4), event.det_pads[2][5].d[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "PPAC #5 d[2]",
        uint16_t(0x0000), event.det_pads[2][5].d[2]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "PPAC #5 d[3]",
        uint16_t(0x0000), event.det_pads[2][5].d[3]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "PPAC #5 sch",
        uint16_t(0x06fc), event.det_pads[2][5].sch);

  }

  void formatData_0() {
  }

  void comparePhillips(const uint16_t* actual, const std::vector<uint16_t>& expected, std::string context)
  {
    for (int i=0; i<17; i++) {
      string msg = context + "[" + to_string(i) + "]";
      CPPUNIT_ASSERT_EQUAL_MESSAGE(
          msg.c_str(),
          expected.at(i), 
          actual[i]);
    }
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION(S800FilterTest);
