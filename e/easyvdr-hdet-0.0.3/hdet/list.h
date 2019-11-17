/* hdet, a simple multimedia hardware detection utility.
 *
 * Copyright (C) 2011 Winfried Koehler
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *                                                             
 * The author can be reached at: handygewinnspiel AT gmx DOT de
 */

#ifndef __LIST_H
#define __LIST_H

/******************************************************************************
 * double linked list as to be found in several GPL'd tools.
 *****************************************************************************/

struct list_head {
   struct list_head *next, *prev;
};

static __inline__ void __list_add(struct list_head * new,
   struct list_head * prev, struct list_head * next) {
   next->prev = new;
   new->next = next;
   new->prev = prev;
   prev->next = new;
}

static __inline__ void list_add_tail(struct list_head *new, struct list_head *head) {
   __list_add(new, head->prev, head);
}

#define list_entry(ptr, type, member) \
   ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

#endif
