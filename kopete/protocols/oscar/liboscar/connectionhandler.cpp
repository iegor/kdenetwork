/*
    Kopete Oscar Protocol
    Oscar Multiple Connection Handling

    Copyright (c) 2005 Matt Rogers <mattr@kde.org>

    Kopete (c) 2002-2005 by the Kopete developers <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This library is free software; you can redistribute it and/or         *
    * modify it under the terms of the GNU Lesser General Public            *
    * License as published by the Free Software Foundation; either          *
    * version 2 of the License, or (at your option) any later version.      *
    *                                                                       *
    *************************************************************************
*/

#include "connectionhandler.h"
#include <qvaluelist.h>
#include <kdebug.h>
#include "connection.h"
#include "oscartypes.h"

class ConnectionHandler::Private
{
public:
	QValueList<Connection*> connections;
    QMap<Connection*, ConnectionRoomInfo> chatRoomConnections;
};

ConnectionHandler::ConnectionHandler()
{
	d = new Private;
}


ConnectionHandler::~ConnectionHandler()
{
	delete d;
}

void ConnectionHandler::append( Connection* c )
{
	d->connections.append( c );
}

void ConnectionHandler::remove( Connection* c )
{
	kdDebug(OSCAR_RAW_DEBUG) << k_funcinfo << "Removing connection "
		<< c << endl;
	d->connections.remove( c );
	c->deleteLater();
}

void ConnectionHandler::remove( int family )
{
	kdDebug(OSCAR_RAW_DEBUG) << k_funcinfo << "Removing all connections " <<
		"supporting family " << family << endl;
	QValueList<Connection*>::iterator it = d->connections.begin();
	QValueList<Connection*>::iterator itEnd = d->connections.end();
	for ( ; it != itEnd; ++it )
	{
		if ( ( *it )->isSupported( family ) )
		{
			Connection* c = ( *it );
			it = d->connections.remove( it );
			c->deleteLater();
		}
	}
}

void ConnectionHandler::clear()
{
	kdDebug(OSCAR_RAW_DEBUG) << k_funcinfo << "Clearing all connections"
		 << endl;
	while ( !d->connections.isEmpty() )
	{
		Connection *c = d->connections.front();
		d->connections.pop_front();
		c->deleteLater();
	}
}

Connection* ConnectionHandler::connectionForFamily( int family ) const
{
	QValueList<Connection*>::iterator it = d->connections.begin();
	QValueList<Connection*>::iterator itEnd = d->connections.end();
	int connectionCount = 0;
	Connection* lastConnection = 0;
	for ( ; it != itEnd; ++it )
	{
		if ( ( *it )->isSupported( family ) )
		{
			connectionCount++;
			lastConnection = ( *it );
		}
	}
	if ( connectionCount == 1 )
		return lastConnection;

	return 0;
}

Connection* ConnectionHandler::defaultConnection() const
{
	if ( d->connections.isEmpty() || d->connections.count() > 1 )
		return 0;

	return d->connections.first();
}

void ConnectionHandler::addChatInfoForConnection( Connection* c, Oscar::WORD exchange, const QString& room )
{
    if ( d->connections.findIndex( c ) == -1 )
        d->connections.append( c );

    ConnectionRoomInfo info = qMakePair( exchange, room );
    d->chatRoomConnections[c] = info;
}

Connection* ConnectionHandler::connectionForChatRoom( Oscar::WORD exchange, const QString& room )
{
    ConnectionRoomInfo infoToFind = qMakePair( exchange, room );
    QMap<Connection*, ConnectionRoomInfo>::iterator it,  itEnd = d->chatRoomConnections.end();
    for ( it = d->chatRoomConnections.begin(); it != itEnd; ++it )
    {
        if ( it.data() == infoToFind )
        {
            Connection* c = it.key();
            return c;
        }
    }

    return 0;
}

QString ConnectionHandler::chatRoomForConnection( Connection* c )
{
    if ( d->connections.findIndex( c ) == -1 )
        return QString::null;

    QMap<Connection*, ConnectionRoomInfo>::iterator it, itEnd = d->chatRoomConnections.end();
    for ( it = d->chatRoomConnections.begin(); it != itEnd; ++it )
    {
        if ( it.key() == c )
        {
            QString room = it.data().second;
            return room;
        }
    }

    return QString::null;
}

Oscar::WORD ConnectionHandler::exchangeForConnection( Connection* c )
{

    if ( d->connections.findIndex( c ) == -1 )
        return 0xFFFF;

    QMap<Connection*, ConnectionRoomInfo>::iterator it, itEnd = d->chatRoomConnections.end();
    for ( it = d->chatRoomConnections.begin(); it != itEnd; ++it )
    {
        if ( it.key() == c )
        {
            Oscar::WORD exchange = it.data().first;
            return exchange;
        }
    }

    return 0xFFFF;
}

