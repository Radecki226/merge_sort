#ifndef OSAPI_THREAD_WINDOWS_H
#define OSAPI_THREAD_WINDOWS_H

#include "windows.h"
#include "windows_proc_info.h"
#include <stdlib.h>

namespace osapi {

typedef enum  {
    WIN_INITIALIZED = 0,
    WIN_READY = 1,
    WIN_RUNNING = 2,
    WIN_STANDBY = 3,
    WIN_TERMINATED = 4,
    WIN_WAIT = 5,
    WIN_TRANSITION = 6,
    WIN_UNKNOWN = 7,
} osapi_windows_thread_t;

/** Thread interface implementation for FreeRTOS. */
class Thread : public ThreadInterface
{

    static void _threadFunction(void* argument) {
        Thread* osapiThreadObject = reinterpret_cast<Thread*>(argument);
				
        
			if (osapiThreadObject) {
                osapiThreadObject->job();
                osapiThreadObject->_handle = NULL;
                osapiThreadObject->giveSemaphore(); 
            }
				
    }
    
    int _priority;
    unsigned int _stackSize;
    Joinable _isJoinable;
    const char* _name;
    HANDLE _handle;
    HANDLE _joinSemaphore;

    void giveSemaphore()
    {
        if(_isJoinable == JOINABLE) {
            ReleaseSemaphore(_joinSemaphore, 1, NULL);
        }
    }

    int validatePriority(int priority) {
        if (priority > THREAD_PRIORITY_HIGHEST) {
            priority = THREAD_PRIORITY_HIGHEST;
        } else if (priority < THREAD_PRIORITY_LOWEST) {
            priority = THREAD_PRIORITY_LOWEST;
        } else {
            priority = priority;
        }

        return priority;
    }

    public:
        /** Thread constructor.
         *  @param[in] priority thread priority
         *  @param[in] stackSize thread stack size in bytes
         *  @param[in] isJoinable decides if the thread supports join operation or not
         *  @param[in] name optional thread name
         */
        Thread(int priority, unsigned int stackSize, Joinable isJoinable, const char* name = "unnamed") {
            _priority = validatePriority(priority);
            _stackSize = stackSize;
            _isJoinable = isJoinable;
            _name = name;

            if (isJoinable == JOINABLE){
                _joinSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
            } else {
                _joinSemaphore = NULL;
            }

            _handle = NULL;
        }
        
        /** Virtual destructor required to properly destroy derived class objects. */
        virtual ~Thread() { 
        }
        
        /** Runs the thread.
        *  @retval true if the thread was started successfully, 
        *  @retval false if the thread was not started successfully, or the thread was already running
        */
        virtual bool run() {
            if (_handle != NULL) return false;

            _handle = CreateThread(
                NULL,
                _stackSize,
                (LPTHREAD_START_ROUTINE)_threadFunction,
                this,
                CREATE_SUSPENDED,
                NULL
            );

            //Set priority before run
            bool priority_ret = SetThreadPriority(_handle, _priority);
            if (!priority_ret) return false;

            //Resume
            DWORD res = ResumeThread(_handle);

            if (res == (DWORD)-1) {
                return false;
            } else {
                return true;
            }
        }

        /** Checks if the thread is running.
        *  @retval true if the thread is running
        *  @retval false if the thread is not running
        */
        virtual bool isRunning() {
            if (_handle == NULL) return false;
            
            DWORD result = WaitForSingleObject(_handle, 0);

            if (result == WAIT_OBJECT_0) { //Terminated
                return false;
            }
            else {
                return true;
            }

        }
        
        /** Waits for the thread to finish executing, with a given timeout.
         *  @param timeout[in] number of milliseconds to wait for the thread to finish executing
         *  @retval true if the thread was successfully joined in the given time
         *  @retval false if the thread was not joined within the given time or the thread is not joinable at all
         */
        virtual bool join(unsigned int timeout) {
            if (_joinSemaphore == NULL) return false;

            DWORD ret = WaitForSingleObject(_joinSemaphore, timeout);

            if (WAIT_OBJECT_0 == ret) {
                return true;
            } else {
                return false;
            }
        }

