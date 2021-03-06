/***************************************************************************
*                                slaveevent.cpp
*                             -------------------
*
*    Revision     : $Id: slaveevent.cpp 530904 2006-04-18 00:26:19Z rohanpm $
*    begin        : Tue Jan 29 2002
*    copyright    : (C) 2002 by Patrick Charbonnier
*    emai         : pch@freeshell.org
*
***************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 ***************************************************************************/


#include "slaveevent.h"

#define EVENT_TYPE (QEvent::User + 252)

SlaveEvent::SlaveEvent(Transfer * _item, unsigned int _event, Q_ULLONG _ldata):QCustomEvent(EVENT_TYPE)
{
    m_event = _event;
    m_item = _item;
    m_ldata = _ldata;
    m_msg = QString("");
}


SlaveEvent::SlaveEvent(Transfer * _item, unsigned int _event, const QString & _msg):QCustomEvent(EVENT_TYPE)
{
    m_event = _event;
    m_item = _item;
    m_ldata = 0L;
    m_msg = _msg;
}


SlaveEvent::~SlaveEvent()
{
}

unsigned int
SlaveEvent::getEvent() const
{
    return m_event;
}

Transfer *SlaveEvent::getItem() const
{
    return m_item;
}

Q_ULLONG SlaveEvent::getData() const
{
    return m_ldata;
}

const QString & SlaveEvent::getMsg() const
{
    return m_msg;
}
