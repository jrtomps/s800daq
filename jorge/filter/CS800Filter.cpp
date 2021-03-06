#include "CS800Filter.h"
#include <CPhysicsEventItem.h>
#include <CRingItemFactory.h>
#include <DataFormat.h>
#include <Actions.h>

#include <Exception.h>

#include <iostream>
#include <iomanip>
#include <exception>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <limits>

#include <FragmentIndex.h>



  mesytec_header_type::mesytec_header_type()
: hsig(0), 
  subheader(0),
  module_id(0),
  format(0), 
  adc_res(0), 
  nwords(0) 
{}

  mesytec_ender_type::mesytec_ender_type()
: esig(0), 
  tcts(0)
{}

  mesytecTDC_type::mesytecTDC_type()
: header(), ender()
{
  for (size_t i=0; i<32; ++i) {
//    std::fill(data[i],  data[i]+sizeof(data[i])/sizeof(uint16_t), 0);
    hits[i] = 0;
  }
}

  EventType::EventType() 
: trigger_pattern(0), 
  mtdc(), 
  evnum(0)
  {
    std::fill(fera, fera+sizeof(fera)/sizeof(uint16_t), 0); 

    for (size_t i=0; i<6; ++i) {
      std::fill(phillips[i], phillips[i]+17, 0);
    }

    // no need to initialize pads b/c the data is compressed in the 
    // lower indices of the array and we only deal with that data

    std::fill(npads,   npads+sizeof(npads)/sizeof(uint32_t), 0);
    std::fill(tdc,     tdc+sizeof(tdc)/sizeof(uint16_t), 0);
    std::fill(labrtdc, labrtdc+sizeof(labrtdc)/sizeof(uint16_t), 0);
    std::fill(coinc,   coinc+sizeof(coinc)/sizeof(uint16_t), 0);
    
    std::fill(timestamp_bit[0], timestamp_bit[0]+NTSWORDS, 0);
    std::fill(timestamp_bit[1], timestamp_bit[1]+NTSWORDS, 0);
    std::fill(timestamp,    timestamp+2, 0);
    std::fill(evnum_bit[0], evnum_bit[0]+NEVNUMWORDS, 0);
    std::fill(evnum_bit[1], evnum_bit[1]+NEVNUMWORDS, 0);
    
  }


  CS800Filter::CS800Filter(bool isbuilt) 
: m_isBuilt(isbuilt), 
  m_ulm24(false),
  m_phillips24(true),
  m_register24(true),
  m_sortedData(), 
  m_found(),
  m_eventCount(0),
  m_prevtstampvmusb(0),
  m_prevtstampccusb(0),
  m_prevevnum(0),
  m_eventCountLastError(0),
  m_BOR(false),
  m_EOR(false),
  m_error()
{}

  CS800Filter::CS800Filter(const CS800Filter& rhs) 
: CFilter(rhs),
  m_isBuilt(rhs.m_isBuilt),
  m_ulm24(rhs.m_ulm24),
  m_phillips24(rhs.m_phillips24),
  m_register24(rhs.m_register24),
  m_sortedData(rhs.m_sortedData),
  m_found(rhs.m_found),
  m_eventCount(rhs.m_eventCount),
  m_prevtstampvmusb(rhs.m_prevtstampvmusb),
  m_prevtstampccusb(rhs.m_prevtstampccusb),
  m_prevevnum(rhs.m_prevevnum),
  m_eventCountLastError(rhs.m_eventCountLastError),
  m_BOR(rhs.m_BOR),
  m_EOR(rhs.m_EOR),
  m_error(rhs.m_error)
{}

CS800Filter& CS800Filter::operator=(const CS800Filter& rhs) 
{
  if (this != &rhs) {
    m_isBuilt = rhs.m_isBuilt;
    m_ulm24 = rhs.m_ulm24,
            m_phillips24 = rhs.m_phillips24,
            m_register24 = rhs.m_register24,
            m_sortedData = rhs.m_sortedData;
    m_found = rhs.m_found,
      m_eventCount = rhs.m_eventCount;
    m_prevtstampvmusb = rhs.m_prevtstampvmusb;
    m_prevtstampccusb = rhs.m_prevtstampccusb;
    m_prevevnum = rhs.m_prevevnum;
    m_eventCountLastError = rhs.m_eventCountLastError;
    m_BOR = rhs.m_BOR;
    m_EOR = rhs.m_EOR;
    m_error = rhs.m_error;
    CFilter::operator=(rhs);
  }
  return *this;
}





/* This is the function that receives the pointer pItem to a  Ring Item from EVB */
CRingItem* CS800Filter::handlePhysicsEventItem(CPhysicsEventItem* pItem)
{
  bool goodstatus = true; 
 

  int status = 0;

  int i,j;

  size_t maxBody = 8192;

  uint16_t control = 0;

  uint16_t* pBody = nullptr; // pBody will point to fragment body
  uint16_t* pTBody = nullptr; // pTBody will point to fragment body

//  EventType* event = nullptr; // event will point to EventType structure (defined in header)

  CRingItem* publish = nullptr; //publish will point to the final formatted data to be published


  ++m_eventCount; // update the event count

  uint32_t sid[MAXNF] = {0,0,0,0,0,0,0,0,0,0};

 
  try {

  FragmentIndex index(reinterpret_cast<uint16_t*>(pItem->getBodyPointer())); 
  size_t NF = index.getNumberFragments(); // Number of fragments in EVB ringitem



  //  event = new EventType(); //event will collect data from modules
  EventType event; //event will collect data from modules

  


  if (NF == 2) { // Number of fragments in Item MUST BE 2 (VM-USB and CC-USB)

    uint64_t earliest = std::numeric_limits<uint64_t>::max();
    i = 0;

    FragmentIndex::iterator it = index.begin();
    FragmentIndex::iterator itend = index.end();


	
    m_found[XLM_TIMESTAMP_TAG] = false;
    m_found[XLM_CRDC1_TAG] = false;
    m_found[XLM_CRDC2_TAG] = false;
    m_found[XLM_PPAC_TAG] = false;
    m_found[MTDC_TAG] = false;
    m_found[ULM_TAG] = false;
    m_found[FERA_TAG] = false;
    m_found[PHILLIPS_ADC_IONCHAMBER_TAG] = false;
    m_found[REGISTER_TAG] = false;
    m_found[PHILLIPS_ADC_HODOSCOPE1_TAG] = false;
    m_found[PHILLIPS_ADC_HODOSCOPE2_TAG] = false;
    m_found[PHILLIPS_ADC_S800_TAG] = false;
    m_found[PHILLIPS_TDC_S800_TAG] = false;
    //m_found[PHILLIPS_TDC_LABR_TAG] = false;
    

    while ( (it != itend) && (goodstatus) ) { // Read fragments within EVB ringitem //////////////////////

      CRingItem* item_h = CRingItemFactory::createRingItem(it->s_itemhdr); // Fragment header 

      if ( item_h->hasBodyHeader() ) { 

        uint32_t id = item_h->getSourceId(); // Fragment Source ID
        uint64_t tstamp = item_h->getEventTimestamp(); // Fragment timestamp
        size_t size = item_h->getStorageSize()/sizeof(uint16_t); // Fragment payload size (ring item header + body header + body)
        size_t bsize = item_h->getBodySize()/sizeof(uint16_t); // Fragment body size (just body)

	/* Check for repeated source IDs**************************************/ 
	j = i-1;
	while(j >= 0) {
	  if (id == sid[j]) {
	    //std::cerr << "*** ERROR: Different fragments have same source ID!!!!!!!" << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR:  Different fragments have same source ID!!!!!!!";
	    Actions::Error ( msg.str() ); 
	    m_error[3] += 1; // Repeated source ID
	    goodstatus = false;
	    break;
	  }
	  j--;
	}
	sid[i] = id;
	i++;

	/* Check timestamps *************************************************/ 
	if (tstamp == 0) {
	  
	  //std::cerr << "*** ERROR: Timestamp = 0 !!!!!!!" << std::endl;
	  std::stringstream msg;
	  msg << " *** ERROR:  Timestamp = 0 !!!!!!!";
	  Actions::Error ( msg.str() ); 
	  m_error[4] += 1; // Corrupted timestamp = 0 
	  goodstatus = false;
	  //break;
	  
	} else if (tstamp < earliest) {
	  
	  earliest = tstamp;
	} else if (tstamp > earliest + WINDOW) {
	  
	  //std::cerr << "*** ERROR: Uncorrelated fragments (different timestamps) !!!!!!!" << std::endl;
	  std::stringstream msg;
	  msg << " *** ERROR: Uncorrelated fragments (different timestamps) !!!!!!! ";
	  Actions::Error ( msg.str() ); 
	  m_error[5] += 1; // Uncorrelated fragments (different timestamps) 
	  goodstatus = false;
	  //break;
	}
	
	
	//std::cout << "\nINFO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
	//std::cout << "ID: " << std::dec << id << std::endl;
	//std::cout << "Size total: " << std::dec << size << ", body: " << bsize << std::endl;
	//std::cout << "Timestamp: " << std::dec << tstamp << std::endl;
	

	pBody = it->s_itembody; // Define pointer to data body 
	pTBody = it->s_itembody; // Define pointer to data body 
	status = parseData(id,bsize,tstamp,pBody,pTBody,&event);
	if (status != 0) goodstatus = false;

      } else {

        //std::cerr << "*** ERROR: Ew !!! A head-less ITEM !!!!!!!" << std::endl;

	std::stringstream msg;
	msg << " *** ERROR: Ew !!! A head-less ITEM !!!!!!! ";
	Actions::Error ( msg.str() ); 
        m_error[2] += 1; // Fragment has no header
        goodstatus = false;

      }

      //incrementCounter(item);
      delete item_h;
      ++it;



    }  ////////// End while-loop over fragments within EVB 


  } else { // If number of fragments is not 2

    //std::cerr << " *** ERROR: Number of fragments is NOT 2 !!!!!!!" << std::endl;
    std::stringstream msg;
    msg << " *** ERROR: Number of fragments is NOT 2 !!!!!!!";
    Actions::Error ( msg.str() ); 
    
    m_error[1] += 1; // Wrong number of fragments 
    goodstatus = false;

  }





  if (goodstatus) { // If event is good, check for missing packet tags
    int ntrue = countTrue(m_found);
    if (ntrue!=m_found.size()) {
      std::stringstream msg;
      msg << " *** ERROR: Incomplete RingItem !!!";
      Actions::Error ( msg.str() ); 
      m_error[10] += 1; // Incomplete RingItem
    }
  }




  publish = new CPhysicsEventItem(event.timestamp[0], 2, 0, maxBody); // Create new ring item 



  //if (goodstatus) { // If we had a good ringitem

    FormatData(status, &event); // Format and pack data
    PublishData(publish); // Publish new ring item

    //} else { //...otherwise

    //PublishCorruptedData(publish); // Publish new ring item with bad data

    //}





  //if (!goodstatus) { 
  // if ( (m_error[1] == 0) || (m_error[1] > 1) ) {

  //    std::stringstream msg;
  //	msg << "******* FATAL ERROR stops the run!!!!!: ";	
  //	Actions::Error(msg.str());
  //	Actions::EndRun(true);
  //}
  //}








  } catch (std::exception& exc) { 
        std::stringstream msg;
	msg << "Caught a std::exception in event handler: " << exc.what();	
        msg << " Shutting down!";
	Actions::Error(msg.str());
	Actions::EndRun();
  } catch (CException& exc) {
	std::stringstream msg;
	msg << "Caught a CException in event handler: " << exc.ReasonText() << " while doing: " << exc.WasDoing();
        msg << " Shutting down!";
	Actions::Error(msg.str());
	Actions::EndRun();
  } catch (std::string errmsg) {
	std::stringstream msg;
	msg << "Caught a string exception in event handler: " << errmsg;
        msg << " Shutting down!";
	Actions::Error(msg.str());
	Actions::EndRun();
  } catch (...) {
	Actions::Error("Caught an unknown exception! Shutting down!");
	Actions::EndRun();
  }


//  delete event;

  return publish;
}



