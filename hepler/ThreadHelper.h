/************************************************************************/
/* 说明 ： 线程简单封装，简化线程操作                                   */
/* Coder： xiongjiming                                                  */
/* time ： 2015年11月13日 12:58:15                                      */
/************************************************************************/
#pragma once
#include <list>

class ThreadHelper
{
public:
    friend unsigned __stdcall ThreadFun(void* lpParam);
    class ThreadHelperDelegate;

public:
    ThreadHelper(void);
    ~ThreadHelper(void);

    bool Create(ThreadHelperDelegate* pDelegate, bool bStart = true);
    bool Start();
    void Stop(int time = INFINITE);

private:
    static unsigned __stdcall ThreadFun(void* lpParam);
   
public:
    class ThreadHelperDelegate
    {
    public:
        virtual void Run() = 0;
    };
    
private:
    DWORD                       m_dwThreadID;
    HANDLE                      m_hThreadHandle;
    ThreadHelperDelegate*       m_pThreadDelegate;
    HANDLE                      m_hStopEvent;
    LONG                        m_bExit;
};



class ThreadLoopHelper: ThreadHelper::ThreadHelperDelegate
{
public:
    ThreadLoopHelper();
    ~ThreadLoopHelper();

    bool Init(DWORD dwWaitTime = INFINITE);

    void UnInit();

private:
    class Event
    {
    public:
        typedef void (*fun)(void*);
    public:
        Event(fun f,void* lpParam);
        ~Event();

        void DoIt();

    private:
        Event& operator=(const Event& );
        Event(const Event& );

    private:
        fun     m_fEvent;
        void*   m_pParam;
    };

    typedef std::list<Event*>    EventList;


public:
    bool PostEvent(ThreadLoopHelper::Event::fun f,void* lpParam);
    bool PostEventToHead(ThreadLoopHelper::Event::fun f,void* lpParam);

public:
    static ThreadLoopHelper& GetInstance();

private:
    virtual void Run();

private:
    ThreadHelper        m_ThreadHelper;
    bool                m_bInit;
    LONG volatile       m_bLoop;
    EventList           m_EventList;
    CRITICAL_SECTION    m_Section;
    HANDLE              m_hPostEvent;
	DWORD				m_WaitTime;
};


class ThreadClockHeler
{
public:
	ThreadClockHeler();
	~ThreadClockHeler();

	void	Lock();
	void	UnLock();
	

private:
	CRITICAL_SECTION    m_Section;
};


class ThreadAutoClockHeler
{
public:
	ThreadAutoClockHeler(ThreadClockHeler* clock);
	~ThreadAutoClockHeler();

private:
	ThreadAutoClockHeler(const ThreadClockHeler&);
	ThreadAutoClockHeler operator=(const ThreadAutoClockHeler&);

private:
	ThreadClockHeler*	m_clock;
};