/*
	oscarclientstream.h - Kopete Oscar Protocol
	
	Copyright (c) 2004 Matt Rogers <mattr@kde.org>
	
	Based on code Copyright (c) 2004 SuSE Linux AG <http://www.suse.com>
	Based on Iris, Copyright (C) 2003  Justin Karneges
	
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

#ifndef OSCAR_CLIENTSTREAM_H
#define OSCAR_CLIENTSTREAM_H

#include "stream.h"

// forward defines
class ByteStream;
class Client;
class Connector;
class Connection;
class Transfer;
class QHostAddress;

class ClientStream : public Stream
{
	Q_OBJECT
public:
	enum Error {
		ErrConnection = ErrCustom,  // Connection error, ask Connector-subclass what's up
		ErrNeg,                     // Negotiation error, see condition
		ErrAuth,                    // Auth error, see condition
		ErrBind                     // Resource binding error
	};
	
	enum Warning {
		WarnOldVersion,             // server uses older XMPP/Jabber "0.9" protocol  // can be customised for novell versions
		WarnNoTLS                   // there is no chance for TLS at this point
	};
	
	enum NegCond {
		HostGone,                   // host no longer hosted
		HostUnknown,                // unknown host
		RemoteConnectionFailed,     // unable to connect to a required remote resource
		SeeOtherHost,               // a 'redirect', see errorText() for other host
		UnsupportedVersion          // unsupported XMPP version
	};

	enum AuthCond {
		GenericAuthError,           // all-purpose "can't login" error
		NoMech,                     // No appropriate auth mech available
		BadProto,                   // Bad SASL auth protocol
		BadServ,                    // Server failed mutual auth
		InvalidUserId,             // bad user id
		InvalidMech,                // bad mechanism
		InvalidRealm,               // bad realm
		MechTooWeak,                // can't use mech with this authzid
		NotAuthorized,              // bad user, bad password, bad creditials
		TemporaryAuthFailure        // please try again later!
	};
	
	enum BindCond {
		BindNotAllowed,             // not allowed to bind a resource
		BindConflict                // resource in-use
	};

	ClientStream(Connector *conn, QObject *parent=0);
	~ClientStream();

	void connectToServer(const QString& server, bool auth=true);
	void accept(); // server
	bool isActive() const;
	bool isAuthenticated() const;

	// login params
	void setUsername(const QString &s);
	void setPassword(const QString &s);

	void setLocalAddr(const QHostAddress &addr, Q_UINT16 port);

	void close();
	
	/** Connection related stuff */
	void setConnection( Connection* c );
	Connection* connection() const;
	
	
	/**
	 * Are there any messages waiting to be read
	 */
	bool transfersAvailable() const;
	
	/**
	 * Read a message received from the server
	 */
	Transfer * read();

	/**
	 * Send a message to the server
	 */
	void write( Transfer* request );

	int errorCondition() const;
	QString errorText() const;

	// extrahttp://bugs.kde.org/show_bug.cgi?id=85158
/*#	void writeDirect(const QString &s); // must be for debug testing*/
	void setNoopTime(int mills);

signals:
	void connected();
	void securityLayerActivated(int);
	void authenticated(); // this signal is ordinarily emitted in processNext
	void warning(int);
public slots:
	void continueAfterWarning();

private slots:
	void cr_connected();
	void cr_error();
	/**
	 * collects wire ready outgoing data from the core protocol and sends
	 */ 
	void cp_outgoingData( const QByteArray& );
	/**
	 * collects parsed incoming data as a transfer from the core protocol and queues
	 */
	void cp_incomingData();

	void bs_connectionClosed();
	void bs_delayedCloseFinished();
	void bs_error(int); // server only
	void bs_readyRead();
	void bs_bytesWritten(int);

	void doNoop();
	void doReadyRead();
	
private:
	class Private;
	Private *d;

	void reset(bool all=false);
	void processNext();
	bool handleNeed();
	void handleError();
	void srvProcessNext();
	
	/** 
	 * convert internal method representation to wire
	 */
	static char* encode_method(Q_UINT8 method);
};

#endif
