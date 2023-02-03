#ifndef OSAPI_RECURSIVE_MUTEX_WINDOWS_H
#define OSAPI_RECURSIVE_MUTEX_WINDOWS_H

namespace osapi {

class RecursiveMutex : public MutexInterface
{
    HANDLE _handle;
public:
    RecursiveMutex() {
        _handle = CreateMutex(NULL, FALSE, NULL);
    }
    virtual ~RecursiveMutex() {
        
    }
    virtual bool lock(unsigned int timeout) {
        if (_handle == NULL) return false;

        DWORD ret = WaitForSingleObject(_handle, timeout);

        if (WAIT_OBJECT_0 == ret) {
            return true;
        } else {
            return false;
        }
    }
    virtual void unlock() {
        ReleaseMutex(_handle);
    }
};

} // namespace osapi

#endif // OSAPI_RECURSIVE_MUTEX_WINDOWS_H
