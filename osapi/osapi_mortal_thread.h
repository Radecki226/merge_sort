#ifndef OSAPI_MORTAL_THREAD_H
#define OSAPI_MORTAL_THREAD_H
 
namespace osapi {

class MortalThread : public Thread
{
    public:
        MortalThread(int priority, unsigned int stackSize, const char* name = "unnamed") : Thread(priority, stackSize, JOINABLE, name),	killSignal(0) {
            ;
        }
 
        virtual ~MortalThread() {
            ;
        }

        /** Sends termination signal to the thread. */
        void kill() {
            killSignal = 1;
        }

    protected:
        virtual void begin() { ; }
        virtual void loop() = 0;
        virtual void end() { ; }

    private:
        sig_atomic_t killSignal;

        /** Implementation of the job method */
        virtual void job(void) {
            begin();
            while (killSignal == 0) {
                loop();
            }
            end();
    }
};

} // namespace osapi
 
#endif // OSAPI_MORTAL_THREAD_H