int CS800Filter::parseData(uint32_t hid, size_t bsize, uint64_t htime, uint16_t* pRData, uint16_t* pData, EventType* pEvent) 
{ 
  int status = 0;
  int i;


  uint16_t nWords=0, ntWords=0, wCounter=0,readWords=0,tag;
  uint16_t subpacket=0,endsubpacket=0; // Read sub-packet tag

  bool display=false;

  uint16_t temp16;
  uint32_t temp32;
  uint64_t temp64;



  /* VMUSB "splits" very long events in chunks of data. Each chunk is preceeded by a header  */
  /* which includes the number of words in the chunck (bits 0 to 11); stack id (bit 13); and */
  /* a continuation bit (bit 12). Whenever the later is set to 1, additional chunck(s) of    */
  /* data follow the current one.                                                            */
  /* In order to process the whole event (including all chuncks of data), we create a buffer */
  /* data pointer (pData) which is re-filled with the content from pRData except the headers */
  /* preceeding the chunk of data. JP 9/27/2015                                              */
  int cbit = 0x1000;
  int stackid;
  while(cbit == 0x1000) {
    
    temp16 = *pRData++; // First header

    ntWords = temp16 & 0xfff; // Number of 16-bit words (self-exclusive) (bits 0 to 11)
    cbit = temp16 & 0x1000; // Continuation bit (bit 12) 
    stackid = temp16 & 0x2000; // Stack ID (bit 13)    

    nWords += ntWords; // Update the word counter with the value from the current chunk of data 
    
    for (i=0; i<ntWords; i++) { // Fill pData with the current chuck of data from pRData
      *pData = *pRData;
      pData++;
      pRData++;
    }
  }
  pData = pData - nWords; // Now move pData back to the beginning of the event buffer
  




  if (display) {
    std::cout << "Body Size: " << std::dec << bsize << std::endl;
    std::cout << "Words: " << std::dec << nWords << std::endl;
  }






  tag =  *pData++; // Controller tag
  wCounter++;

  if (display) std::cout << "Controller tag: "  << std::hex << tag  << std::endl;


  if (tag == VMUSB || tag == CCUSB ) { 


    /* READ EVENT NUMBER *****************************************************************/
    if (tag == VMUSB ) {

      /* Decode event number from VME crate - 2 x 32 bit native output */
      for (i=0; i < NEVNUMWORDS; i++) {
        pEvent->evnum_bit[0][i] = *pData++;  
      }
      pData++; // Ignore bits 49-64 - event number will roll over at 48 bits as CAMAC

    } else {

      /* Decode event number from CAMAC crate - 2 x 24 bit native output */
      pEvent->evnum_bit[1][0] = *pData++; 

      temp16 = *pData++;
      pEvent->evnum_bit[1][1] = temp16;
      temp16 = *pData++;
      pEvent->evnum_bit[1][1] |= (temp16 & 0xff) << 8;

      pEvent->evnum_bit[1][2] = (temp16 & 0xff00) >> 8;
      temp16 = *pData++;
      pEvent->evnum_bit[1][2] |= (temp16 & 0xff) << 8;        

    }

    wCounter+=4;





    /* Build the 48-bit event number from VM-USB (CC-USB event number includes scaler events) */
    if (tag == VMUSB ) {
      pEvent->evnum = pEvent->evnum_bit[0][0];
      temp64 = pEvent->evnum_bit[0][1];
      pEvent->evnum |= temp64 << 16;
      temp64 = pEvent->evnum_bit[0][2];
      pEvent->evnum |= temp64 << 32;
      if (display) std::cout << "Event number:  " << std::dec << pEvent->evnum  << std::endl;

      if ( (m_prevevnum != 0) && ((pEvent->evnum - 1) != m_prevevnum) ) {
	std::stringstream msg;
	msg << " *** ERROR: Non-contiguous event number in VMUSB!!!! Previous: " << m_prevevnum << " Current: " << pEvent->evnum;
	Actions::Error ( msg.str() ); 
	m_error[16] += 1;   
	status = 1;
      } 
      m_prevevnum = pEvent->evnum;
      if (status != 0) return status;




    } else {
      pEvent->evnum = 0;
    }


    //std::map<uint16_t,uint16_t> m_error;


    /* SEARCH FOR SUBPACKET TAGS *****************************************************************/

    while(wCounter < nWords) {

      subpacket = *pData++; // Read sub-packet tag
      wCounter++;

      if (display) std::cout << "subpacket tag: "  << std::hex << subpacket << std::endl;

      switch(subpacket) {


        case XLM_TIMESTAMP_TAG: // XLM TIMESTAMP ////////////////////////////////////////////////////////////////////


          if (!m_found[XLM_TIMESTAMP_TAG]) { 

            m_found[XLM_TIMESTAMP_TAG] = true;

            //std::cout << "Decoding XLM TIMESTAMP:"  << std::hex << subpacket << std::endl;

            int timeflag = 0; // Timestamp flag = XLM
            uint16_t* p_time1 = DecodeXLMTimestamp(pData, pEvent, timeflag, status);
	    if (status != 0) return status;

            readWords = p_time1 - pData;
            //std::cout << "Read XLM timestamp uint16_t words: :"  << std::dec << readWords << std::endl;

            pData += readWords;
            wCounter += readWords;

            if (pEvent->timestamp[timeflag] == 0 || (pEvent->timestamp[timeflag] != htime) ) {
              //std::cerr << "*** ERROR: Wrong timestamp!!!!!!!"  << std::hex << pEvent->timestamp[timeflag] << std::endl;
	      std::stringstream msg;
	      msg << " *** ERROR: Wrong timestamp!!!!!!!";
	      Actions::Error ( msg.str() ); 
 	      m_error[9] += 1;
              status = 1;
            }

	    if ( (m_prevtstampvmusb != 0) && (pEvent->timestamp[timeflag] <= m_prevtstampvmusb) ) {
	      std::stringstream msg;
	      msg << " *** ERROR: Non-increasing XLM timestamp!!!!!!!";
	      Actions::Error ( msg.str() ); 
	      m_error[17] += 1;   
	      status = 1;
	    } 
	    m_prevtstampvmusb = pEvent->timestamp[timeflag];
	    if (status != 0) return status;

          } else {
            //std::cerr << "*** ERROR: duplicated subpacket tag!!!!!!!"  << std::hex << subpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: duplicated subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 

            m_error[8] += 1;
            status = 1;
            return status;
          }

          endsubpacket = *pData++; 
          wCounter++;

          if (display) std::cout << "End-of-subpacket tag: "  << std::hex << endsubpacket << ". Counted words: " << std::dec << wCounter << std::endl;

          if (endsubpacket != XLM_TIMESTAMP_ETAG) { // Check that we are at the end of the subpacket
            //std::cerr << "*** ERROR: Missing end-of-subpacket tag!!!!!!!"  << std::hex << endsubpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: Missing end-of-subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[7] += 1;
            status = 1;
            return status;
          }


          break;



        case XLM_CRDC1_TAG:   // XLM CRDC1 ////////////////////////////////////////////////////////////////////

          if (!m_found[XLM_CRDC1_TAG]) { 

            m_found[XLM_CRDC1_TAG] = true;

            //std::cout << "Decoding XLM CRDC1:"  << std::hex << subpacket << std::endl;

            int det = 0; // PAD detector flag = CRDC1
            uint16_t* p_crdc1 = DecodeXLMpads(pData, pEvent, det, status);
	    if (status != 0) return status;

            readWords = p_crdc1 - pData;
            //std::cout << "Read CRDC1 uint16_t words: :"  << std::dec << readWords << std::endl;

            pData += readWords;
            wCounter += readWords;

          } else {
            //std::cerr << "*** ERROR: duplicated subpacket tag!!!!!!!"  << std::hex << subpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: duplicated subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[8] += 1;
            status = 1;
            return status;
          }

          endsubpacket = *pData++; 
          wCounter++;

          if (display) std::cout << "End-of-subpacket tag: "  << std::hex << endsubpacket << ". Counted words: " << std::dec << wCounter << std::endl;

          if (endsubpacket != XLM_CRDC1_ETAG) { // Check that we are at the end of the subpacket
            //std::cerr << "*** ERROR: Missing end-of-subpacket tag!!!!!!!"  << std::hex << endsubpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: Missing end-of-subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[7] += 1;
            status = 1;
            return status;
          }

          break;

        case XLM_CRDC2_TAG:   // XLM CRDC2 ////////////////////////////////////////////////////////////////////

          if (!m_found[XLM_CRDC2_TAG]) { 

            m_found[XLM_CRDC2_TAG] = true;

            //std::cout << "Decoding XLM CRDC2:"  << std::hex << subpacket << std::endl;

            int det = 1; // PAD detector flag = CRDC2
            uint16_t* p_crdc2 = DecodeXLMpads(pData, pEvent, det, status);
	    if (status != 0) return status;

            readWords = p_crdc2 - pData;
            //std::cout << "Read CRDC2 uint16_t words: :"  << std::dec << readWords << std::endl;

            pData += readWords;
            wCounter += readWords;

          } else {
            //std::cerr << "*** ERROR: duplicated subpacket tag!!!!!!!"  << std::hex << subpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: duplicated subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[8] += 1;
            status = 1;
            return status;
          }


          endsubpacket = *pData++; 
          wCounter++;

          if (display) std::cout << "End-of-subpacket tag: "  << std::hex << endsubpacket << ". Counted words: " << std::dec << wCounter << std::endl;

          if (endsubpacket != XLM_CRDC2_ETAG) { // Check that we are at the end of the subpacket
            //std::cerr << "*** ERROR: Missing end-of-subpacket tag!!!!!!!"  << std::hex << endsubpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: Missing end-of-subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[7] += 1;
            status = 1;
            return status;
          }

          break;


        case XLM_PPAC_TAG:   // XLM PPAC ////////////////////////////////////////////////////////////////////

          if (!m_found[XLM_PPAC_TAG]) { 

            m_found[XLM_PPAC_TAG] = true;

            //std::cout << "Decoding XLM PPAC:"  << std::hex << subpacket << std::endl;

            int det = 2; // PAD detector flag = PPAC
            uint16_t* p_ppac = DecodeXLMpads(pData, pEvent, det, status);
	    if (status != 0) return status;

            readWords = p_ppac - pData;
            //std::cout << "Read PPAC uint16_t words: :"  << std::dec << readWords << std::endl;

            pData += readWords;
            wCounter += readWords;

          } else {
            //std::cerr << "*** ERROR: duplicated subpacket tag!!!!!!!"  << std::hex << subpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: duplicated subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[8] += 1;
            status = 1;
            return status;
          }


          endsubpacket = *pData++; 
          wCounter++;

	  if (display) std::cout << "End-of-subpacket tag: "  << std::hex << endsubpacket << ". Counted words: " << std::dec << wCounter << std::endl;

          if (endsubpacket != XLM_PPAC_ETAG) { // Check that we are at the end of the subpacket
            //std::cerr << "*** ERROR: Missing end-of-subpacket tag!!!!!!!"  << std::hex << endsubpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: Missing end-of-subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[7] += 1;
            status = 1;
            return status;
          }

          break;

        case MTDC_TAG:   // Mesytec TDC ////////////////////////////////////////////////////////////////////

          if (!m_found[MTDC_TAG]) { 

            m_found[MTDC_TAG] = true;

            //std::cout << "Decoding Mesytec TDC:"  << std::hex << subpacket << std::endl;

            uint16_t* p_mtdc = DecodeMTDC(pData, pEvent, status);
	    if (status != 0) return status;

            readWords = p_mtdc - pData;
            //std::cout << "Read Mesytec TDC uint16_t words: :"  << std::dec << readWords << std::endl;

            pData += readWords;
            wCounter += readWords;

          } else {
            //std::cerr << "*** ERROR: duplicated subpacket tag!!!!!!!"  << std::hex << subpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: duplicated subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[8] += 1;
            status = 1;
            return status;
          }


          endsubpacket = *pData++; 
          wCounter++;

          if (display) std::cout << "End-of-subpacket tag: "  << std::hex << endsubpacket << ". Counted words: " << std::dec << wCounter << std::endl;

          if (endsubpacket != MTDC_ETAG) { // Check that we are at the end of the subpacket
            //std::cerr << "*** ERROR: Missing end-of-subpacket tag!!!!!!!"  << std::hex << endsubpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: Missing end-of-subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[7] += 1;
            status = 1;
            return status;
          }

          break;


        case ULM_TAG: // ULM TRIGGER AND TIMESTAMP ////////////////////////////////////////////////////////////////////

          if (!m_found[ULM_TAG]) { // ULM sends 24-bit words

            m_found[ULM_TAG] = true;

            //std::cout << "Decoding ULM TRIGGER AND TIMESTAMP:"  << std::hex << subpacket << std::endl;

            pEvent->trigger_pattern = *pData++; // Read trigger pattern (bits 0 to 15 from 24-bit word)
            if (m_ulm24) {
              pData++; // Skip bits 16 to 23 for now
            }

            wCounter+=2; 


            int timeflag = 1; // Timestamp flag = ULM
            uint16_t* p_time2 = DecodeULMTimestamp(pData, pEvent, timeflag, status);
	    if (status != 0) return status;

            readWords = p_time2 - pData;
            //std::cout << "Read ULM timestamp uint16_t words: :"  << std::dec << readWords << std::endl;

            pData += readWords;
            wCounter += readWords;


            if (pEvent->timestamp[timeflag] == 0 || (pEvent->timestamp[timeflag] != htime) ) {
              //std::cerr << "*** ERROR: Wrong timestamp!!!!!!!"  << std::hex << pEvent->timestamp[timeflag] << std::endl;
	      std::stringstream msg;
	      msg << " *** ERROR: Wrong timestamp!!!!!!!";
	      Actions::Error ( msg.str() ); 

	      m_error[9] += 1;
              status = 1;
            }

	    if ( (m_prevtstampccusb != 0) && (pEvent->timestamp[timeflag] <= m_prevtstampccusb) ) {
	      std::stringstream msg;
	      msg << " *** ERROR: Non-increasing ULM timestamp!!!!!!!";
	      Actions::Error ( msg.str() ); 
	      m_error[18] += 1;   
	      status = 1;
	    } 
	    m_prevtstampccusb = pEvent->timestamp[timeflag];
	    if (status != 0) return status;

          } else {
            //std::cerr << "*** ERROR: duplicated subpacket tag!!!!!!!"  << std::hex << subpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: duplicated subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[8] += 1;
            status = 1;
            return status;
          }



          endsubpacket = *pData++; 
          wCounter++;

          if (display) std::cout << "End-of-subpacket tag: "  << std::hex << endsubpacket << ". Counted words: " << std::dec << wCounter << std::endl;

          if (endsubpacket != ULM_ETAG) { // Check that we are at the end of the subpacket
            //std::cerr << "*** ERROR: Missing end-of-subpacket tag!!!!!!!"  << std::hex << endsubpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: Missing end-of-subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[7] += 1;
            status = 1;
            return status;
          }

          break;

        case FERA_TAG: // FERA ADC ////////////////////////////////////////////////////////////////////

          if (!m_found[FERA_TAG]) { 

            m_found[FERA_TAG] = true;

            //std::cout << "Decoding FERA ADC:"  << std::hex << subpacket << std::endl;

            uint16_t* p_fera = DecodeFERAADC(pData, pEvent, status);
	    if (status != 0) return status;

            readWords = p_fera - pData;
            //std::cout << "Read FERA uint16_t words: :"  << std::dec << readWords << std::endl;

            pData += readWords;
            wCounter += readWords;

          } else {
            //std::cerr << "*** ERROR: duplicated subpacket tag!!!!!!!"  << std::hex << subpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: duplicated subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[8] += 1;
            status = 1;
            return status;
          }


          endsubpacket = *pData++; 
          wCounter++;

          if (display) std::cout << "End-of-subpacket tag: "  << std::hex << endsubpacket << ". Counted words: " << std::dec << wCounter << std::endl;

          if (endsubpacket != FERA_ETAG) { // Check that we are at the end of the subpacket
            //std::cerr << "*** ERROR: Missing end-of-subpacket tag!!!!!!!"  << std::hex << endsubpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: Missing end-of-subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[7] += 1;
            status = 1;
            return status;
          }


          break;

        case PHILLIPS_ADC_IONCHAMBER_TAG: // PHILLIPS_ADC_IONCHAMBER ADC ////////////////////////////////////////////////////////////////////

          if (!m_found[PHILLIPS_ADC_IONCHAMBER_TAG]) { 

            m_found[PHILLIPS_ADC_IONCHAMBER_TAG] = true;

            //std::cout << "Decoding Phillips ADC Ion Chamber:"  << std::hex << subpacket << std::endl;

            int phillipsflag = 0; // phillipsflag = ION CHAMBER

            uint16_t* p_PADC1 = DecodePhillips(pData, pEvent, phillipsflag, status);
	    if (status != 0) return status;

            readWords = p_PADC1 - pData;
            //std::cout << "Read Phillips ADC Ion Chamber uint16_t words: :"  << std::dec << readWords << std::endl;

            pData += readWords;
            wCounter += readWords;

          } else {
            //std::cerr << "*** ERROR: duplicated subpacket tag!!!!!!!"  << std::hex << subpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: duplicated subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[8] += 1;
            status = 1;
            return status;
          }


          endsubpacket = *pData++; 
          wCounter++;

          if (display) std::cout << "End-of-subpacket tag: "  << std::hex << endsubpacket << ". Counted words: " << std::dec << wCounter << std::endl;

          if (endsubpacket != PHILLIPS_ADC_IONCHAMBER_ETAG) { // Check that we are at the end of the subpacket
            //std::cerr << "*** ERROR: Missing end-of-subpacket tag!!!!!!!"  << std::hex << endsubpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: Missing end-of-subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[7] += 1;
            status = 1;
            return status;
          }

          break;



        case REGISTER_TAG: // REGISTER ADC ////////////////////////////////////////////////////////////////////

          if (!m_found[REGISTER_TAG]) { 

            m_found[REGISTER_TAG] = true;

            //	  std::cout << "Decoding LeCroy coincidence register:"  << std::hex << subpacket << std::endl;

            pEvent->coinc[0] = *pData++; //Coincidence register for hodoscope 1-16
            if (m_register24) {
              pData++; //skip
            }
            //	  std::cout << "Coincidence register 1-16:"  << std::hex << pEvent->coinc[0] << std::endl;

            pEvent->coinc[1] = *pData++; //Coincidence register for hodoscope 17-31
            if (m_register24) {
              pData++; //skip
            }
            //	  std::cout << "Coincidence register 17-31:"  << std::hex << pEvent->coinc[1] << std::endl;

            wCounter+=4;

          } else {
            //std::cerr << "*** ERROR: duplicated subpacket tag!!!!!!!"  << std::hex << subpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: duplicated subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[8] += 1;
            status = 1;
            return status;
          }


          endsubpacket = *pData++; 
          wCounter++;

          if (display) std::cout << "End-of-subpacket tag: "  << std::hex << endsubpacket << ". Counted words: " << std::dec << wCounter << std::endl;

          if (endsubpacket != REGISTER_ETAG) { // Check that we are at the end of the subpacket
            //std::cerr << "*** ERROR: Missing end-of-subpacket tag!!!!!!!"  << std::hex << endsubpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: Missing end-of-subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[7] += 1;
            status = 1;
            return status;
          }

          break;


        case PHILLIPS_ADC_HODOSCOPE1_TAG: // PHILLIPS_ADC_HODOSCOPE1 ADC ////////////////////////////////////////////////////////////////////

          if (!m_found[PHILLIPS_ADC_HODOSCOPE1_TAG]) { 

            m_found[PHILLIPS_ADC_HODOSCOPE1_TAG] = true;

            //std::cout << "Decoding Phillips ADC Hodoscope 1:"  << std::hex << subpacket << std::endl;

            int phillipsflag = 1; // phillipsflag = Hodoscope 1

            uint16_t* p_PADC2 = DecodePhillips(pData, pEvent, phillipsflag, status);
	    if (status != 0) return status;

            readWords = p_PADC2 - pData;
            //std::cout << "Read Phillips ADC Hodoscope 1 uint16_t words: :"  << std::dec << readWords << std::endl;

            pData += readWords;
            wCounter += readWords;

          } else {
            //std::cerr << "*** ERROR: duplicated subpacket tag!!!!!!!"  << std::hex << subpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: duplicated subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[8] += 1;
            status = 1;
            return status;
          }


          endsubpacket = *pData++; 
          wCounter++;

           if (display) std::cout << "End-of-subpacket tag: "  << std::hex << endsubpacket << ". Counted words: " << std::dec << wCounter << std::endl;

          if (endsubpacket != PHILLIPS_ADC_HODOSCOPE1_ETAG) { // Check that we are at the end of the subpacket
            //std::cerr << "*** ERROR: Missing end-of-subpacket tag!!!!!!!"  << std::hex << endsubpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: Missing end-of-subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[7] += 1;
            status = 1;
            return status;
          }


          break;


        case PHILLIPS_ADC_HODOSCOPE2_TAG: // PHILLIPS_ADC_HODOSCOPE2 ADC ////////////////////////////////////////////////////////////////////

          if (!m_found[PHILLIPS_ADC_HODOSCOPE2_TAG]) { 

            m_found[PHILLIPS_ADC_HODOSCOPE2_TAG] = true;

            //std::cout << "Decoding Phillips ADC Hodoscope 2:"  << std::hex << subpacket << std::endl;

            int phillipsflag = 2; // phillipsflag = Hodoscope 2

            uint16_t* p_PADC3 = DecodePhillips(pData, pEvent, phillipsflag, status);
	    if (status != 0) return status;

            readWords = p_PADC3 - pData;
            //std::cout << "Read Phillips ADC Hodoscope 2 uint16_t words: :"  << std::dec << readWords << std::endl;

            pData += readWords;
            wCounter += readWords;

          } else {
            //std::cerr << "*** ERROR: duplicated subpacket tag!!!!!!!"  << std::hex << subpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: duplicated subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[8] += 1;
            status = 1;
            return status;
          }


          endsubpacket = *pData++; 
          wCounter++;

          if (display) std::cout << "End-of-subpacket tag: "  << std::hex << endsubpacket << ". Counted words: " << std::dec << wCounter << std::endl;

          if (endsubpacket != PHILLIPS_ADC_HODOSCOPE2_ETAG) { // Check that we are at the end of the subpacket
            //std::cerr << "*** ERROR: Missing end-of-subpacket tag!!!!!!!"  << std::hex << endsubpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: Missing end-of-subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[7] += 1;
            status = 1;
            return status;
          }


          break;




        case PHILLIPS_ADC_S800_TAG: // PHILLIPS_ADC_S800 ADC ////////////////////////////////////////////////////////////////////

          if (!m_found[PHILLIPS_ADC_S800_TAG]) { 

            m_found[PHILLIPS_ADC_S800_TAG] = true;

            //std::cout << "Decoding Phillips S800 ADC:"  << std::hex << subpacket << std::endl;

            int phillipsflag = 3; // phillipsflag = S800 ADC

            uint16_t* p_PADC4 = DecodePhillips(pData, pEvent, phillipsflag, status);
	    if (status != 0) return status;

            readWords = p_PADC4- pData;
            //std::cout << "Read Phillips S800 ADC uint16_t words: :"  << std::dec << readWords << std::endl;

            pData += readWords;
            wCounter += readWords;

          } else {
            //std::cerr << "*** ERROR: duplicated subpacket tag!!!!!!!"  << std::hex << subpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: duplicated subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[8] += 1;
            status = 1;
            return status;
          }


          endsubpacket = *pData++; 
          wCounter++;

          if (display) std::cout << "End-of-subpacket tag: "  << std::hex << endsubpacket << ". Counted words: " << std::dec << wCounter << std::endl;

          if (endsubpacket != PHILLIPS_ADC_S800_ETAG) { // Check that we are at the end of the subpacket
            //std::cerr << "*** ERROR: Missing end-of-subpacket tag!!!!!!!"  << std::hex << endsubpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: Missing end-of-subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[7] += 1;
            status = 1;
            return status;
          }


          break;

        case PHILLIPS_TDC_S800_TAG: // PHILLIPS_TDC_S800 TDC ////////////////////////////////////////////////////////////////////

          if (!m_found[PHILLIPS_TDC_S800_TAG]) { 

            m_found[PHILLIPS_TDC_S800_TAG] = true;

            //std::cout << "Decoding Phillips S800 TDC:"  << std::hex << subpacket << std::endl;

            int phillipsflag = 4; // phillipsflag = S800 TDC

            uint16_t* p_PTDC = DecodePhillips(pData, pEvent, phillipsflag, status);
	    if (status != 0) return status;

            readWords = p_PTDC- pData;
            //std::cout << "Read Phillips S800 TDC uint16_t words: :"  << std::dec << readWords << std::endl;

            pData += readWords;
            wCounter += readWords;

          } else {
            //std::cerr << "*** ERROR: duplicated subpacket tag!!!!!!!"  << std::hex << subpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: duplicated subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[8] += 1;
            status = 1;
            return status;
          }


          endsubpacket = *pData++; 
          wCounter++;

          if (display) std::cout << "End-of-subpacket tag: "  << std::hex << endsubpacket << ". Counted words: " << std::dec << wCounter << std::endl;

          if (endsubpacket != PHILLIPS_TDC_S800_ETAG) { // Check that we are at the end of the subpacket
            //std::cerr << "*** ERROR: Missing end-of-subpacket tag!!!!!!!"  << std::hex << endsubpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: Missing end-of-subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[7] += 1;
            status = 1;
            return status;
          }


          break;







        case PHILLIPS_TDC_LABR_TAG: // PHILLIPS_TDC_LABR TDC ////////////////////////////////////////////////////////////////////

          if (!m_found[PHILLIPS_TDC_LABR_TAG]) { 

            m_found[PHILLIPS_TDC_LABR_TAG] = true;

            //std::cout << "Decoding Phillips LaBr TDC:"  << std::hex << subpacket << std::endl;

            int phillipsflag = 5; // phillipsflag = LaBr TDC

            uint16_t* p_PLaBr = DecodePhillips(pData, pEvent, phillipsflag, status);
	    if (status != 0) return status;

            readWords = p_PLaBr- pData;
            //std::cout << "Read Phillips LaBr TDC uint16_t words: :"  << std::dec << readWords << std::endl;

            pData += readWords;
            wCounter += readWords;

          } else {
            //std::cerr << "*** ERROR: duplicated subpacket tag!!!!!!!"  << std::hex << subpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: duplicated subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[8] += 1;
            status = 1;
            return status;
          }


          endsubpacket = *pData++; 
          wCounter++;

          if (display) std::cout << "End-of-subpacket tag: "  << std::hex << endsubpacket << ". Counted words: " << std::dec << wCounter << std::endl;

          if (endsubpacket != PHILLIPS_TDC_LABR_ETAG) { // Check that we are at the end of the subpacket
            //std::cerr << "*** ERROR: Missing end-of-subpacket tag!!!!!!!"  << std::hex << endsubpacket << std::endl;
	    std::stringstream msg;
	    msg << " *** ERROR: Missing end-of-subpacket tag!!!!!!!";
	    Actions::Error ( msg.str() ); 
            m_error[7] += 1;
            status = 1;
            return status;
          }


          break;



        default:

          //std::cerr << "*** ERROR: Unknown word in fragment body!!!!!!!! "  << std::hex << subpacket << std::endl;
	  std::stringstream msg;
	  msg << " *** ERROR: Unknown word in fragment body!!!!!!!! ";
	  Actions::Error ( msg.str() ); 
          m_error[11] += 1;
          status = 1;
          std::cout << "Word count = " << std::dec << wCounter << " nWords=" << nWords << std::endl;

          return status;
          //break;


      }

    }

    //int ntrue = countTrue(found);
    //if (ntrue!=found.size()) {
    //  status = 256; // Not all subpacket tags were found
    //}



  } else {
    //std::cerr << "*** ERROR: Unknown controller tag!!!!!!!" << std::endl;
    std::stringstream msg;
    msg << " *** ERROR: Unknown controller tag!!!!!!!";
    Actions::Error ( msg.str() ); 
    m_error[12] += 1; // Unknown controller
    status = 1;
    return status;    
  }


  return status;
} 



