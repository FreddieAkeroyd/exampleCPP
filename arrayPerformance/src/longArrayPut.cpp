// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

/* longArrayPut.cpp */

/**
 * @author mrk
 * @date 2013.08.09
 */

#define epicsExportSharedSymbols
#include <pv/longArrayPut.h>

namespace epics { namespace exampleCPP { namespace arrayPerformance { 

using namespace std;
using namespace epics::pvData;
using namespace epics::pvAccess;
using namespace epics::pvaClient;



LongArrayPut::LongArrayPut(
    string const &providerName,
    string const & channelName,
    size_t arraySize,
    int iterBetweenCreateChannel,
    int iterBetweenCreateChannelPut,
    double delayTime)
: providerName(providerName),
  channelName(channelName),
  arraySize(arraySize),
  iterBetweenCreateChannel(iterBetweenCreateChannel),
  iterBetweenCreateChannelPut(iterBetweenCreateChannelPut),
  delayTime(delayTime)
{
     thread = std::auto_ptr<epicsThread>(new epicsThread(
        *this,
        threadName.c_str(),
        epicsThreadGetStackSize(epicsThreadStackSmall),
        epicsThreadPriorityLow));
     thread->start();
}

LongArrayPut::~LongArrayPut() {}

void LongArrayPut::run()
{
    PvaClientPtr pva(PvaClient::create());
    PvaClientChannelPtr pvaChannel(pva->createChannel(channelName,providerName));
    PvaClientPutPtr pvaPut(pvaChannel->createPut("value"));
    while(true) {
        TimeStamp timeStamp;
        TimeStamp timeStampLast;
        timeStampLast.getCurrent();
        int numChannelPut = 0;
        int numChannelCreate = 0;
        size_t nElements = 0;
        while(true) {
            PvaClientPutDataPtr putData(pvaPut->getData());
            PVStructurePtr pvStructure(putData->getPVStructure());
            BitSetPtr bitSet(putData->getChangedBitSet());
            PVLongArrayPtr pvLongArray(pvStructure->getSubField<PVLongArray>("value"));
            nElements += sizeof(int64) * arraySize;
            shared_vector<int64> xxx(arraySize,numChannelPut);
            shared_vector<const int64> data(freeze(xxx));
            pvLongArray->replace(data);
            bitSet->set(pvLongArray->getFieldOffset());
            pvaPut->put();
            timeStamp.getCurrent();
            double diff = TimeStamp::diff(timeStamp,timeStampLast);
            if(diff>=1.0) {
                ostringstream out;
                out << "put numChannelPut " << numChannelPut;
                out << " time " << diff ;
                double elementsPerSec = nElements;
                elementsPerSec /= diff;
                if(elementsPerSec>10.0e9) {
                     elementsPerSec /= 1e9;
                     out << " gigaElements/sec " << elementsPerSec;
                } else if(elementsPerSec>10.0e6) {
                     elementsPerSec /= 1e6;
                     out << " megaElements/sec " << elementsPerSec;
                } else if(elementsPerSec>10.0e3) {
                     elementsPerSec /= 1e3;
                     out << " kiloElements/sec " << elementsPerSec;
                } else  {
                     out << " Elements/sec " << elementsPerSec;
                }
                cout << out.str() << endl;
                timeStampLast = timeStamp;
                nElements = 0;
            }
            if(delayTime>0.0) epicsThreadSleep(delayTime);
            ++numChannelPut;
            bool createPut = false;
            if(iterBetweenCreateChannelPut!=0) {
                if(numChannelPut>=iterBetweenCreateChannelPut) createPut = true;
            }
            if(createPut) {
                 numChannelPut = 0;
                 pvaPut->destroy();
                 pvaPut = pvaChannel->createPut("value");
            }
            ++numChannelCreate;
            if(iterBetweenCreateChannel!=0) {
                if(numChannelCreate>=iterBetweenCreateChannel) {
                    pvaChannel->destroy();
                    pvaChannel = pva->createChannel(channelName,providerName);
                    pvaPut = pvaChannel->createPut("value");
                    numChannelCreate = 0;
                }
            }
            
        }
    }
}


}}}


