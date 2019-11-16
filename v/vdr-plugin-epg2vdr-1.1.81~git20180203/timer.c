/*
 * timer.c: EPG2VDR plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/tools.h>
#include <vdr/menu.h>

#include "update.h"
#include "ttools.h"
#include "service.h"

//***************************************************************************
// Has Timer Changed
//***************************************************************************

int cUpdate::timerChanged()
{
   int maxSp = 0;
   int changed = no;

   selectMaxUpdSp->execute();
   maxSp = timerDb->getIntValue("UPDSP");

   if (timersTableMaxUpdsp != maxSp)
   {
      timersTableMaxUpdsp = maxSp;
      changed = yes;

      cPluginManager::CallAllServices(EPG2VDR_TIMER_UPDATED, &timersTableMaxUpdsp);
   }

   selectMaxUpdSp->freeResult();

   return changed;
}

//***************************************************************************
// Perform Timer Jobs
//***************************************************************************

int cUpdate::performTimerJobs()
{
   int createCount = 0;
   int modifyCount = 0;
   int deleteCount = 0;
   uint64_t start = cTimeMs::Now();

   tell(1, "Checking pending timer actions ..");

   // check if timer pending
   {
      timerDb->clear();
      timerDb->setValue("VDRUUID", Epg2VdrConfig.uuid);

      if (!selectPendingTimerActions->find())
      {
         selectPendingTimerActions->freeResult();
         tell(1, ".. nothing to do");
         timerJobsUpdateTriggered = no;
         return done;
      }

      selectPendingTimerActions->freeResult();
   }

   GET_TIMERS_WRITE(timers);     // get timers lock
   GET_CHANNELS_READ(channels);  // get channels lock

   // get schedules lock

#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
   cStateKey schedulesKey;
   const cSchedules* schedules = cSchedules::GetSchedulesRead(schedulesKey, 100/*ms*/);
#else
   cSchedulesLock* schedulesLock = new cSchedulesLock(false, 100/*ms*/);
   const cSchedules* schedules = (cSchedules*)cSchedules::Schedules(*schedulesLock);