int CS800Filter::countTrue(std::map<uint16_t,bool> mmap)
{
  std::map<uint16_t,bool>::const_iterator it = mmap.begin();
  std::map<uint16_t,bool>::const_iterator itend = mmap.end();
  int count=0;
  while (it!=itend) {
    if (it->second) ++count;
    ++it;
  }

  return count;
}



uint16_t*  CS800Filter::DecodeXLMTimestamp(uint16_t* pXLMTimedata, EventType* pXLMTime, int flag, int status) 
{

  int i;
  uint16_t temp16;

  uint64_t temp64;

  pXLMTime->timestamp[flag] = 0;
  for (i=0; i < NTSWORDS; i++) {   

    pXLMTime->timestamp_bit[flag][i] = *pXLMTimedata++;
    temp64 = pXLMTime->timestamp_bit[flag][i];
    pXLMTime->timestamp[flag] |= temp64 << (i*16); 

  }

  pXLMTime = NULL;

  return pXLMTimedata;

}


uint16_t*  CS800Filter::DecodeULMTimestamp(uint16_t* pULMTimedata, EventType* pULMTime, int flag, int status) 
{

  int i;
  uint16_t temp16;

  uint64_t temp64;

  pULMTime->timestamp[flag] = 0;
  for (i=0; i < NTSWORDS; i++) {   

    //std::cout << "Timestamp!!!!!!!"  << std::hex << *pULMTimedata++ << std::endl; 

    pULMTime->timestamp_bit[flag][i] = *pULMTimedata++;
    if (m_ulm24) {
      pULMTimedata++; //skip bits 17 to 24
    }

    temp64 = pULMTime->timestamp_bit[flag][i];
    pULMTime->timestamp[flag] |= temp64 << (i*16); 

  }

  //std::cout << "Timestamp b0:  " <<  std::hex << pULMTime->timestamp_bit[0]  << " b1: "  << pULMTime->timestamp_bit[1]  << " b2: " << pULMTime->timestamp_bit[2] << " b3: " << pULMTime->timestamp_bit[3]  << std::endl; 
  //std::cout << "Timestamp!!!!!!!"  << std::dec << pULMTime->timestamp[flag] << std::endl;

  pULMTime = NULL;

  return pULMTimedata;

}



