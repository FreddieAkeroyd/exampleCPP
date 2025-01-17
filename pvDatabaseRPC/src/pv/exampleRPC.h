/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
/**
 * @author dgh
 * @date 2015.12.08
 */

#ifndef EXAMPLERPC_H
#define EXAMPLERPC_H

#include <pv/pvDatabase.h>
#include <pv/timeStamp.h>
#include <pv/pvTimeStamp.h>
#include "pv/point.h"

#include <shareLib.h>


namespace epics { namespace exampleCPP { namespace exampleRPC { 

typedef std::tr1::shared_ptr<epicsThread> EpicsThreadPtr;

class Device;
typedef std::tr1::shared_ptr<Device> DevicePtr;

class epicsShareClass Device : public epicsThreadRunable,
    public std::tr1::enable_shared_from_this<Device>
{
public:
    static DevicePtr create();

    POINTER_DEFINITIONS(Device);

    enum State {
        IDLE, READY, RUNNING, PAUSED
    };

    static std::string toString(State state);

    State getState();

    virtual bool init() { return false;}
    virtual void run();
    void startThread() { thread->start(); }
    void stop() {}

   class Callback : public std::tr1::enable_shared_from_this<Callback>
   {
   public:
       POINTER_DEFINITIONS(Callback);

       virtual void update(int flags) = 0;

       const static int SETPOINT_CHANGED  = 0x1;
       const static int READBACK_CHANGED  = 0x2;
       const static int STATE_CHANGED     = 0x4;
       const static int SCAN_COMPLETE     = 0x8;
   };

    void registerCallback(Callback::shared_pointer const & callback);

    bool unregisterCallback(Callback::shared_pointer const & callback);

    void update();

    Point getPositionSetpoint();

    Point getPositionReadback();

    void setSetpoint(Point sp);

    void abort();

    void configure(const std::vector<Point> & newPoints);

    void runScan();

    void pause();

    void resume();

    void stopScan();

    void rewind(int n);

private:
    Device();

    void setSetpointImpl(Point sp);

    void setReadbackImpl(Point rb);

    void setStateImpl(State state);

    void scanComplete();

    State state;
    int flags;

    Point positionSP;
    Point positionRB;

    std::vector<Callback::shared_pointer> callbacks;

    size_t index;

    std::vector<Point> points;

    epics::pvData::Mutex mutex;
    EpicsThreadPtr thread;
};

class AbortService;
typedef std::tr1::shared_ptr<AbortService> AbortServicePtr;

class ConfigureService;
typedef std::tr1::shared_ptr<ConfigureService> ConfigureServicePtr;

class RunService;
typedef std::tr1::shared_ptr<RunService> RunServicePtr;

class PauseService;
typedef std::tr1::shared_ptr<PauseService> PauseServicePtr;

class ResumeService;
typedef std::tr1::shared_ptr<ResumeService> ResumeServicePtr;

class StopService;
typedef std::tr1::shared_ptr<StopService> StopServicePtr;

class RewindService;
typedef std::tr1::shared_ptr<RewindService> RewindServicePtr;

class ScanService;
typedef std::tr1::shared_ptr<ScanService> ScanServicePtr;



class ExampleRPC;
typedef std::tr1::shared_ptr<ExampleRPC> ExampleRPCPtr;



class epicsShareClass AbortService :
    public epics::pvAccess::RPCServiceAsync
{
public:
    POINTER_DEFINITIONS(AbortService);

    static AbortService::shared_pointer create(ExampleRPCPtr const & pvRecord)
    {
        return AbortServicePtr(new AbortService(pvRecord));
    }
    ~AbortService() {};
 
    void request(
        epics::pvData::PVStructurePtr const & args,
        epics::pvAccess::RPCResponseCallback::shared_pointer const & callback
    );
private:
    AbortService(ExampleRPCPtr const & pvRecord)
    : pvRecord(pvRecord)
    {
    }

    ExampleRPCPtr pvRecord;
};


class epicsShareClass ConfigureService :
    public virtual epics::pvAccess::RPCServiceAsync
{
public:
    POINTER_DEFINITIONS(ConfigureService);

    static ConfigureService::shared_pointer create(ExampleRPCPtr const & pvRecord)
    {
        return ConfigureServicePtr(new ConfigureService(pvRecord));
    }
    ~ConfigureService() {};

    void request(
        epics::pvData::PVStructurePtr const & args,
        epics::pvAccess::RPCResponseCallback::shared_pointer const & callback
    );
private:
    ConfigureService(ExampleRPCPtr const & pvRecord)
    : pvRecord(pvRecord)
    {
    }

    ExampleRPCPtr pvRecord;
};


class epicsShareClass RunService :
    public virtual epics::pvAccess::RPCServiceAsync
{
public:
    POINTER_DEFINITIONS(RunService);

    static RunService::shared_pointer create(ExampleRPCPtr const & pvRecord)
    {
        return RunServicePtr(new RunService(pvRecord));
    }
    ~RunService() {};

    void request(
        epics::pvData::PVStructurePtr const & args,
        epics::pvAccess::RPCResponseCallback::shared_pointer const & callback
    ); 
private:
    RunService(ExampleRPCPtr const & pvRecord)
    : pvRecord(pvRecord)
    {
    }

    ExampleRPCPtr pvRecord;
};

class epicsShareClass PauseService :
    public virtual epics::pvAccess::RPCServiceAsync
{
public:
    POINTER_DEFINITIONS(PauseService);

    static PauseService::shared_pointer create(ExampleRPCPtr const & pvRecord)
    {
        return PauseServicePtr(new PauseService(pvRecord));
    }
    ~PauseService() {};

    void request(
        epics::pvData::PVStructurePtr const & args,
        epics::pvAccess::RPCResponseCallback::shared_pointer const & callback
    ); 
private:
    PauseService(ExampleRPCPtr const & pvRecord)
    : pvRecord(pvRecord)
    {
    }

    ExampleRPCPtr pvRecord;
};

class epicsShareClass ResumeService :
    public virtual epics::pvAccess::RPCServiceAsync
{
public:
    POINTER_DEFINITIONS(ResumeService);

    static ResumeService::shared_pointer create(ExampleRPCPtr const & pvRecord)
    {
        return ResumeServicePtr(new ResumeService(pvRecord));
    }
    ~ResumeService() {};

    void request(
        epics::pvData::PVStructurePtr const & args,
        epics::pvAccess::RPCResponseCallback::shared_pointer const & callback
    ); 
private:
    ResumeService(ExampleRPCPtr const & pvRecord)
    : pvRecord(pvRecord)
    {
    }

    ExampleRPCPtr pvRecord;
};

class epicsShareClass StopService :
    public virtual epics::pvAccess::RPCServiceAsync
{
public:
    POINTER_DEFINITIONS(StopService);

    static StopService::shared_pointer create(ExampleRPCPtr const & pvRecord)
    {
        return StopServicePtr(new StopService(pvRecord));
    }
    ~StopService() {};

    void request(
        epics::pvData::PVStructurePtr const & args,
        epics::pvAccess::RPCResponseCallback::shared_pointer const & callback
    ); 
private:
    StopService(ExampleRPCPtr const & pvRecord)
    : pvRecord(pvRecord)
    {
    }

    ExampleRPCPtr pvRecord;
};

class epicsShareClass RewindService :
    public virtual epics::pvAccess::RPCServiceAsync
{
public:
    POINTER_DEFINITIONS(RewindService);

    static RewindService::shared_pointer create(ExampleRPCPtr const & pvRecord)
    {
        return RewindServicePtr(new RewindService(pvRecord));
    }
    ~RewindService() {};

    void request(
        epics::pvData::PVStructurePtr const & args,
        epics::pvAccess::RPCResponseCallback::shared_pointer const & callback
    ); 
private:
    int getRequestedSteps(epics::pvData::PVStructurePtr const & args);

private:
    RewindService(ExampleRPCPtr const & pvRecord)
    : pvRecord(pvRecord)
    {
    }

    ExampleRPCPtr pvRecord;
};

class epicsShareClass ScanService :
    public epics::pvAccess::RPCServiceAsync,
    public std::tr1::enable_shared_from_this<ScanService>
{
public:
    POINTER_DEFINITIONS(ScanService);

    virtual void update(int flags);

    class Callback : public Device::Callback
    {
    public:
        POINTER_DEFINITIONS(Callback);
        static Callback::shared_pointer create(ScanServicePtr const & record);

        virtual void update(int flags);

    private:
        Callback(ScanServicePtr service)
        : service(service)
        {}

        ScanServicePtr service;
    };

    static ScanService::shared_pointer create(ExampleRPCPtr const & pvRecord)
    {
        return ScanServicePtr(new ScanService(pvRecord));
    }
    
    void request(
        epics::pvData::PVStructurePtr const & args,
        epics::pvAccess::RPCResponseCallback::shared_pointer const & callback
    );
private:
    ScanService(ExampleRPCPtr const & pvRecord)
    : pvRecord(pvRecord)
    {
    }

    virtual void stateChanged(Device::State state);

    virtual void scanComplete();

    void handleError(const std::string & message);

    epics::pvAccess::RPCResponseCallback::shared_pointer rpcCallback;

    Callback::shared_pointer deviceCallback;

    ExampleRPCPtr pvRecord;
};



class epicsShareClass ExampleRPC :
    public epics::pvDatabase::PVRecord
{
public:
    POINTER_DEFINITIONS(ExampleRPC);
    static ExampleRPCPtr create(
        std::string const & recordName);
    virtual ~ExampleRPC() {}
    virtual bool init() {return false;}
    virtual void process();
    virtual epics::pvAccess::RPCServiceAsync::shared_pointer getService(
        epics::pvData::PVStructurePtr const & pvRequest);

    class Callback : public Device::Callback
    {
    public:
        POINTER_DEFINITIONS(Callback);
        static Callback::shared_pointer create(ExampleRPCPtr const & record);

        virtual void update(int flags);

    private:
        Callback(ExampleRPCPtr record)
        : record(record)
        {}
        ExampleRPCPtr record;
    };

    virtual void update(int flags);

    DevicePtr getDevice() { return device; }

private:

    ExampleRPC(std::string const & recordName,
        epics::pvData::PVStructurePtr const & pvStructure);
    void initPvt();

    epics::pvData::PVDoublePtr      pvx;
    epics::pvData::PVDoublePtr      pvy;
    epics::pvData::PVDoublePtr      pvx_rb;
    epics::pvData::PVDoublePtr      pvy_rb;

    epics::pvData::PVIntPtr         pvStateIndex;
    epics::pvData::PVStringArrayPtr pvStateChoices;

    epics::pvData::PVTimeStamp pvTimeStamp;
    epics::pvData::PVTimeStamp pvTimeStamp_sp;
    epics::pvData::PVTimeStamp pvTimeStamp_rb;
    epics::pvData::PVTimeStamp pvTimeStamp_st;

    bool firstTime;

    DevicePtr device;
};


}}}

#endif  /* EXAMPLERPC_H */