        /** Checks, if the thread is joinable.
         *  @retval true if the thread is joinable
         *  @retval false if the thread is not joinable
         */
        virtual bool isJoinable() {
            return _isJoinable;
        }

        /** Suspends thread execution.
         *  @retval true if the thread was suspended successfully
         *  @retval false if the thread was not suspended for some reason
         */
        virtual bool suspend() {
            DWORD res = SuspendThread(_handle);

            if (res == (DWORD)-1) {
                return false;
            } else {
                return true;
            }
        }           

        /** Resumes thread execution.
         *  @retval true if the thread was resumed successfully
         *  @retval false if the thread was not resumed for some reason
         */
        virtual bool resume() {
            DWORD res = ResumeThread(_handle);

            if (res == (DWORD)-1) {
                return false;
            } else {
                return true;
            }
        }           

        /** Sets thread priority
         *  @param[in] priority new thread priority
         *  @retval true if the priority for the thread was set successfully
         *  @retval false if the priority for the thread was not set successfully for some reason
         */
        virtual bool setPriority(int priority) {

            priority = validatePriority(priority);
                        
            if (_handle == NULL) return true;

            return SetThreadPriority(_handle, priority);
        }

        /** Gets the thread priority
         *  @return current thread priority
         */
        virtual int getPriority() {
            if (_handle == NULL) {
                return _priority;
            } else {
                return GetThreadPriority(_handle);
            }
        }
        
        /** Gets thread name
         *  @return name of the thread
         */
        virtual const char* getName() {
            return _name;
        }

        /** Gets the current state of the thread
         *  @return current state of the thread
         */
        virtual ThreadState getState() {

            if (_handle == NULL) return INACTIVE;
            
            WindowsProcInfo procInfo;
            DWORD err = procInfo.Capture();

            if (err != 0) return UNKNOWN;

            SYSTEM_PROCESS_INFORMATION* processInformation = procInfo.FindProcessByPid(GetCurrentProcessId());
            if (processInformation == NULL) return UNKNOWN;

            SYSTEM_THREAD_INFORMATION* threadInformation = procInfo.FindThreadByTid(processInformation, GetThreadId(_handle));
            if (threadInformation == NULL) return UNKNOWN;

            switch (threadInformation->ThreadState) {
                case WIN_INITIALIZED:
                    return READY;
                    break;
                case WIN_READY:
                    return READY;
                    break;
                case WIN_RUNNING:
                    return RUNNING;
                    break;
                case WIN_STANDBY:
                    return READY;
                    break;
                case WIN_TERMINATED:
                    return INACTIVE;
                    break;
                case WIN_WAIT:
                    if (threadInformation->WaitReason == 5) {
                        return SUSPENDED;
                    } else {
                        return BLOCKED;
                    }
                    break;
                case WIN_TRANSITION:
                    return BLOCKED;
                    break;
                case WIN_UNKNOWN:
                    return UNKNOWN;
                    break;
                default:
                    return UNKNOWN;
            }

        }

        /** Gets the total size of the stack assigned for this thread
         *  @return total stack size in number of bytes
         */
        virtual unsigned int getTotalStackSize() {
            if (_handle == NULL) return 0;

            WindowsProcInfo procInfo;
            DWORD err = procInfo.Capture();
            if (err != 0) return 0;

            return procInfo.getTotalStackSize(_handle);
        }

        /** Gets the size of the used stack for this thread
         *  @return number of bytes used on the stack
         */
        virtual unsigned int getUsedStackSize() {

            return this->getTotalStackSize();
        }

        /** Delays thread execution for a given time.
         *  @param time[in] number of milliseconds to delay thread execution
         */
        virtual void sleep(unsigned int time) {
            Sleep(time);
        }

};

} // namespace osapi

#endif // OSAPI_THREAD_WINDOWS_H