uint16_t*  CS800Filter::DecodeXLMpads(uint16_t* pPaddata, EventType* pPad, int det, int status) 
{

  int i;

  size_t nbytes;

  uint16_t temp16;
  uint32_t temp32,npads,maxpads;

  bool display=false;


  if (det == 0 || det == 1) {

    maxpads = NCRDCPADS*NCRDCSAMPLES;

  } else if (det == 2) {

    maxpads = NPPACSTRIPS*NPPACPLANES*NPPACS*NPPACSAMPLES;

  } else {
    //std::cerr << "*** ERROR: Unknown pad detector!!!!!!!"  <<  std::endl;
    std::stringstream msg;
    msg << " *** ERROR: Unknown pad detector!!!!!!!";
    Actions::Error ( msg.str() ); 
    m_error[13] += 1;
    status = 1;
    return pPaddata;
  }

  nbytes = *pPaddata++;
  temp32 = *pPaddata++;
  nbytes |= temp32 << 16;
  npads = nbytes/sizeof(uint64_t);


  if (display) std::cout << "Number of bytes: " << std::dec << nbytes << "  and pads: " << npads << std::endl;

  if (npads > maxpads) {
    /* attempt to skip over */
    //nCorrPads++;
    pPaddata += 2046;
    //std::cerr << "*** ERROR: too many pads!!!!!!!"  << std::dec << npads << std::endl;
    std::stringstream msg;
    msg << " *** ERROR: too many pads!!!!!!!";
    Actions::Error ( msg.str() ); 
    m_error[14] += 1;
    status = 1;
    return pPaddata;

  }

  pPad->npads[det] = npads;
  if (npads != 0) {

    for (i=0; i<npads; i++) {

      pPad->det_pads[det][i].d[0] = 0;
      pPad->det_pads[det][i].d[1] = 0;
      pPad->det_pads[det][i].d[2] = 0;
      pPad->det_pads[det][i].d[3] = 0;

      temp16 = *pPaddata++;
      if (display) std::cout << std::hex << temp16 << std::endl;
      pPad->det_pads[det][i].d[0] = temp16 & 0x3FF;

      pPad->det_pads[det][i].d[1] = (temp16 & 0xFC00) >> 10;
      temp16 = *pPaddata++;
      if (display) std::cout << std::hex << temp16 << std::endl;
      pPad->det_pads[det][i].d[1] |= (temp16 & 0xF) << 6;

      pPad->det_pads[det][i].d[2] = (temp16 & 0x3FF0) >> 4;

      temp16 = *pPaddata++;
      if (display) std::cout << std::hex << temp16 << std::endl;
      pPad->det_pads[det][i].d[3] = temp16 & 0x3FF;

      temp16 = *pPaddata++;
      if (display) std::cout << std::hex << temp16 << std::endl;
      pPad->det_pads[det][i].sch = temp16 & 0x7FF;   

    }
  }

  pPad = NULL;

  return pPaddata;

}


