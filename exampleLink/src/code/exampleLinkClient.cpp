// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

/*pvaClientTestPutGet.cpp */

/**
 * @author mrk
 */

/* Author: Marty Kraimer */

#include <iostream>

#include <pv/pvaClient.h>

using namespace std;
using namespace epics::pvData;
using namespace epics::pvAccess;
using namespace epics::pvaClient;


int main(int argc,char *argv[])
{
    cout << "_____exampleLinkClient starting_______\n";
    PvaClientPtr pva = PvaClient::create();
    try {
        PvaClientPutPtr put = pva->channel("doubleArray")->put();
        PvaClientPutDataPtr putData = put->getData();
        PvaClientMonitorPtr monitor = pva->channel("exampleLink")->monitor("");
        PvaClientMonitorDataPtr pvaData = monitor->getData();
        if(!monitor->waitEvent(0.0)) {
            cout << "waitEvent returned false. Why???\n";
        } else {
            cout << "exampleLink\n" << pvaData->getPVStructure() << endl;
            monitor->releaseEvent();
        }
        {
             size_t nElements = 5;
             shared_vector<double> value(nElements);
             for(size_t i=0; i< nElements; ++i) value[i] = 0.0;
             shared_vector<const double> data(freeze(value));
             putData->putDoubleArray(data); put->put();
        }
        if(!monitor->waitEvent(0.0)) {
            cout<< "waitEvent returned false. Why???\n";
        } else {
            cout << "exampleLink\n" << pvaData->getPVStructure() << endl;
            monitor->releaseEvent();
        }
        {
             size_t nElements = 5;
             shared_vector<double> value(nElements);
             for(size_t i=0; i< nElements; ++i) value[i] = .1*(i+1);
             shared_vector<const double> data(freeze(value));
             putData->putDoubleArray(data); put->put();
        }
        if(!monitor->waitEvent(0.0)) {
            cout<< "waitEvent returned false. Why???\n";
        } else {
            cout << "exampleLink\n" << pvaData->getPVStructure() << endl;
            monitor->releaseEvent();
        }
    } catch (std::runtime_error e) {
        cout << "exception " << e.what() << endl;
        return 1;
    }
    cout << "_____exampleLinkClient done_______\n";
    return 0;
}