#endif

   if (!schedules)
   {
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
      schedulesKey.Remove();
#else
      delete schedulesLock;
#endif
      tell(0, "Info: Can't get lock on schedules, skipping timer update!");
      return fail;
   }

   // iterate pending actions ...

   timerDb->clear();
   timerDb->setValue("VDRUUID", Epg2VdrConfig.uuid);

   for (int f = selectPendingTimerActions->find(); f && dbConnected(); f = selectPendingTimerActions->fetch())
   {
      int timerid = timerDb->getIntValue("ID");
      int doneid = na;
      long eventid = timerDb->getValue("EVENTID")->isNull() ? na : timerDb->getIntValue("EVENTID");
      tChannelID channelId = tChannelID::FromString(timerDb->getStrValue("CHANNELID"));
      const cEvent* event = 0;
      char requetedAction = timerDb->getStrValue("ACTION")[0];
      cTimer* timer = getTimerById(timers, timerid);   // lookup VDRs timer object
      int insert = !timer;

      if (!timerDb->getValue("DONEID")->isEmpty())
         doneid = timerDb->getIntValue("DONEID");

      tell(1, "DEBUG: Pending Action '%c' for timer (%d), event %ld, doneid %d", requetedAction, timerid, eventid, doneid);

      // --------------------------------
      // Delete timer request

      if (requetedAction == taDelete || requetedAction == taReject)
      {
         if (timerDb->hasValue("VDRUUID", "any"))
         {
            tell(0, "Error: Ignoring delete/reject request of timer (%d) without VDRUUID", timerid);
            timerDb->getValue("INFO")->sPrintf("Error: Ignoring delete/reject request of timer (%d) without VDRUUID", timerid);
            timerDb->setCharValue("ACTION", taFailed);
            timerDb->setCharValue("STATE", tsError);
            timerDb->update();
            updateTimerDone(timerid, doneid, tdsTimerCreateFailed);
            continue;
         }

         // delete VDRs timer

         if (timer)
         {
            if (timer->Recording())
            {
               timer->Skip();
#if defined (APIVERSNUM) && (APIVERSNUM >= 20302)
               cRecordControls::Process(timers, time(0));
#else
               cRecordControls::Process(time(0));
#endif
            }

            timers->Del(timer);
            deleteCount++;
            tell(0, "Deleted timer %d", timerid);
         }
         else
         {
            tell(0, "Info: Timer (%d) not found, ignoring delete/reject request", timerid);
         }

         updateTimerDone(timerid, doneid, requetedAction == taDelete ? tdsTimerDeleted : tdsTimerRejected);
         timerDb->setCharValue("ACTION", taAssumed);
         timerDb->setCharValue("STATE", tsDeleted);
         timerDb->update();
      }

      // --------------------------------
      // Create or Modify timer request

      else if (requetedAction == taModify || requetedAction == taAdjust || requetedAction == taCreate)
      {
         cSchedule* s = 0;

         if (!timer && (requetedAction == taModify || requetedAction == taAdjust))
         {
            tell(0, "Fatal: Timer (%d) not found, skipping modify request", timerid);

            timerDb->getValue("INFO")->sPrintf("Fatal: Timer (%d) not found, skipping modify request", timerid);
            timerDb->setCharValue("ACTION", taFailed);
            timerDb->setCharValue("STATE", tsError);
            timerDb->update();
            updateTimerDone(timerid, doneid, tdsTimerCreateFailed);
            continue;
         }

         // get schedule (channel) and optional the event

         if (!(s = (cSchedule*)schedules->GetSchedule(channelId)))
         {
            const cChannel* channel = channels->GetByChannelID(channelId);

            tell(0, "Error: Time (%d), missing channel '%s' (%s) or channel not found, ignoring request",
                 timerid, channel ? channel->Name() : "", timerDb->getStrValue("CHANNELID"));

            timerDb->getValue("INFO")->sPrintf("Error: Timer, (%d), missing channel '%s' (%s) or channel not found, ignoring request",
                                               timerid, channel ? channel->Name() : "", timerDb->getStrValue("CHANNELID"));
            timerDb->setCharValue("ACTION", taFailed);
            timerDb->setCharValue("STATE", tsError);
            timerDb->update();
            updateTimerDone(timerid, doneid, tdsTimerCreateFailed);

            // mark this channel as 'unknown'

            mapDb->clear();
            mapDb->setValue("CHANNELID", channelId.ToString());
            markUnknownChannel->execute();
            markUnknownChannel->freeResult();
            continue;
         }

         if (eventid > 0 && !(event = s->GetEvent(eventid)))
         {
            const cChannel* channel = channels->GetByChannelID(channelId);

            tell(0, "Error: Timer (%d), missing event '%ld' on channel '%s' (%s), ignoring request",
                 timerid, eventid, channel->Name(), timerDb->getStrValue("CHANNELID"));

            timerDb->getValue("INFO")->sPrintf("Error: Timer (%d), missing event '%ld' on channel '%s' (%s), ignoring request",
                                               timerid, eventid, channel->Name(), timerDb->getStrValue("CHANNELID"));
            timerDb->setCharValue("ACTION", taFailed);
            timerDb->setCharValue("STATE", tsError);
            timerDb->update();
            updateTimerDone(timerid, doneid, tdsTimerCreateFailed);

            // force reload of events

            tell(0, "Info: Trigger EPG full-reload due to missing event!");
            triggerEpgUpdate(yes);

            continue;
         }

         if (insert)
         {
            tell(1, "DEBUG: Create of timer %d for event %ld", timerid, eventid);

            // create timer ...

            if (event)
            {
               // event should run at least more the 2 Minutes

               if (getTimerByEvent(timers, event))
                  tell(0, "Warning: Timer for event (%d) '%s' already exist, creating additional timer due to request!",
                       event->EventID(), event->Title());

               if (event->StartTime() + event->Duration() - (2 * tmeSecondsPerMinute) <= time(0))
               {
                  tell(0, "Info: Event '%s' finished in the past, ignoring timer request!", event->Title());
                  timerDb->getValue("INFO")->sPrintf("Info: Event '%s' finished in the past, ignoring timer request!", event->Title());
                  timerDb->setCharValue("ACTION", taFailed);
                  timerDb->setCharValue("STATE", tsError);
                  timerDb->update();
                  updateTimerDone(timerid, doneid, tdsTimerCreateFailed);
                  continue;
               }

               timer = new cTimer(event);
            }
            else
            {
#if APIVERSNUM >= 20301
               const cChannel* channel = channels->GetByChannelID(channelId);
#else
               cChannel* channel = channels->GetByChannelID(channelId);
#endif
               timer = new cTimer(no, no, channel);  // timer without a event
            }

            // reset error message in 'reason'

            if (!timerDb->getValue("INFO")->isEmpty())
               timerDb->setValue("INFO", "");

            tell(1, "Create timer '%d'", timerid);

            if (timerDb->hasValue("VDRUUID", "any"))
            {
               tell(0, "Took timer (%d) for uuid 'any', event (%ld)", timerid, eventid);

               // mark 'na' record as assumed and create new with my UUID in primary key

               timerDb->setCharValue("ACTION", taAssumed);
               timerDb->setCharValue("STATE", tsIgnore);

               timerDb->update();

               // I take the timer -> new record will created!

               timerDb->setValue("VDRUUID", Epg2VdrConfig.uuid);
               timerDb->insert();

               // update timerid !!!

               timerid = timerDb->getLastInsertId();
               timerDb->setValue("ID", timerid);
            }
         }
         else
         {
            // modify timer ...

            if (timerDb->hasValue("VDRUUID", "any"))
            {
               tell(0, "Error: Ignoring modify request of timer (%d) without VDRUUID", timerid);
               timerDb->getValue("INFO")->sPrintf("Error: Ignoring modify request of timer (%d) without VDRUUID", timerid);
               timerDb->setCharValue("ACTION", taFailed);
               timerDb->setCharValue("STATE", tsError);
               timerDb->update();
               updateTimerDone(timerid, doneid, tdsTimerCreateFailed);
               continue;
            }

            tell(1, "Modify timer (%d), set event to (%d)", timerid, event->EventID());

// #TODO ?!?!
//             if (event && timer->Event() != event)
//                timer->SetEvent(event);
         }

         // update the timer with data from timers table ...

         updateTimerObjectFromRow(timer, timerDb->getRow(), event);

         // add / store ...

         if (insert)
         {
            timers->Add(timer);
            updateTimerDone(timerid, doneid, tdsTimerCreated);
            createCount++;
         }
         else
         {
            if (requetedAction == taAdjust && event)
            {
               // adjust time to given event ..

               cTimer* dummyTimer = new cTimer(event);
               timer->SetStart(dummyTimer->Start());
               timer->SetStop(dummyTimer->Stop());
               timer->SetDay(dummyTimer->Day());
               delete dummyTimer;
            }

            modifyCount++;
         }

         timerDb->setValue("AUX", timer->Aux());
         timerDb->setCharValue("ACTION", taAssumed);
         timerDb->setCharValue("STATE", tsPending);
         timerDb->store();
      }
   }

   selectPendingTimerActions->freeResult();

   // schedules lock freigeben

