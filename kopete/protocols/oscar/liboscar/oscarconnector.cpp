
/***************************************************************************
                   gwconnector.cpp  -  Socket Connector for KNetwork
                             -------------------
    begin                : Wed Jul 7 2004
    copyright            : (C) 2004 by Till Gerken <till@tantalo.net>
 
			Kopete (C) 2004 Kopete developers <kopete-devel@kde.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <kbufferedsocket.h>
#include <kdebug.h>
#include <kresolver.h>

#include "oscarconnector.h"
#include "oscarbytestream.h"

KNetworkConnector::KNetworkConnector( QObject *parent, const char */*name*/ )
		: Connector( parent )
{
	kdDebug( 14151 ) << k_funcinfo << "New KNetwork connector." << endl;

	mErrorCode = KNetwork::KSocketBase::NoError;

	mByteStream = new KNetworkByteStream( this );

	connect( mByteStream, SIGNAL ( connected () ), this, SLOT ( slotConnected () ) );
	connect( mByteStream, SIGNAL ( error ( int ) ), this, SLOT ( slotError ( int ) ) );
	mPort = 0;
}

KNetworkConnector::~KNetworkConnector()
{
	delete mByteStream;
}

void KNetworkConnector::connectToServer( const QString &server )
{
	kdDebug( 14151 ) << k_funcinfo << "Initiating connection to " << mHost << endl;
	Q_ASSERT( !mHost.isNull() );
	Q_ASSERT( mPort );

	mErrorCode = KNetwork::KSocketBase::NoError;

	if ( !mByteStream->connect ( mHost, QString::number ( mPort ) ) )
	{
		// Houston, we have a problem
		mErrorCode = mByteStream->socket()->error();
		emit error();
	}
}

void KNetworkConnector::slotConnected()
{
	kdDebug( 14151 ) << k_funcinfo << "We are connected." << endl;

	// FIXME: setPeerAddress() is something different, find out correct usage later
	//KInetSocketAddress inetAddress = mStreamSocket->address().asInet().makeIPv6 ();
	//setPeerAddress ( QHostAddress ( inetAddress.ipAddress().addr () ), inetAddress.port () );

	emit connected ();
}

void KNetworkConnector::slotError( int code )
{
	kdDebug( 14151 ) << k_funcinfo << "Error detected: " << code << endl;

	mErrorCode = code;
	emit error ();
}

int KNetworkConnector::errorCode()
{
	return mErrorCode;
}

ByteStream *KNetworkConnector::stream() const
{
	return mByteStream;
}

void KNetworkConnector::done()
{
	kdDebug ( 14151 ) << k_funcinfo << endl;
	mByteStream->close ();
}

void KNetworkConnector::setOptHostPort( const QString &host, Q_UINT16 port )
{
	kdDebug ( 14151 ) << k_funcinfo << "Manually specifying host " << host << " and port " << port << endl;

	mHost = host;
	mPort = port;

}

#include "oscarconnector.moc"

// kate: indent-width 4; replace-tabs off; tab-width 4; space-indent off;