uint16_t*  CS800Filter::DecodeMTDC(uint16_t* pMTDCdata, EventType* pMTDC, int status) 
{

  int i,channel,crap=-1;

  uint16_t temp16;


  /* Two different anomalous behaviors were seen sometimes when rates are very high:  */
  /* 1. MTDC sends only a bunch of ffff words                                         */
  /* 2. MTDC "sends" few "unknown" words                                              */
  /* At the present time, the reason for these behaviors is not yet understood (no    */
  /* documentation found in manual). In order to avoid this "bug"                     */
  /* we configured the MTDC to have module id=0 and subheader=0 so that the second    */
  /* 16-bit word of the header (bits 16-31) has always the value 4000 (bit 31=0 and   */
  /* bit 30 = 0 (hsig), bits 16-29 = 0 (module id =0, subheader = 0)                  */
  /* It is very unlikely that any of those "unknown" words match this value           */


  while (*pMTDCdata == 0xFFFF) { // skip all ffff words seen in the beginning (if any)

    //std::cout << std::hex << *pMTDCdata << std::endl;

    pMTDCdata++; 

    if (*pMTDCdata == MTDC_ETAG) { //If there was nothing useful, return
      pMTDC = NULL;
      return pMTDCdata;
    }
  }


  while (*pMTDCdata != 0x4000) {

    pMTDCdata++;
    crap += 1; // Count crap words (we started at -1 because 4000 is in the second word of a good header)

    if (*pMTDCdata == MTDC_ETAG) { //If there was nothing useful, return....and scream

      std::stringstream msg;
      msg << " *** Warning: anomalous event seen in mesytec TDC !!!! ";
      Actions::Warning ( msg.str() ); 

      pMTDC = NULL;
      return pMTDCdata;
    }
  }
  pMTDCdata--; // 4000 was found, so move one word back to read the full header


  if (crap > 0) {
    std::stringstream msg;
    msg << " *** Warning: anomalous event seen in mesytec TDC !!!! ";
    Actions::Warning ( msg.str() ); 
  }



  /* Read header stuff */ 
  temp16 = *pMTDCdata++;

  /* Commented next two lines because it only applies to MADC, not MTDC        */
  /* MTDC header doesn't include any "format"                                  */
  /* MTDC resolution is coded in bits 12-15 of header, not 12-14 like the MADC */
  /* JP 9/27/2017                                                              */
  //pMTDC->mtdc.header.format = (temp16 & 0x8000)>>15; 
  //pMTDC->mtdc.header.adc_res = (temp16 & 7000)>>12;
  pMTDC->mtdc.header.format = 0; 
  pMTDC->mtdc.header.adc_res = (temp16 & 0xF000)>>12; // Resolution masked in bits 12-15
  pMTDC->mtdc.header.nwords = temp16 & 0xFFF;
  
  temp16 = *pMTDCdata++;
  pMTDC->mtdc.header.hsig = (temp16 & 0xC000)>>14;
  pMTDC->mtdc.header.subheader = (temp16 & 0x3F00)>>8; 
  pMTDC->mtdc.header.module_id = temp16 & 0xFF; 


  //if (pMTDC->mtdc.header.nwords-1 > 32) {
  //  //std::cerr << "*** ERROR: too many channels seen for mesytec TDC !!!! "  << std::dec << pMTDC->mtdc.header.nwords << std::endl;
  //  std::stringstream msg;
  //  msg << " *** ERROR: too many channels seen for mesytec TDC !!!! " << pMTDC->mtdc.header.nwords-1;
  //  Actions::Error ( msg.str() ); 
  //  m_error[15] += 1;
  //  status = 1;
  //  return pMTDCdata;
  // }

  for (i=0; i < 32; i++) pMTDC->mtdc.hits[i] = 0; // Init hit counts to zero

  for (i=0; i < pMTDC->mtdc.header.nwords-1; i++) {
    temp16 = *pMTDCdata++;
    channel = *pMTDCdata & 0x1F;
    if (pMTDC->mtdc.hits[channel] < 32) { // We currently accept a maximum of 32 multi-hit events per channel
                                          
      if ((*pMTDCdata++ & 0x0400) == 0x0400) {
	pMTDC->mtdc.data[channel][pMTDC->mtdc.hits[channel]] = temp16 & 0xFFFF;
	pMTDC->mtdc.hits[channel]++;
      } 

    } else { // Ignore larger multihit numbers...but send a warning
      std::stringstream msg;
      msg << " *** Warning: too many multihits per channel seen in mesytec TDC !!!! ";
      Actions::Warning ( msg.str() ); 
    }

  }

  pMTDC->mtdc.ender.tcts = *pMTDCdata++;
  temp16 = *pMTDCdata++;
  pMTDC->mtdc.ender.esig = (temp16&0xC000)>>14;
  pMTDC->mtdc.ender.tcts |= (temp16&0x3FFF)<<16;

  /* mesytec puts some extra ffff ffff in the data stream */
  pMTDCdata++;
  pMTDCdata++;
  while (*pMTDCdata == 0xFFFF) { // sometimes much more!
    pMTDCdata++;
  }

  pMTDC = NULL;

  return pMTDCdata;

}

