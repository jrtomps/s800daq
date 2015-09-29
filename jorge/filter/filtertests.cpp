
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
#include <iterator>

using namespace std;

// Overload the insertion operator to allow us to use
// CPPUNIT_ASSERT_EQUAL with any std::vector
template<class T>
std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vec) {
  stream << "{";
  size_t size = vec.size();
  for ( size_t i=0; i<size; ++i) stream << vec.at(i) << ", ";
  stream << "}";
  return stream;
}


template<class T>
void print_vectors(const std::vector<T>& expected, const std::vector<T>& actual)
{
    size_t max = std::max(expected.size(), actual.size());
    for (size_t index=0; index<max; ++index) {
        std::cout << setw(4) << index << " : ";

        std::cout << std::hex;
        std::cout << "0x";
        if (index < expected.size() ) {
		cout << setfill('0');
		cout << setw(4) << expected.at(index);
        } else {
		cout << setfill(' ');
		cout << setw(4) << " ";
        }

	cout << "  ";

        if (index < actual.size() ) {
                cout << "0x";
		cout << setfill('0');
		cout << setw(4) << actual.at(index);
        } else {
		cout << setfill(' ');
		cout << setw(4) << " ";
	} 
        cout << std::dec;
	cout << "\n";
    } 

    cout << endl;

}

std::vector<std::uint16_t> itemToVector(CRingItem* pItem)
{
	std::vector<uint16_t> data;
	uint16_t* pData = reinterpret_cast<uint16_t*>(pItem->getItemPointer());

	size_t nShorts = (*pData)/sizeof(uint16_t);
	data.reserve(nShorts);
	data.insert(data.end(), pData, pData+nShorts);
	
	return data;
}


class S800FilterTest : public CppUnit::TestFixture 
{
  CPPUNIT_TEST_SUITE(S800FilterTest);
  CPPUNIT_TEST(test_0);
  CPPUNIT_TEST(test_1);
  CPPUNIT_TEST(parseCCUSB_0);
  CPPUNIT_TEST(parseVMUSB_0);
  CPPUNIT_TEST(handleScalers_0);
  CPPUNIT_TEST(handleScalers_1);
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
  void test_0() {
    Utility::fillBodyFromFile(pItem, "test_0_in.dat");
    Utility::fillBodyFromFile(pResultItem, "test_0_out.dat");

    CRingItem* pFiltered = pFilter->handlePhysicsEventItem(pItem);

    uint8_t* pExpBytes = reinterpret_cast<uint8_t*>(pResultItem->getItemPointer());
    size_t   nExpectedBytes = pResultItem->getBodySize() + sizeof(BodyHeader) + sizeof(RingItemHeader);
    uint8_t* pFiltBytes = reinterpret_cast<uint8_t*>(pFiltered->getItemPointer());
    size_t   nFilteredBytes = pFiltered->getBodySize() + sizeof(BodyHeader) + sizeof(RingItemHeader);


    size_t nToCompare = min(nExpectedBytes, nFilteredBytes);
    CPPUNIT_ASSERT_MESSAGE(
        "Filter output must match that produced by old S800",
        equal(pExpBytes, pExpBytes + nToCompare, pFiltBytes));

    delete pFiltered;
  }

