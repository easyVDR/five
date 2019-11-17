/*
 *      vdr-plugin-robotv - roboTV server plugin for VDR
 *
 *      Copyright (C) 2016 Alexander Pipelka
 *
 *      https://github.com/pipelka/vdr-plugin-robotv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#ifndef ROBOTV_EPGHANDLER_H
#define ROBOTV_EPGHANDLER_H

#include <vdr/epg.h>
#include <db/storage.h>
#include <thread>

class EpgHandler : public cEpgHandler, roboTV::Storage {
public:

    EpgHandler();

    bool SortSchedule(cSchedule *Schedule) override;

    void cleanup();

    void triggerCleanup();

private:

    bool processEvent(const cEvent *Event);

    void createDb();

    std::mutex m_mutex;
};

#endif //ROBOTV_EPGHANDLER_H