uint16_t*  CS800Filter::DecodeFERAADC(uint16_t* pFERAdata, EventType* pFERA, int status) 
{

  int i;

  uint16_t wc,chan;

  wc = (*pFERAdata++ & 0x7800) >> 11; // Number of words: bits 11 to 14 (from bit 0)
  if (wc > 0) {
    for (i=0; i < wc; i++) {
      chan = (*pFERAdata & 0x7800) >> 11; // Channel number
      pFERA->fera[chan] = *pFERAdata++ & 0x7ff; // ADC value: bits 0 to 10
    }
  }

  pFERA = NULL;

  return pFERAdata;

}


uint16_t*  CS800Filter::DecodePhillips(uint16_t* pPhillipsdata, EventType* pPhillips, int flag, int status) 
{

  int i;

  pPhillips->phillips[flag][0] = *pPhillipsdata++; // Number of channels

  if (pPhillips->phillips[flag][0] > 0) {

    //std::cout << "Phillips ADC ion-chamber words: :"  << std::hex << pPhillipsADC->phillips_adc[flag][0] << std::endl;

    for (i=0; i<16; i++) {

      if ( (pPhillips->phillips[flag][0] >> i) & 0x1 ) {
        uint16_t value = *pPhillipsdata++; 
        uint16_t channel = (value >> 12) & 0xf;
        pPhillips->phillips[flag][channel+1] = value & 0xfff; // Value: bits 0 to 11

        if (m_phillips24) { // If using Phillips in 24-bit format
          pPhillipsdata++; // skip
        }

      }

    }

  }

  pPhillips = NULL;

  return pPhillipsdata;

}


