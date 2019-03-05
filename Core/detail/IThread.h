#ifndef System_Detail_IThread
#define System_Detail_IThread

#include <memory>
#include <functional>

namespace core
{
namespace detail
{
#ifndef _WIN32
#include <pthread.h>
typedef pthread_t ThreadID;
#else
typedef unsigned ThreadID;
#endif

class IThread
{
public:
    typedef std::function<std::shared_ptr<IThread>(const std::function<void()>&)> Factory;

    virtual ~IThread() = default;

    virtual bool run() = 0;
    virtual void stop() = 0;
    virtual void setPriority(int priority) = 0;

    virtual bool isStarted() const = 0;

    virtual ThreadID getCurrentThreadID() = 0;
    virtual bool isMainThread() = 0;
};

}
}

#endif