#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
   schedulesKey.Remove();
#else
   delete schedulesLock;
#endif

   if (createCount || modifyCount || deleteCount)
      timers->SetModified();

   timerJobsUpdateTriggered = no;

   tell(0, "Timer requests done, created %d, modified %d, deleted %d in %s",
        createCount, modifyCount, deleteCount, ms2Dur(cTimeMs::Now()-start).c_str());

   return success;
}

//***************************************************************************
// Timer Done to Failed
//***************************************************************************

int cUpdate::updateTimerDone(int timerid, int doneid, char state)
{
   if (doneid == na)
      return done;

   timerDoneDb->clear();
   timerDoneDb->setValue("ID", doneid);

   if (timerDoneDb->find())
   {
      // don't toggle 'D'eleted by user to re'J'ected

      if (timerDoneDb->hasCharValue("STATE", tdsTimerDeleted) && state == tdsTimerRejected)
         return done;

      timerDoneDb->setValue("TIMERID", timerid);
      timerDoneDb->setCharValue("STATE", state);
      timerDoneDb->update();
   }
   else
   {
      tell(0, "Warning: Id (%d) in timersdone for timer (%d) not found.",
           doneid, timerid);
   }

   return done;
}

//***************************************************************************
// Update Timer Table
//***************************************************************************

