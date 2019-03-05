#ifndef System_Detail_Thread
#define System_Detail_Thread

#include <memory>
#include <functional>
#include "IThread.h"

namespace core
{
namespace detail
{

class Thread: public IThread
{
public:
    Thread(const std::function<void()> &workFunc);
    virtual ~Thread();

    void ThreadFunction();

    virtual bool run();
    virtual void stop();
    virtual void setPriority(int priority);

    virtual bool isStarted() const
    { return handle_ != 0; }

    virtual ThreadID getCurrentThreadID();
    virtual bool isMainThread();

    static IThread::Factory getFactory()
    {
        return [](const std::function<void()>& workFunc)
        {
            return std::make_shared<Thread>(workFunc);
        }; // NOLINT
    }
protected:
    void *handle_;
    volatile bool shouldRun_;
    const std::function<void()> workFunc_;
    ThreadID mainThreadID_;
};

}
}

#endif
