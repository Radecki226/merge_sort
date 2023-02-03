#ifndef OSAPI_MUTEX_WINDOWS_H
#define OSAPI_MUTEX_WINDOWS_H

#include <Windows.h>

namespace osapi {

class Mutex : public MutexInterface
{
    HANDLE _handle;
    volatile bool _locked;
public:
    Mutex() {
        _handle = CreateMutex(NULL, FALSE, NULL);
        _locked = false;
    }
    virtual ~Mutex() {
        
    }
    
    virtual bool lock(unsigned int timeout) {
        if (_handle == NULL) return false;

        DWORD ret = WaitForSingleObject(_handle, timeout);

        if (_locked) return false;

        if (WAIT_OBJECT_0 == ret) {
            _locked = true;
            return true;
        } else {
            return false;
        }
    }

    virtual void unlock() {
        _locked = false;
        ReleaseMutex(_handle);
    }
};

} // namespace osapi

#endif // OSAPI_MUTEX_WINDOWS_H