int cUpdate::updateTimerTable()
{
   cMutexLock lock(&timerMutex);
   int timerMod = 0;
   int cnt = 0;

   tell(1, "Updating table timers (and remove deleted and finished timers older than 2 days)");

   timerDb->deleteWhere("%s < unix_timestamp() - %d and %s in ('D','F')",
                        timerDb->getField("UPDSP")->getDbName(),
                        2 * tmeSecondsPerDay,
                        timerDb->getField("STATE")->getDbName());

   connection->startTransaction();

   // --------------------------
   // get timers lock

#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
   LOCK_TIMERS_WRITE;
   cTimers* timers = Timers;
   // cTimersLock timersLock(true);
   // cTimers* timers = timersLock.Timers();
#else
   cTimers* timers = &Timers;
#endif

   // --------------------------
   // remove deleted timers

   timerDb->clear();
   timerDb->setValue("VDRUUID", Epg2VdrConfig.uuid);

   for (int f = selectMyTimer->find(); f && dbConnected(); f = selectMyTimer->fetch())
   {
      int exist = no;

      // delete only assumed timers

      if (!timerDb->getValue("ACTION")->isEmpty() && !timerDb->hasCharValue("ACTION", taAssumed))
         continue;

      // count my timers to detect truncated (epmty) table
      // on empty table ignore known timer ids

      cnt++;

      for (const cTimer* t = timers->First(); t; t = timers->Next(t))
      {
         int timerid = getTimerIdOf(t);

         // compare by timerid

         if (timerid != na && timerDb->getIntValue("ID") == timerid)
         {
            exist = yes;
            break;
         }

         // compare by start-time and channelid

         if (t->StartTime() == timerDb->getIntValue("StartTime") &&
             strcmp(t->Channel()->GetChannelID().ToString(), timerDb->getStrValue("ChannelId")) == 0)
         {
            exist = yes;
            break;
         }
      }

      if (!exist)
      {
         int doneid = timerDb->getIntValue("DONEID");

         if (!timerDb->hasCharValue("STATE", tsFinished))
         {
            timerDb->setCharValue("STATE", tsDeleted);
            timerDb->update();
         }

         if (doneid > 0)
         {
            timerDoneDb->clear();
            timerDoneDb->setValue("ID", doneid);

            if (timerDoneDb->find() &&
                (timerDoneDb->hasCharValue("STATE", tdsTimerCreated) || timerDoneDb->hasCharValue("STATE", tdsTimerRequested)))
            {
               timerDoneDb->setCharValue("STATE", tdsTimerDeleted);
               timerDoneDb->update();
            }
         }
      }
   }

   selectMyTimer->freeResult();

   if (!cnt && timers->Count())
      tell(0, "No timer of my uuid found, assuming cleared table and ignoring the known timerids");

   // --------------------------
   // update timers

   for (cTimer* t = timers->First(); t && dbConnected(); t = timers->Next(t))
   {
      int insert = yes;
      int timerId = getTimerIdOf(t);

      // no timer id or not in table -> handle as insert!

      timerDb->clear();

      if (timerId != na && cnt)
      {
         timerDb->setValue("ID", timerId);
         timerDb->setValue("VDRUUID", Epg2VdrConfig.uuid);

         insert = !timerDb->find();
         timerDb->clearChanged();
      }

      updateRowByTimer(timerDb->getRow(), t);

      if (insert)
      {
         timerDb->setCharValue("STATE", tsPending);
         timerDb->insert();

         // get timerid of auto increment field and store to timers 'aux' data

         timerId = timerDb->getLastInsertId();
         setTimerId(t, timerId);
         timerMod++;
      }
      else if (!timerDb->getValue("STATE")->isEmpty() && strchr("DE", timerDb->getStrValue("STATE")[0]))
      {
         timerDb->setValue("STATE", "P");   // timer pending
      }

      if (insert || timerDb->getChanges())
      {
         timerDb->setValue("ID", timerId);  // set ID for update!!
         timerDb->update();                 // at least for aux (on insert case)

         tell(1, "'%s' timer for event %u '%s' at database",
              insert ? "Insert" : "Update",
              t->Event() ? t->Event()->EventID() : na,
              t->Event() ? t->Event()->Title() : t->File());
      }
      else
      {
         tell(3, "Nothing changed ... skipping db update");
      }

      timerDb->reset();
   }

   connection->commit();
   timerTableUpdateTriggered = no;

   if (timerMod)
      timers->SetModified();

   tell(1, "Updating table timers done");

   return success;
}