/********************************************************************************************************/
/* Function FormatData takes event-structure pointer pEvent and uses it to create a map (m_sortedData)  */
/* with the re-formatted data to be published                                                           */
/********************************************************************************************************/
void CS800Filter::FormatData(int status, EventType* pEvent) 
{

  int i,j,id;

  uint16_t time=0, energy=0, TEMP_PACKET_CRDC;
  uint16_t size_subpacket_ic_energy=0, size_subpacket_crdc_raw=0, size_subpacket_crdc_anode=0, size_subpacket_ppac_raw=0;
  uint16_t label;


  std::vector<uint16_t> ic_energy, crdc_raw, crdc_anode, ppac_raw;

  std::vector<uint16_t>::const_iterator it, itend;


  m_sortedData.clear();



  /****S800_TIMESTAMP_PACKET****/
  for (i=0; i< NTSWORDS; i++) {
    m_sortedData[PACKET_TIMESTAMP].push_back(pEvent->timestamp_bit[0][i]); // Timestamp from XLM 
  }


  /****S800_EVENT_NUMBER_PACKET****/
  for (i=0; i< NEVNUMWORDS; i++) {
    m_sortedData[PACKET_EVENT_NUMBER].push_back(pEvent->evnum_bit[0][i]); // Event number from VM-USB Controller
  }

  /****S800_TRIGGER_PACKET****/
  m_sortedData[PACKET_TRIGGER].push_back(pEvent->trigger_pattern); // Trigger pattern

  for (i=S800_TRIGGER_FIRST; i<S800_TRIGGER_FIRST+S800_TRIGGER_CHANNELS; i++) { // Loop over TDC trigger channels

    time = pEvent->phillips[4][i+1];

    if (time > 0) {
      time |= i << 12; // Include channel number
      m_sortedData[PACKET_TRIGGER].push_back(time); // Time of each trigger channel
    }

  }

  /*****S800_TOF_PACKET****/
  for (i=S800_TOF_FIRST; i<S800_TOF_FIRST+S800_TOF_CHANNELS; i++) { // Loop over TDC ToF channels

    time = pEvent->phillips[4][i+1];

    if (time > 0) {
      time |= i << 12; // Include channel number
      m_sortedData[PACKET_TOF].push_back(time); // time-of-flight from TDC
    }
  }
  time = 0;
  time = pEvent->phillips[3][7+1]; // ToF TAC (XF_SCI) from S800 ADC
  time |= 4<<12;
  m_sortedData[PACKET_TOF].push_back(time); 
  time = 0;
  time = pEvent->phillips[3][8+1]; // ToF TAC (OBJ_SCI) from S800 ADC
  time |= 5<<12;
  m_sortedData[PACKET_TOF].push_back(time); 


  /****S800_FP_SCINT_PACKET****/
  for (i=S800_FP_SCINT_FIRST; i<S800_FP_SCINT_FIRST+S800_FP_SCINT_CHANNELS; i++) {

    energy = pEvent->fera[i];
    time = pEvent->phillips[4][i+1];

    if (energy > 0 || i == 2) { // added to ensure we always see delayed E1 up. 
      energy |= i<<12;          // In the current version, there is nothing in TDC channel i=2
      time |= i<<12;            // However, I keep it to be consistent with old data. JP 9/17/2015
      m_sortedData[PACKET_FP_SCINT].push_back(energy); 
      m_sortedData[PACKET_FP_SCINT].push_back(time); 
    }
  }


  /****S800_FP_IC_PACKET****/
  ic_energy.push_back(SUBPACKET_FP_IC_ENERGY); // Define IC energy sub-packet and fill it with subpacket tag
  for (i=S800_FP_IC_FIRST; i<S800_FP_IC_FIRST+S800_FP_IC_CHANNELS; i++) {

    energy = pEvent->phillips[0][i+1];

    if (energy > 0) { 
      energy |= i<<12;
      ic_energy.push_back(energy); // Fill IC energy sub-packet with  data
    }
  }
  size_subpacket_ic_energy = ic_energy.size()+1;

  m_sortedData[PACKET_FP_IC].push_back(size_subpacket_ic_energy); // Define IC packet and fill it with sub-packet size 
  it = ic_energy.begin(); 
  itend = ic_energy.end(); 
  while (it != itend) {
    m_sortedData[PACKET_FP_IC].push_back(*it++); // Now insert sub-packet data into IC packet 
  }
  ic_energy.clear();




  /****S800_FP_CRDC_PACKET****/

  for (i=0; i < 2; i++) {

    label = i;

    crdc_raw.push_back(SUBPACKET_FP_CRDC_RAW); // Define CRDC Raw sub-packet and fill it with subpacket tag
    crdc_raw.push_back(0); // this used to be a global threshold - now is a dummy word 
    for (j=0; j < pEvent->npads[i]; j++) {
      crdc_raw.push_back(pEvent->det_pads[i][j].sch | 0x8000);

      if (pEvent->det_pads[i][j].d[0] != 0) {      
        pEvent->det_pads[i][j].d[0] |= 0x0000;
        crdc_raw.push_back(pEvent->det_pads[i][j].d[0]);
      }

      if (pEvent->det_pads[i][j].d[1] != 0) {
        pEvent->det_pads[i][j].d[1] |= 0x0400;
        crdc_raw.push_back(pEvent->det_pads[i][j].d[1]);
      }

      if (pEvent->det_pads[i][j].d[2] != 0) {
        pEvent->det_pads[i][j].d[2] |= 0x0800;
        crdc_raw.push_back(pEvent->det_pads[i][j].d[2]);
      }

      if (pEvent->det_pads[i][j].d[3] != 0) {
        pEvent->det_pads[i][j].d[3] |= 0x0C00;
        crdc_raw.push_back(pEvent->det_pads[i][j].d[3]);
      }	  
    }
    size_subpacket_crdc_raw = crdc_raw.size() + 1;


    crdc_anode.push_back(SUBPACKET_FP_CRDC_ANODE); // Define CRDC Anode sub-packet and fill it with subpacket tag
    if (i == 0) {
      TEMP_PACKET_CRDC = PACKET_CRDC1;
      energy = pEvent->phillips[3][S800_FP_CRDC1_ANODE+1];
      time = pEvent->phillips[3][S800_FP_CRDC1_TAC+1];
      crdc_anode.push_back(energy & 0xFFF);
      crdc_anode.push_back(time & 0xFFF);
    }

    if (i == 1) {
      TEMP_PACKET_CRDC = PACKET_CRDC2;
      energy = pEvent->phillips[3][S800_FP_CRDC2_ANODE+1];
      time = pEvent->phillips[3][S800_FP_CRDC2_TAC+1];
      crdc_anode.push_back(energy & 0xFFF);
      crdc_anode.push_back(time & 0xFFF);
    }
    size_subpacket_crdc_anode = crdc_anode.size() + 1;


    m_sortedData[TEMP_PACKET_CRDC].push_back(label); // Define CRDC packet and include CRDC identifying tag
    m_sortedData[TEMP_PACKET_CRDC].push_back(size_subpacket_crdc_raw); // Fill CRDC packet with sub-packet RAW size 
    it = crdc_raw.begin(); 
    itend = crdc_raw.end();
    while (it != itend) {
      m_sortedData[TEMP_PACKET_CRDC].push_back(*it++); // Now insert RAW sub-packet data into CRDC packet 
    }
    crdc_raw.clear();

    m_sortedData[TEMP_PACKET_CRDC].push_back(size_subpacket_crdc_anode); // Fill CRDC packet with sub-packet ANODE size 
    it = crdc_anode.begin(); 
    itend = crdc_anode.end();
    while (it != itend) {
      m_sortedData[TEMP_PACKET_CRDC].push_back(*it++); // Now insert ANODE sub-packet data into CRDC packet 
    }
    crdc_anode.clear();

  }





  /****S800_FP_HODO_PACKET (ENERGIES) ****/

  label = 0; // First hodoscope set
  m_sortedData[PACKET_FP_HODO1].push_back(label); 
  for (i = S800_FP_HODO_FIRST; i < S800_FP_HODO_FIRST + S800_FP_HODO_CHANNELS; i++) {

    energy = pEvent->phillips[1][i+1]; // Energies of first set of hodoscopes

    if (energy) {
      energy |= i<<12;
      m_sortedData[PACKET_FP_HODO1].push_back(energy); 
      energy = 0;
    }
  }

  label = 1; // Second hodoscope set
  m_sortedData[PACKET_FP_HODO3].push_back(label); 
  for (i = S800_FP_HODO_FIRST; i < S800_FP_HODO_FIRST + S800_FP_HODO_CHANNELS; i++) {

    energy = pEvent->phillips[2][i+1]; // Energies of second set of hodoscopes

    if (energy) {
      energy |= i<<12;
      m_sortedData[PACKET_FP_HODO3].push_back(energy); 
      energy = 0;
    }
  }


  /****S800_FP_HODO_PACKET (HIT PATTERN) ****/
  label = 2; // Hit pattern subpacket
  m_sortedData[PACKET_FP_HODO2].push_back(label); 
  m_sortedData[PACKET_FP_HODO2].push_back(pEvent->coinc[0]); // Hit pattern firs set
  m_sortedData[PACKET_FP_HODO2].push_back(pEvent->coinc[1]);  // Hit pattern second set
  m_sortedData[PACKET_FP_HODO2].push_back(pEvent->phillips[3][S800_FP_HODO_TAC+1]); // TAC signal






  /****S800_II_TRACK_PACKET****/
  ppac_raw.push_back(SUBPACKET_II_TRACK_RAW); // Define PPAC Raw sub-packet and fill it with subpacket tag
  ppac_raw.push_back(0); // this used to be a global threshold - now is a dummy word 
  for (j = 0; j < pEvent->npads[2]; j++) {
    ppac_raw.push_back(pEvent->det_pads[2][j].sch | 0x8000);

    if (pEvent->det_pads[2][j].d[0] != 0) {      
      pEvent->det_pads[2][j].d[0] |= 0x0000;
      ppac_raw.push_back(pEvent->det_pads[2][j].d[0]);
    }

    if (pEvent->det_pads[2][j].d[1] != 0) {
      pEvent->det_pads[2][j].d[1] |= 0x0400;
      ppac_raw.push_back(pEvent->det_pads[2][j].d[1]);
    }

    if (pEvent->det_pads[2][j].d[2] != 0) {
      pEvent->det_pads[2][j].d[2] |= 0x0800;
      ppac_raw.push_back(pEvent->det_pads[2][j].d[2]);
    }

    if (pEvent->det_pads[2][j].d[3] != 0) {
      pEvent->det_pads[2][j].d[3] |= 0x0C00;
      ppac_raw.push_back(pEvent->det_pads[2][j].d[3]);
    }	  
  }
  size_subpacket_ppac_raw = ppac_raw.size() + 1;

  m_sortedData[PACKET_II_TRACK].push_back(size_subpacket_ppac_raw); // Define PPAC packet and include sub-packet RAW size 
  it = ppac_raw.begin(); 
  itend = ppac_raw.end(); 
  while (it != itend) {
    m_sortedData[PACKET_II_TRACK].push_back(*it++); // Now insert sub-packet data into PPAC packet 
  }
  ppac_raw.clear();




  /****S800_OB_PIN_PACKET****/
  m_sortedData.insert( make_pair( PACKET_OB_PIN, std::vector<uint16_t>() ) );
  energy = pEvent->phillips[3][S800_OB_PIN+1]; // PIN energy from generic S800 Phillips ADC 
  if (energy > 0) { 
    energy |= S800_OB_PIN <<12;
    m_sortedData[PACKET_OB_PIN].push_back(energy); 
  }



  /****S800_GALOTTE_PACKET****/
  m_sortedData.insert( make_pair( PACKET_GALOTTE, std::vector<uint16_t>() ) );
  for (i = S800_GALOTTE_FIRST; i < S800_GALOTTE_FIRST + S800_GALOTTE_CHANNELS; i++) {

    time = pEvent->phillips[3][i+1]; // Galotte TAC from generic S800 Phillips ADC

    if (time > 0) {
      time |= (i - S800_GALOTTE_FIRST) << 12;
      m_sortedData[PACKET_GALOTTE].push_back(time); 
    }
  }



  /****S800_LABR_PACKET****/
  m_sortedData.insert( make_pair( PACKET_LABR, std::vector<uint16_t>() ) );
  for (i = S800_LABR_FIRST; i < S800_LABR_CHANNELS; i++) {

    energy = pEvent->fera[i+4];
    time = pEvent->phillips[5][i+1]; // LaBr time from generic LaBr Phillips TDC

    if (time > 0) {

      energy |= i << 12;
      m_sortedData[PACKET_LABR].push_back(energy);
      time |= i << 12;
      m_sortedData[PACKET_LABR].push_back(time); 
    }
  }


  /****S800_VME_TDC_PACKET****/
  for (i = 0; i < 32; i++) {
    for (j = 0; j < pEvent->mtdc.hits[i]; j++) {
      if (pEvent->mtdc.data[i][j] > 0) {
        m_sortedData[PACKET_MTDC].push_back((j<<8) + i); // hit number is coded in first 8 bits
        m_sortedData[PACKET_MTDC].push_back(pEvent->mtdc.data[i][j]); // This is 16 bit data
      }
    }
  }

}




