

#ifndef CCAESARFILTER_H
#define CCAESARFILTER_H

//#include <utility>
//#include <map>
//#include <vector>
//#include <stdint.h>
//#include <stddef.h>
//#include "CFilter.h"
//#include "fragment.h"



#include <stdint.h>
#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <CFilter.h>
#include <DataFormat.h>
#include <CRingItem.h>
#include <CRingStateChangeItem.h>
#include <CPhysicsEventItem.h>
#include <CRingTextItem.h>
#include <CRingItemFactory.h>
#include "FragmentIndex.h"
#include <CRingPhysicsEventCountItem.h>
#include <CRingFragmentItem.h>



#define CCUSB 0xC800
#define VMUSB 0xE800


#define S800_PACKET 0x5800

#define S800_EVENT_NUMBER_PACKET 0x5804
#define S800_TIMESTAMP_PACKET 0x5803
#define S800_TIMESTAMP_PACKET_VM 0x6803
#define S800_TIMESTAMP_PACKET_CC 0x7803


#define PACKET_S800 0x5800
#define VERSION_S800 0x0005

#define PACKET_TIMESTAMP 0x5803
#define PACKET_EVENT_NUMBER 0x5804
#define PACKET_TRIGGER 0x5801
#define PACKET_TOF 0x5802
#define PACKET_FP_SCINT 0x5810

#define PACKET_FP_IC 0x5820
#define SUBPACKET_FP_IC_ENERGY 0x5821

#define PACKET_CRDC 0x5840
#define PACKET_CRDC1 0x6840
#define PACKET_CRDC2 0x7840
#define SUBPACKET_FP_CRDC_RAW 0x5841
#define SUBPACKET_FP_CRDC_ANODE 0x5845

#define PACKET_FP_HODO 0x58B0
#define PACKET_FP_HODO1 0x68B0
#define PACKET_FP_HODO2 0x78B0
#define PACKET_FP_HODO3 0x88B0

#define PACKET_II_TRACK 0x5870
#define SUBPACKET_II_TRACK_RAW 0x5871

#define PACKET_OB_PIN 0x58A0
#define PACKET_GALOTTE 0x58D0
#define PACKET_LABR 0x58E0

#define PACKET_MTDC 0x58F0












#define XLM_TIMESTAMP_TAG 0x5803
#define XLM_TIMESTAMP_ETAG 0xF803

#define XLM_CRDC1_TAG 0xCFDC
#define XLM_CRDC1_ETAG 0xFFDC

#define XLM_CRDC2_TAG 0xCFDD
#define XLM_CRDC2_ETAG 0xFFDD

#define XLM_PPAC_TAG 0x5870
#define XLM_PPAC_ETAG 0xF870

#define MTDC_TAG 0x0DDC
#define MTDC_ETAG 0xFDDC

#define ULM_TAG 0x2367
#define ULM_ETAG 0xF367

#define FERA_TAG 0x4300
#define FERA_ETAG 0xF300

#define PHILLIPS_ADC_IONCHAMBER_TAG 0x7164
#define PHILLIPS_ADC_IONCHAMBER_ETAG 0xF164

#define REGISTER_TAG 0x4448
#define REGISTER_ETAG 0xF448

#define PHILLIPS_ADC_HODOSCOPE1_TAG 0x7165
#define PHILLIPS_ADC_HODOSCOPE1_ETAG 0xF165

#define PHILLIPS_ADC_HODOSCOPE2_TAG 0x7166
#define PHILLIPS_ADC_HODOSCOPE2_ETAG 0xF166

#define PHILLIPS_ADC_S800_TAG 0x7167
#define PHILLIPS_ADC_S800_ETAG 0xF167

#define PHILLIPS_TDC_S800_TAG 0x7186
#define PHILLIPS_TDC_S800_ETAG 0xF186

#define PHILLIPS_TDC_LABR_TAG 0x7187
#define PHILLIPS_TDC_LABR_ETAG 0xF187

#define VMUSB_SCALER_TAG 0x1234
 
#define CCUSB_SCALER_TAG 0x4434





#define WINDOW 20 /* Timestamp offset between CC- and VM-USB controllers */

#define MAXNF 10 /* Maximum number of fragments (at present we only expect 2, but this may change in the future) */

#define NEVNUMWORDS 3 /* Number of 16-bit event-number words */
#define NTSWORDS 4 /* Number of 16-bit timestamp words */

#define NCRDCPADS 64 /* crdcs are read out in groups of four, for a total of 64 x 4 = 256 channels */
#define NCRDCSAMPLES 8

#define NPPACSTRIPS 64
#define NPPACPLANES 2
#define NPPACS 2
#define NPPACSAMPLES 8

#define MAXPADS 2048 



const int S800_TRIGGER_FIRST=8;
const int S800_TRIGGER_CHANNELS=4;

const int S800_TOF_FIRST=12;
const int S800_TOF_CHANNELS=4;

const int S800_FP_SCINT_FIRST=0;
const int S800_FP_SCINT_CHANNELS=3;

const int S800_FP_IC_FIRST=0;
const int S800_FP_IC_CHANNELS=16;

const int S800_FP_CRDC1_ANODE=1;
const int S800_FP_CRDC2_ANODE=2;
const int S800_FP_CRDC1_TAC=3;
const int S800_FP_CRDC2_TAC=4;

