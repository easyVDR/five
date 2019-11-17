#ifndef __Ax_Vdr_Thread_H__
#define __Ax_Vdr_Thread_H__
//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   03.06.2004
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2005-09-13 11:03:31 +0200 (Di, 13 Sep 2005) $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
#include <string>

//----- C ---------------------------------------------------------------------
//----- Vdr -------------------------------------------------------------------
#include <vdr/thread.h>

//----- AxLib -----------------------------------------------------------------
//----- local -----------------------------------------------------------------
//----- other includes --------------------------------------------------------

//=============================================================================
//     forward declarations
//=============================================================================

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax  {
namespace Vdr {

//=============================================================================
//     class Thread
//=============================================================================
class Thread : public cThread
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef cThread PARENT;

    //-------------------------------------------------------------------------
    //     Start()
    //-------------------------------------------------------------------------
    /** Hide the Start()-method of cThread to enforce the usage of start()
     */
    void Start();

  public:
    //-------------------------------------------------------------------------
    //     Thread()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    Thread(const std::string &theDescription);

    //-------------------------------------------------------------------------
    //     ~Thread()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~Thread();

    //-------------------------------------------------------------------------
    //     getName()
    //-------------------------------------------------------------------------
    /** returns the name of the thread, that is the descrption given in the
     *  constructor
     */
    const std::string &getName() const;

    //-------------------------------------------------------------------------
    //     start()
    //-------------------------------------------------------------------------
    /** Sets the internal stop-condition to false and starts the thread
     *  calling \c cThread::Start().
     */
    virtual bool start();

    //-------------------------------------------------------------------------
    //     stop()
    //-------------------------------------------------------------------------
    /** Sets the internal stop-condition and sends an event to the
     *  waiting thread using \c setStopRequested() and then calls
     *  the method \c Cancel() of \c cThread to kill the thread after
     *  the given time.
     */
    virtual void stop(int theWaitSeconds = 0);

    //-------------------------------------------------------------------------
    //     isActive()
    //-------------------------------------------------------------------------
    /** Synonym for \c cThread::Active() simply to follow my habits.
     *
     */
    virtual bool isActive() { return Active(); }

    //-------------------------------------------------------------------------
    //     signalEvent()
    //-------------------------------------------------------------------------
    /** Signals the thread that an event occured -> if the thread is
     *  blocked in \c waitEvent() the call returns.
     *
     *  See the description of \c waitEvent().
     */
    virtual void signalEvent();

  protected:
    //-------------------------------------------------------------------------
    //     setStopRequested()
    //-------------------------------------------------------------------------
    /** Sets the internal stopRequested flag and sends an event to
     *  \c waitEvent() to end the thread.
     *
     *  See the description of \c waitEvent().
     */
    virtual void setStopRequested();

    //-------------------------------------------------------------------------
    //     stopRequested()
    //-------------------------------------------------------------------------
    /** Simply returns true if the internal stop-condition is set.
     */
    virtual bool stopRequested();

    //-------------------------------------------------------------------------
    //     waitEvent()
    //-------------------------------------------------------------------------
    /** waits for a call to signalEvent and returns true if
     *  the thread should continue (\c !stopRequested() \c && \c Running() )
     *  or false if the thread should end.
     *
     *  This allows \c Action() methods like this:
     *  \code
void Thread::Action()
{
while (waitEvent())
{
  do_some_processing();
} // while
}
        \endcode
     *
     *  From another thread a call to \c signalEvent() starts another 'cycle'
     *  in the Action-Loop.
     *
     *  To end the thread call \c stop() which sets the exit-condition
     *  (\c myStopRequested ), calls \c signalEvent() to leave the action-loop
     *  and then calls \c cThread::Cancel() to kill the thread after a timeout.
     */
    virtual bool waitEvent(int theTimeOutMS = 0, bool *fTimeOut = 0);

  public:
    //-------------------------------------------------------------------------
    //     setDebugFlag()
    //-------------------------------------------------------------------------
    /** Enable/disable some debug-output
     */
    static void setDebugFlag(bool fSet = true);

  private:
    //-------------------------------------------------------------------------
    //    attributes
    //-------------------------------------------------------------------------
    std::string    myName;
    bool           myStopRequested;
    cCondWait      myWaitCondition;
    static bool    myDebugFlag;

}; // class Thread

//=============================================================================
//     namespaces
//=============================================================================
} // Vdr
} // Ax

#endif