void CS800Filter::PublishData(CRingItem* item)
{
  uint16_t label = 0;


  uint16_t* pData = reinterpret_cast<uint16_t*>(item->getBodyPointer());
  // Clear out the data;
  //item->setBodyCursor(pData);

  uint16_t* pStart = reinterpret_cast<uint16_t*>(pData); // Create room for total inclusive buffer size
  pData++;
  //pData++;

  uint16_t* pPktStart = reinterpret_cast<uint16_t*>(pData); // Create room for total inclusive packet size
  pData++;

  *pData++ = PACKET_S800; // Insert S800 tag

  *pData++ = VERSION_S800; // Insert S800 DAQ version tag

  pData = appendKeyedData(pData,label,PACKET_TIMESTAMP);
  pData = appendKeyedData(pData,label,PACKET_EVENT_NUMBER);
  pData = appendKeyedData(pData,label,PACKET_TRIGGER);
  pData = appendKeyedData(pData,label,PACKET_TOF);
  pData = appendKeyedData(pData,label,PACKET_FP_SCINT);
  pData = appendKeyedData(pData,label,PACKET_FP_IC);
  pData = appendKeyedData(pData,PACKET_CRDC,PACKET_CRDC1); //Overwrite tag
  pData = appendKeyedData(pData,PACKET_CRDC,PACKET_CRDC2); //Overwrite tag
  pData = appendKeyedData(pData,PACKET_FP_HODO,PACKET_FP_HODO1); //Overwrite tag
  pData = appendKeyedData(pData,PACKET_FP_HODO,PACKET_FP_HODO3); //Overwrite tag
  pData = appendKeyedData(pData,PACKET_FP_HODO,PACKET_FP_HODO2); //Overwrite tag
  pData = appendKeyedData(pData,label,PACKET_II_TRACK);
  pData = appendKeyedData(pData,label,PACKET_OB_PIN);
  pData = appendKeyedData(pData,label,PACKET_GALOTTE);
  pData = appendKeyedData(pData,label,PACKET_LABR);
  pData = appendKeyedData(pData,label,PACKET_MTDC);


  // Compute the final size and set it
  *pStart = static_cast<uint16_t>(pData - pStart);
  *pPktStart = static_cast<uint16_t>(pData - pPktStart);


  item->setBodyCursor(pData);
  item->updateSize();


  uint16_t* readData = reinterpret_cast<uint16_t*>(item->getBodyPointer());
  size_t bsize = item->getBodySize()/sizeof(uint16_t); // Fragment body size (just body)

  int nwords = bsize-1;
  //while (nwords>0) {
  //  std::cout << "Buffer element: " << std::hex << *readData++ << ". Words left: " << std::dec << nwords << std::endl;
  //  nwords--;
  //}


}



uint16_t* CS800Filter::appendKeyedData(uint16_t* pData, uint16_t keyoverwrite, uint16_t key)
{

  std::map<uint16_t, std::vector<uint16_t> >::const_iterator vec_iter;

  // Find the keyed value 
  vec_iter = m_sortedData.find(key);

  // If key exists, then append all data
  if (vec_iter != m_sortedData.end()) {

    std::vector<uint16_t>::const_iterator it, itend;

    it = vec_iter->second.begin(); 
    itend = vec_iter->second.end(); 

    // Insert packet size
    // This is the inclusive size...includes tag and packet length
    *pData = vec_iter->second.size() + 2;
    pData++;


    // Insert packet tag
    if (keyoverwrite != 0) key = keyoverwrite; 
    *pData = key; 
    pData++;

    // Insert data
    while (it != itend) {
      *pData = *it; 

      it++;  
      pData++;
    } 
  }

  // Return the incremented pointer
  return pData;
}








void CS800Filter::PublishCorruptedData(CRingItem* item)
{

  uint16_t* pData = reinterpret_cast<uint16_t*>(item->getBodyPointer());

  uint16_t* pStart = reinterpret_cast<uint16_t*>(pData); // Create room for total inclusive buffer size
  pData++;

  uint16_t* pPktStart = reinterpret_cast<uint16_t*>(pData); // Create room for total inclusive packet size
  pData++;

  *pData++ = PACKET_BAD; // Insert Bad-packet tag where S800 tag should go


  // Compute the final size and set it
  *pStart = static_cast<uint16_t>(pData - pStart);
  *pPktStart = static_cast<uint16_t>(pData - pPktStart);


  item->setBodyCursor(pData);
  item->updateSize();


  uint16_t* readData = reinterpret_cast<uint16_t*>(item->getBodyPointer());
  size_t bsize = item->getBodySize()/sizeof(uint16_t); // Fragment body size (just body)

  int nwords = bsize-1;
  //while (nwords>0) {
  //  std::cout << "Buffer element: " << std::hex << *readData++ << ". Words left: " << std::dec << nwords << std::endl;
  //  nwords--;
  //}


}

CRingItem* CS800Filter::handleStateChangeItem(CRingStateChangeItem* pItem)
{
	assert(pItem);


	if (pItem->type() == BEGIN_RUN) {
	  //resetCounters();
	  if (!m_BOR) { 
	    resetCounters();
	    m_BOR = true;
	    m_EOR = false;
	  }
	} else if (pItem->type() == END_RUN) {
	  //printCounterSummary();
	  if (!m_EOR) {
	    printCounterSummary();
	    m_BOR = false;
	    m_EOR = true;
	  }
	}	

	return pItem;
}


void CS800Filter::resetCounters()
{
  m_eventCount = 0;

  m_error[1] = 0;
  m_error[2] = 0;
  m_error[3] = 0;
  m_error[4] = 0;
  m_error[5] = 0;
  m_error[6] = 0;
  m_error[7] = 0;
  m_error[8] = 0;
  m_error[9] = 0;
  m_error[10] = 0;
  m_error[11] = 0; 
  m_error[12] = 0; 
  m_error[13] = 0;
  m_error[14] = 0;
  m_error[15] = 0;

  m_prevtstampvmusb = 0;
  m_prevtstampccusb = 0;
  m_prevevnum = 0;


  std::stringstream msg;
  msg << " SET ERROR COUNTERS TO ZERO";
  Actions::Log ( msg.str() ); 
}

void CS800Filter::printCounterSummary()
{
   

  std::cout << "\nERROR COUNTER RESULTS" << std::endl;

  std::cout << "ERROR 1. Number of fragments in RingItem is not 2:                           " << m_error[1] << std::endl;
  std::cout << "ERROR 2. Fragment with no head:                                              " << m_error[2] << std::endl;
  std::cout << "ERROR 3. Duplicated source ID:                                               " << m_error[3] << std::endl;
  std::cout << "ERROR 4. Corrupted timestamp = 0:                                            " << m_error[4] << std::endl;
  std::cout << "ERROR 5. Uncorrelated fragments (different timestamps):                      " << m_error[5] << std::endl;
  std::cout << "ERROR 6. Body size value from body and header differ (OBSOLETE):             " << m_error[6] << std::endl;
  std::cout << "ERROR 7. Missing end-of-packet tag:                                          " << m_error[7] << std::endl;
  std::cout << "ERROR 8. Duplicated packet tag:                                              " << m_error[8] << std::endl;
  std::cout << "ERROR 9. Timestamp from header different than decoded timestamp from body:   " << m_error[9] << std::endl;
  std::cout << "ERROR 10. Incomplete RingItem (missing packet tags):                         " << m_error[10] << std::endl;
  std::cout << "ERROR 11. Unknown word found in fragment body:                               " << m_error[11] << std::endl;
  std::cout << "ERROR 12. Unknown controller type:                                           " << m_error[12] << std::endl;
  std::cout << "ERROR 13. Unknown PAD detector type:                                         " << m_error[13] << std::endl;
  std::cout << "ERROR 14. Too many pads in PAD detector:                                     " << m_error[14] << std::endl;
  std::cout << "ERROR 15. Too many MTDC channels (OBSOLETE):                                 " << m_error[15] << std::endl;
  std::cout << "ERROR 16. Non-contiguous event number:                                       " << m_error[16] << std::endl;
  std::cout << "ERROR 17. Non-increasing XLM timestamp:                                      " << m_error[17] << std::endl;
  std::cout << "ERROR 18. Non-increasing ULM timestamp:                                      " << m_error[18] << std::endl;
  std::cout << "\nTOTAL NUMBER OF EVENTS:                                                    " << m_eventCount << std::endl;

  std::cout << "----------------------------------------" << std::endl;

}



void CS800Filter::initialize() 
{
}

void CS800Filter::finalize() 
{
}

CRingItem* CS800Filter::handleScalerItem(CRingScalerItem* item) {
  
  //++m_eventCount;
  
  int i;
  int offset = 1;  

  CRingScalerItem* product;

  try {

  std::vector<uint32_t> sclrs = item->getScalers();
  std::vector<uint32_t> newSclrs(sclrs.begin()+offset, sclrs.end());



  // Mask out the upper bits [31:24]
  // This gets rid of the QX code inserted by
  // the CC-USB
  uint32_t mask = 0x00ffffff;
  uint32_t nsclr = sclrs.size();

  uint32_t tag1 = (sclrs[0] & 0xffff0000)>>16;
  uint32_t tag2 = (sclrs[0] & 0xffff);

  if (tag1 == VMUSB_SCALER_TAG) {
    return NULL;
  }

  else if (tag1 == CCUSB_SCALER_TAG) {

    for (i=0; i<nsclr-offset; ++i) {
      newSclrs[i] &= mask;
    }
    if ( item->hasBodyHeader() ) {
	    product = new CRingScalerItem(item->getEventTimestamp(), 
                            item->getSourceId(), item->getBarrierType(), 
                            item->getStartTime(), item->getEndTime(),
			    item->getTimestamp(), newSclrs,
			    item->getTimeDivisor(), item->isIncremental()
			    );
    } else {
	    product = new CRingScalerItem(item->getStartTime(), item->getEndTime(),
			    item->getTimestamp(), newSclrs,
			    item->isIncremental(), item->getTimeDivisor()
			    );
    }

  } else {
    return NULL;
  }


  product->updateSize();

  } catch (std::exception& exc) { std::stringstream msg;
	msg << "Caught a std::exception in scaler handler: " << exc.what();	
	Actions::Error(msg.str());
	Actions::EndRun();
  } catch (CException& exc) {
	std::stringstream msg;
	msg << "Caught a CException in scaler handler: " << exc.ReasonText() << " while doing: " << exc.WasDoing();
	Actions::Error(msg.str());
	Actions::EndRun();
  } catch (std::string errmsg) {
	std::stringstream msg;
	msg << "Caught a string exception in scaler handler: " << errmsg;
	Actions::Error(msg.str());
	Actions::EndRun();
  } catch (...) {
	Actions::Error("Caught an unknown exception in scaler handler! Shutting down!");
	Actions::EndRun();
  }
  return product;
}








