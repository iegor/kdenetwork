/***************************************************************************
*                               dlgSystem.h
*                             -------------------
*
*    Revision     : $Id: dlgSystem.h 476275 2005-11-01 01:30:58Z thiago $
*    begin        : Tue Jan 29 2002
*    copyright    : (C) 2002 by Patrick Charbonnier
*                 : Based On Caitoo v.0.7.3 (c) 1998 - 2000, Matej Koss
*    email        : pch@freeshell.org
*
****************************************************************************/

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




#ifndef _DLGSYSTEM_H
#define _DLGSYSTEM_H

#include "dlgsystembase.h"

class DlgSystem : public DlgSystemBase
{
Q_OBJECT public:
    DlgSystem(QWidget * parent);
    ~DlgSystem() {}

    void applyData();
    void setData();

signals:
    void configChanged();

protected slots:
    void slotChanged();
};

#endif                          // _DLGSYSTEM_H