//***************************************************************************
// Recording Changed
//***************************************************************************

int cUpdate::recordingChanged()
{
   cMutexLock lock(&runningRecMutex);

   cRunningRecording* rr = runningRecordings.First();

   tell(3, "recording changed; rr = %p", (void*)rr);

   while (rr && dbConnected())
   {
      int timerWasDeleted = no;

      // first: update timer state ..

      if (!isEmpty(rr->aux))
      {
         int timerid = getTimerIdOf(rr->aux);
         timerDb->clear();
         timerDb->setValue("ID", timerid);
         timerDb->setValue("VDRUUID", Epg2VdrConfig.uuid);

         tell(1, "Try to lookup timer with id %d", timerid);

         if (timerDb->find())
         {
            tell(1, "Found timer %ld", timerDb->getIntValue("ID"));

            if (timerDb->hasCharValue("STATE", tsDeleted))
               timerWasDeleted = yes;
            else
               timerDb->setCharValue("STATE", rr->failed ? tsError : rr->finished ? tsFinished : tsRunning);

            timerDb->setValue("INFO", rr->info);
            timerDb->store();
         }
         else
            tell(0, "Error: Can't lookup timer, id %d not found!", timerid);

         timerDb->reset();
      }

      // second: update done entry and remove from 'running' if 'finished' ..

      if (rr->finished)
      {
         if (rr->doneid != na)
         {
            timerDoneDb->clear();
            timerDoneDb->setValue("ID", rr->doneid);

            if (timerDoneDb->find())
            {
               if (timerWasDeleted)
                  timerDoneDb->setCharValue("STATE", tdsTimerDeleted);     // -> Recording deleted
               else if (rr->failed)
                  timerDoneDb->setCharValue("STATE", tdsRecordingFailed);  // -> Recording failed
               else
                  timerDoneDb->setCharValue("STATE", tdsRecordingDone);    // -> Recording done

               tell(1, "Update 'done state' for doneid %ld to %s", rr->doneid, timerDoneDb->getStrValue("STATE"));

               timerDoneDb->update();
            }
            else
            {
               tell(0, "Error: Can't update timersdone state, doneid %ld not found!", rr->doneid);
            }
         }

         cRunningRecording* rrNext = runningRecordings.Next(rr);
         runningRecordings.Del(rr);
         rr = rrNext;

         continue;
      }

      rr = runningRecordings.Next(rr);
   }

   return done;
}