  void test_1() {
    Utility::fillBodyFromFile(pItem, "test_1_in.dat");
    Utility::fillBodyFromFile(pResultItem, "test_1_out.dat");

    CRingItem* pFiltered = pFilter->handlePhysicsEventItem(pItem);

    uint8_t* pExpBytes = reinterpret_cast<uint8_t*>(pResultItem->getItemPointer());
    size_t   nExpectedBytes = pResultItem->getBodySize() + sizeof(BodyHeader) + sizeof(RingItemHeader);
    uint8_t* pFiltBytes = reinterpret_cast<uint8_t*>(pFiltered->getItemPointer());
    size_t   nFilteredBytes = pFiltered->getBodySize() + sizeof(BodyHeader) + sizeof(RingItemHeader);

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

  void handleScalers_0() {
    uint32_t start = 18;	
    uint32_t stop = 19;	
    time_t now = 0x01234567;
    vector<uint32_t> sclrs;
    sclrs.reserve(32);
    sclrs.push_back(1144309760);
    sclrs.push_back(50355969);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50355969);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50355969);
    sclrs.push_back(50331648);
    sclrs.push_back(50530896);
    sclrs.push_back(50530896);
    sclrs.push_back(50355969);
    sclrs.push_back(50331847);
    sclrs.push_back(50331668);
    sclrs.push_back(50355969);
    sclrs.push_back(50355970);
    sclrs.push_back(50355970);
    sclrs.push_back(50355970);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50355969);
    uint32_t sourceId = 0;
    uint64_t tstamp = 0x12341234;
    uint32_t barrierType = 0;

    vector<uint32_t> expect;
    expect.reserve(sclrs.size()-1);
    transform(sclrs.begin()+1, sclrs.end(), back_inserter(expect), 
                [](const uint32_t& value) {
                  return (0x00ffffff&value);
                });
    unique_ptr<CRingScalerItem> pInputItem(new CRingScalerItem(tstamp, sourceId, 
                                                                barrierType, start, 
                                                                stop, now, 
                                                                sclrs, 1, false));

    CRingItem* pOutputItem = pFilter->handleScalerItem(pInputItem.get());
      
    CRingScalerItem& sclrItem = dynamic_cast<CRingScalerItem&>(*pOutputItem);
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Source id", sourceId, sclrItem.getSourceId() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Timestamp", tstamp, sclrItem.getEventTimestamp() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "barrier", barrierType, sclrItem.getBarrierType() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "sclrs", expect, sclrItem.getScalers() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Start time", start, sclrItem.getStartTime() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "End time", stop, sclrItem.getEndTime() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Unix time", now, sclrItem.getTimestamp() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Body header", true, sclrItem.hasBodyHeader() );
    if (pInputItem.get() != pOutputItem) {
      delete pOutputItem;
    }
  }


  void handleScalers_1() {
    uint32_t start = 18;	
    uint32_t stop = 19;	
    time_t now = 0x01234567;
    vector<uint32_t> sclrs;
    sclrs.reserve(32);
    sclrs.push_back(1144309760);
    sclrs.push_back(50355969);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50355969);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50355969);
    sclrs.push_back(50331648);
    sclrs.push_back(50530896);
    sclrs.push_back(50530896);
    sclrs.push_back(50355969);
    sclrs.push_back(50331847);
    sclrs.push_back(50331668);
    sclrs.push_back(50355969);
    sclrs.push_back(50355970);
    sclrs.push_back(50355970);
    sclrs.push_back(50355970);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50331648);
    sclrs.push_back(50355969);
    uint32_t sourceId = 0;
    uint64_t tstamp = 0x12341234;
    uint32_t barrierType = 0;

    vector<uint32_t> expect;
    expect.reserve(sclrs.size()-1);
    transform(sclrs.begin()+1, sclrs.end(), back_inserter(expect), 
                [](const uint32_t& value) {
                  return (0x00ffffff&value);
                });
    unique_ptr<CRingScalerItem> pInputItem(new CRingScalerItem(start, 
                                                                stop, now, 
                                                                sclrs, false, 
                                                                1));

    CRingItem* pOutputItem = pFilter->handleScalerItem(pInputItem.get());
      
    CRingScalerItem& sclrItem = dynamic_cast<CRingScalerItem&>(*pOutputItem);
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "sclrs", expect, sclrItem.getScalers() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Start time", start, sclrItem.getStartTime() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "End time", stop, sclrItem.getEndTime() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Unix time", now, sclrItem.getTimestamp() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Body header", false, sclrItem.hasBodyHeader() );
    if (pInputItem.get() != pOutputItem) {
      delete pOutputItem;
    }
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
