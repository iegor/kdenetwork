/*
    Kopete Oscar Protocol
    oscartypes.h - Oscar Type Definitions

    Copyright (c) 2004 Matt Rogers <mattr@kde.org>

    Kopete (c) 2002-2004 by the Kopete developers <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This library is free software; you can redistribute it and/or         *
    * modify it under the terms of the GNU Lesser General Public            *
    * License as published by the Free Software Foundation; either          *
    * version 2 of the License, or (at your option) any later version.      *
    *                                                                       *
    *************************************************************************
*/

#ifndef _OSCARTYPES_H_
#define _OSCARTYPES_H_

#include "oscartypeclasses.h"
#include <qglobal.h>
#include <qdatetime.h>
#include <qstring.h>

//! Debug Areas
const int OSCAR_RAW_DEBUG = 14151;
const int OSCAR_GEN_DEBUG = 14150;
const int OSCAR_AIM_DEBUG = 14152;
const int OSCAR_ICQ_DEBUG = 14153;

namespace Oscar
{
//! Capabilities
enum Capabilities
{
	CAP_CHAT = 0, CAP_VOICE, CAP_SENDFILE, CAP_ISICQ, CAP_IMIMAGE, CAP_BUDDYICON, CAP_SAVESTOCKS,
	CAP_GETFILE, CAP_ICQSERVERRELAY, CAP_GAMES, CAP_GAMES2, CAP_SENDBUDDYLIST, CAP_RTFMSGS, CAP_IS_2001,
	CAP_TRILLIAN, CAP_TRILLIANCRYPT, CAP_APINFO, CAP_UTF8, CAP_TYPING, CAP_INTEROPERATE, CAP_KOPETE, CAP_MICQ,
	CAP_MACICQ, CAP_SIMOLD, CAP_SIMNEW, CAP_XTRAZ, CAP_STR_2001, CAP_STR_2002, CAP_LAST
};

typedef unsigned char cap[16];
const cap oscar_caps[] =
{
	//CAP_CHAT,
	{0x74, 0x8f, 0x24, 0x20, 0x62, 0x87, 0x11, 0xd1,
			0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00},

	//CAP_VOICE,
	{0x09, 0x46, 0x13, 0x41, 0x4c, 0x7f, 0x11, 0xd1,
			0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00},

	// CAP_SENDFILE,
	{0x09, 0x46, 0x13, 0x43, 0x4c, 0x7f, 0x11, 0xd1,
			0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00},

	// CAP_ISICQ,
	{0x09, 0x46, 0x13, 0x44, 0x4c, 0x7f, 0x11, 0xd1,
			0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00},

	// CAP_IMIMAGE,
	{0x09, 0x46, 0x13, 0x45, 0x4c, 0x7f, 0x11, 0xd1,
			0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00},

	// CAP_BUDDYICON,
	{0x09, 0x46, 0x13, 0x46, 0x4c, 0x7f, 0x11, 0xd1,
			0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00},

	// CAP_SAVESTOCKS,
	{0x09, 0x46, 0x13, 0x47, 0x4c, 0x7f, 0x11, 0xd1,
			0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00},

	// CAP_GETFILE,
	{0x09, 0x46, 0x13, 0x48, 0x4c, 0x7f, 0x11, 0xd1,
			0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00},

	// CAP_ICQSERVERRELAY,
	{0x09, 0x46, 0x13, 0x49, 0x4c, 0x7f, 0x11, 0xd1,
			0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00},

	// CAP_GAMES,
	{0x09, 0x46, 0x13, 0x4a, 0x4c, 0x7f, 0x11, 0xd1,
			0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00},

	// CAP_GAMES2,
	{0x09, 0x46, 0x13, 0x4a, 0x4c, 0x7f, 0x11, 0xd1,
			0x22, 0x82, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00},

	// CAP_SENDBUDDYLIST,
	{0x09, 0x46, 0x13, 0x4b, 0x4c, 0x7f, 0x11, 0xd1,
			0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00},

	// CAP_RTFMSGS,
	{0x97, 0xb1, 0x27, 0x51, 0x24, 0x3c, 0x43, 0x34,
			0xad, 0x22, 0xd6, 0xab, 0xf7, 0x3f, 0x14, 0x92},

	// CAP_IS_2001,
	{0x2e, 0x7a, 0x64, 0x75, 0xfa, 0xdf, 0x4d, 0xc8,
			0x88, 0x6f, 0xea, 0x35, 0x95, 0xfd, 0xb6, 0xdf},

	// CAP_TRILLIAN
	{0x97, 0xb1, 0x27, 0x51, 0x24, 0x3c, 0x43, 0x34,
			0xad, 0x22, 0xd6, 0xab, 0xf7, 0x3f, 0x14, 0x09},

	// CAP_TRILLIANCRYPT
	{0xf2, 0xe7, 0xc7, 0xf4, 0xfe, 0xad, 0x4d, 0xfb,
			0xb2, 0x35, 0x36, 0x79, 0x8b, 0xdf, 0x00, 0x00},

	// CAP_APINFO,
	{0xAA, 0x4A, 0x32, 0xB5, 0xF8, 0x84, 0x48, 0xc6,
			0xA3, 0xD7, 0x8C, 0x50, 0x97, 0x19, 0xFD, 0x5B},

	// CAP_UTF8,
	{0x09, 0x46, 0x13, 0x4E, 0x4C, 0x7F, 0x11, 0xD1,
			0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00},

	// CAP_TYPING - client supports mini typing notifications
	{0x56, 0x3F, 0xC8, 0x09, 0x0B, 0x6f, 0x41, 0xBD,
			0x9F, 0x79, 0x42, 0x26, 0x09, 0xDF, 0xA2, 0xF3},

	// CAP_INTEROPERATE,
	{0x09, 0x46, 0x13, 0x4D, 0x4C, 0x7F, 0x11, 0xD1,
			0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00},

	// CAP_KOPETE,
	// last 4 bytes determine version
	// NOTE change with each Kopete Release!
	// first number, major version
	// second number,  minor version
	// third number, point version 100+
	// fourth number,  point version 0-99
	{'K', 'o', 'p', 'e', 't', 'e', ' ', 'I',
			'C', 'Q', ' ', ' ', 0, 12, 0, 7},

	// CAP_MICQ
	// last 4 bytes determine version
	{0x6d, 0x49, 0x43, 0x51, 0x20, 0xa9, 0x20, 0x52,
			0x2e, 0x4b, 0x2e, 0x20, 0x00, 0x00, 0x00, 0x00},

	// CAP_MACICQ
	{0xDD, 0x16, 0xF2, 0x02, 0x84, 0xE6, 0x11, 0xD4,
			0x90, 0xDB, 0x00, 0x10, 0x4B, 0x9B, 0x4B, 0x7D},

	// CAP_SIMOLD
	// last byte determines version
	// (major + 1) << 6 + minor
	{0x97, 0xB1, 0x27, 0x51, 0x24, 0x3C, 0x43, 0x34,
			0xAD, 0x22, 0xD6, 0xAB, 0xF7, 0x3F, 0x14, 0x00},

	// CAP_SIMNEW
	// last 4 bytes determine version (US-ASCII encoded)
	{'S', 'I', 'M', ' ', 'c', 'l', 'i', 'e',
			'n', 't', ' ', ' ',  0 ,  0 ,  0 , 0},

	// CAP_XTRAZ
	{0x1A, 0x09, 0x3C, 0x6C, 0xD7, 0xFD, 0x4E, 0xC5,
			0x9D, 0x51, 0xA6, 0x47, 0x4E, 0x34, 0xF5, 0xA0},

	// CAP_STR_2001
	{0xA0, 0xE9, 0x3F, 0x37, 0x4C, 0x7F, 0x11, 0xD1,
			0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00},

	// CAP_STR_2002
	{0x10, 0xCF, 0x40, 0xD1, 0x4C, 0x7F, 0x11, 0xD1,
			0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00},

	// CAP_LAST,
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

//! Oscar Data Types
typedef Q_UINT8 BYTE;
typedef Q_UINT16 WORD;
typedef Q_UINT32 DWORD;


struct FLAP
{
	BYTE channel;
	WORD sequence;
	WORD length;
};

struct SNAC
{
	WORD family;
	WORD subtype;
	WORD flags;
	DWORD id;
};

struct RateInfo
{
	WORD classId;
	DWORD windowSize;
	DWORD initialLevel;
	DWORD clearLevel;
	DWORD alertLevel;
	DWORD limitLevel;
	DWORD disconnectLevel;
	DWORD currentLevel;
	DWORD maxLevel;
	DWORD lastTime;
	BYTE currentState;
};

struct ChatExchangeInfo
{
	WORD number;
    WORD maxRooms;
    WORD maxRoomNameLength;
	WORD maxMsgLength;
	BYTE flags;
	QString description;
	BYTE canCreate;
	QString charset1;
	QString charset2;
	QString lang1;
	QString lang2;
};

struct ChatRoomInfo
{
	WORD exchange;
	QByteArray cookie;
	WORD instance;
	QString description;
	WORD maxMsgLength;
	QString name;
};

struct ClientVersion
{
	QString clientString;
	WORD clientId;
	WORD major;
	WORD minor;
	WORD point;
	WORD build;
	DWORD other;
	QString country;
	QString lang;
};

	/* ICQ Version Characteristics */
	const unsigned char ICQ_TCP_VERSION 	= 0x0008;

	/* AIM Version Characteristics */
	const char AIM_MD5_STRING[]     = "AOL Instant Messenger (SM)";

	/* SSI types */
	const WORD ROSTER_CONTACT       = 0x0000; // a normal contact
	const WORD ROSTER_GROUP         = 0x0001; // a group of contacts
	const WORD ROSTER_VISIBLE       = 0x0002; // a contact on the visible list
	const WORD ROSTER_INVISIBLE     = 0x0003; // a contact on the invisible list
	const WORD ROSTER_VISIBILITY    = 0x0004; // this entry contains visibility setting TLV(0xca)=TLV(202)
	const WORD ROSTER_PRESENCE      = 0x0005; // Presence info (if others can see your idle status, etc)
	const WORD ROSTER_ICQSHORTCUT   = 0x0009; // Unknown or ICQ2k shortcut bar items
	const WORD ROSTER_IGNORE        = 0x000e; // a contact on the ignore list
	const WORD ROSTER_LASTUPDATE    = 0x000F; // Last update date (name: "LastUpdateDate")
	const WORD ROSTER_NONICQ        = 0x0010; // a non-icq contact, no UIN, used to send SMS
	const WORD ROSTER_IMPORTTIME    = 0x0013; // roster import time (name: "Import time")
	const WORD ROSTER_BUDDYICONS    = 0x0014; // Buddy icon info. (names: from "0" and incrementing by one)

    /* User classes/statuses */
    const WORD CLASS_UNCONFIRMED    = 0x0001; // AOL Unconfirmed user
    const WORD CLASS_ADMINISTRATOR  = 0x0002; // AOL Administrator
    const WORD CLASS_AOL            = 0x0004; // AOL Staff
    const WORD CLASS_COMMERCIAL     = 0x0008; // AOL commercial account
    const WORD CLASS_FREE           = 0x0010; // ICQ non-commerical account
    const WORD CLASS_AWAY           = 0x0020; // Away status
    const WORD CLASS_ICQ            = 0x0040; // ICQ user
    const WORD CLASS_WIRELESS       = 0x0080; // AOL wireless user
    const WORD CLASS_UNKNOWN100     = 0x0100; // Unknown
    const WORD CLASS_UNKNOWN400     = 0x0400; // Unknown
    const WORD CLASS_UNKNOWN800     = 0x0800; // Unknown

    const WORD STATUS_ONLINE        = 0x0000; // Online
    const WORD STATUS_AWAY          = 0x0001; // Away
    const WORD STATUS_DND           = 0x0002; // Do not Disturb
    const WORD STATUS_NA            = 0x0004; // Not Available
    const WORD STATUS_OCCUPIED      = 0x0010; // Occupied (BUSY/BISY)
    const WORD STATUS_FREE4CHAT     = 0x0020; // Free for chat
    const WORD STATUS_INVISIBLE     = 0x0100; // Invisible
}

#endif

//kate: tab-width 4; indent-mode csands;
