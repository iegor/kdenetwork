/***************************************************************************
*                               dlgIndividual.h
*                             -------------------
*
*    Revision     : $Id: dlgIndividual.h 530904 2006-04-18 00:26:19Z rohanpm $
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

#ifndef __dlgprogress_h__
#define __dlgprogress_h__

#include <qdatetime.h>

#include <qwidget.h>

#include <kio/global.h>

class Transfer;

class QLabel;
class QRadioButton;
class QTabWidget;
class QTextEdit;
class QCheckBox;
class KPushButton;
class KProgress;
class KToggleAction;
class KDateTimeWidget;

class DockIndividual;

class DlgIndividual:public QWidget
{
Q_OBJECT
public:
    DlgIndividual(Transfer * _item);
    ~DlgIndividual()
    {}
    void setLog(const QString & _msg);
    void appendLog(const QString & _msg);
    void enableOpenFile();

public slots:
    void setTotalSize(KIO::filesize_t bytes);

    void setProcessedSize(KIO::filesize_t bytes);

    void setSpeed(QString speed);
    void setPercent(unsigned long percent);

    void setCopying(const KURL & src, const KURL & dest);
    void setCanResume(bool);
    void slotKeepOpenToggled(bool);
    void slotOpenLocation();
    void slotOpenFile();

    bool keepDialogOpen() const { return bKeepDlgOpen; }
    
protected slots:
    void slotToggleAdvanced();
    void slotToggleDock();
    
protected:
    QLabel *sourceLabel;
    QLabel *destLabel;
    QLabel *speedLabel;
    QLabel *sizeLabel;
    QLabel *resumeLabel;
    QTextEdit *ml_log;

    KProgress *m_pProgressBar;
    DockIndividual * m_pDockIndividual;

    KPushButton   * openFile;
    KPushButton   * openLocation;
    KPushButton   * pbAdvanced ;

    QTabWidget    * panelAdvanced;
    KToggleAction * m_paDock;

    QDateTime qdt;
    KDateTimeWidget *spins;

    Transfer *item;

    KURL m_location;

    bool bKeepDlgOpen;

    KIO::filesize_t m_iTotalSize;
    
    bool advanced;
}

;

#endif                          // __dlgprogress_h__