const int S800_FP_HODO_FIRST=0;
const int S800_FP_HODO_CHANNELS=16;
const int S800_FP_HODO_TAC=15;

const int S800_OB_PIN=0;

const int S800_GALOTTE_FIRST=9;
const int S800_GALOTTE_CHANNELS=5;

const int S800_LABR_FIRST=0;
const int S800_LABR_CHANNELS=4;




struct pad_type
{
  uint16_t sch;
  uint16_t d[4];
};

struct mesytec_header_type
{
  uint16_t hsig;
  uint16_t subheader;
  uint16_t module_id;
  uint16_t format;
  uint16_t adc_res;
  uint16_t nwords;

  mesytec_header_type();
};

struct mesytec_ender_type
{
  uint16_t esig;
  uint32_t tcts;

  mesytec_ender_type();
};

struct mesytecTDC_type
{
  struct mesytec_header_type header;
  uint16_t data[32][200];
  uint16_t hits[32];
  struct mesytec_ender_type ender;

  mesytecTDC_type();
};


struct EventType
{
  uint16_t trigger_pattern;
  uint16_t fera[16];

  uint16_t phillips[6][17]; // 0: Ion chamber ADC, 1: hodoscope 1-16 ADC, 2: hodoscope 17-32 ADC, 3: S800 CRDC's anodes + TACs, 4: S800 TDC, 5: LaBr TDC
  struct pad_type det_pads[3][MAXPADS]; //0: CRDC1, 1: CRDC2, 2: PPACs
  uint32_t npads[3]; //0: CRDC1, 1: CRDC2, 2: PPACs
  uint16_t tdc[17];
  uint16_t labrtdc[17];
  uint16_t coinc[2];
  struct mesytecTDC_type mtdc;
  uint16_t timestamp_bit[2][NTSWORDS]; // 0: timestamp from VM-USB, 1: timestamp from CC-USB
  uint64_t timestamp[2];               // 0: timestamp from VM-USB, 1: timestamp from CC-USB
  uint16_t evnum_bit[2][NEVNUMWORDS]; // 0: event number from VM-USB, 1: event number from CC-USB
  uint64_t evnum;

  
  EventType();
};





class CS800Filter : public CFilter
{
    private:
       bool m_isBuilt;
       bool m_ulm24;
       bool m_phillips24;
       bool m_register24;
       std::map<uint16_t,std::vector<uint16_t> > m_sortedData;
       std::map<uint16_t,bool> m_found; // Map to keep track of tags found
       int m_eventCount;
       int m_eventCountLastError;
       std::map<uint16_t,uint16_t> m_error;


    public:
       CS800Filter(bool isbuilt);
       CS800Filter(const CS800Filter& rhs);
       CS800Filter& operator=(const CS800Filter& rhs);
    
       virtual CFilter* clone() const { return new CS800Filter(*this); }
    
       virtual CRingItem* handlePhysicsEventItem(CPhysicsEventItem* item);
       virtual CRingItem* handleScalerItem(CRingScalerItem* item);
       
       void setBuiltFlag(bool isbuilt) { m_isBuilt = isbuilt; } 
       void setULMIs24Bit(bool is24Bit) { m_ulm24 = is24Bit; }
       void setPhillipsIs24Bit(bool is24Bit) { m_phillips24 = is24Bit; }
       void setRegisterIs24Bit(bool is24Bit) { m_register24 = is24Bit; }

       virtual void finalize();




    private:
       int parseData(uint32_t hid, size_t size, uint64_t htime, uint16_t* data, EventType* pEvent);
       int countTrue(std::map<uint16_t,bool> mmap);
       uint16_t* DecodeXLMTimestamp(uint16_t* pTimedata, EventType* pXLMTime, int flag, int status);
       uint16_t* DecodeULMTimestamp(uint16_t* pTimedata, EventType* pULMTime, int flag, int status);
       uint16_t* DecodeXLMpads(uint16_t* pPadData, EventType* pPad, int det, int status);
       uint16_t* DecodeMTDC(uint16_t* pMTDCdata, EventType* pMTDC, int status);
       uint16_t* DecodeFERAADC(uint16_t* pFERAdata, EventType* pFERA, int status);
       uint16_t* DecodePhillips(uint16_t* pPhillipsADCdata, EventType* pPhillipsADC, int flag, int status);
       void FormatData(int status, EventType* pEvent);
       void PublishData(CRingItem* item);
       uint16_t* appendKeyedData(uint16_t* pData, uint16_t keyoverwrite, uint16_t key);
       
       
       //int parseData(CRingItem* item);
       //int parseData(uint16_t* pData, int nbytes);
       //uint16_t* handleBoundedData(uint16_t marker, uint16_t* pData, int& nbytes);
       //void formatData(CRingItem* item);
       //uint16_t* appendKeyedData(uint16_t key, uint16_t* pData, bool prependSize);
       //void trimAndEvaluateDataIntegrity(); 
       //bool trimAndEvaluateDataIntegrity(uint16_t key); 
       //bool isFERAHeader(uint16_t value);
       //uint16_t countWords();
       
       //CRingItem* wrapBodyInBadPacket(CRingItem* item);
       //int countTrue(std::map<uint16_t,bool> mmap);
};

#endif
