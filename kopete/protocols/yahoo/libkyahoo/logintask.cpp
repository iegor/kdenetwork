/*
    Kopete Yahoo Protocol
    Handles logging into to the Yahoo service

    Copyright (c) 2004 Duncan Mac-Vicar P. <duncan@kde.org>
    Copyright (c) 2005-2006 André Duffeck <duffeck@kde.org>
    Copyright     2009 Matt Rogers <mattr@kde.org>

    Kopete (c) 2002-2009 by the Kopete developers <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This library is free software; you can redistribute it and/or         *
    * modify it under the terms of the GNU Lesser General Public            *
    * License as published by the Free Software Foundation; either          *
    * version 2 of the License, or (at your option) any later version.      *
    *                                                                       *
    *************************************************************************
*/



#include "logintask.h"
#include "transfer.h"
#include "ymsgtransfer.h"
#include "yahootypes.h"
#include "client.h"
#include <qstring.h>
#include <kdebug.h>
#include <stdlib.h>

#include <kio/job.h>
#include <kmdcodec.h>

extern "C"
{
#include "libyahoo.h"
}

LoginTask::LoginTask(Task* parent) : Task(parent)
{
	kdDebug(YAHOO_RAW_DEBUG) ;
	mState = InitialState;
}

LoginTask::~LoginTask()
{

}

bool LoginTask::take(Transfer* transfer)
{
	/*
	  Yahoo login task has various stages

	  1 .- Initial State
	      1.1 .- OnGo is called
	      1.2 .- SendVerify() - send a service verify ack
	  2.- SentVerify
	      2.1 - take(), get a useless transfer, sendAuth is called
	  3.- SentAuth
	      2.2 - take(), get a transfer with login and challenge string
	            sendAuthResp is called.
              2.3 - Need to decode and send a transfer back
	  4.- SentAuthResp
	*/

	if ( !forMe( transfer ) )
		return false;

	YMSGTransfer *t = static_cast<YMSGTransfer *>(transfer);


	if ( t->service() == Yahoo::ServicePing) {
		emit buddyListReady();
		return true;
	}

	switch (mState)
	{
		case (InitialState):
			client()->notifyError( "Error in login procedure.", "take called while in initial state", Client::Debug );
			return false;
		break;
		case (SentVerify):
			sendAuth( t );
			return true;
		break;
		case (SentAuth):
			sendAuthResp( t );
			return true;
		break;
		case (SentAuthResp):
			parseCookies( t );
			handleAuthResp( t );
			// Throw transfer to the next task as it contains further data
			return false;
		break;
		default:
		return false;
		break;
	}
}

bool LoginTask::forMe(const Transfer* transfer) const
{
	const YMSGTransfer *t = 0L;
	t = dynamic_cast<const YMSGTransfer*>(transfer);
	if (!t)
		return false;

	if ( t->service() == Yahoo::ServicePing)
		return true;

	switch (mState)
	{
		case (InitialState):
			//there shouldn't be a incoming transfer for this task at this state
			return false;
		break;
		case (SentVerify):
			if ( t->service() == Yahoo::ServiceVerify )
			return true;
		break;
		case (SentAuth):
			if ( t->service() == Yahoo::ServiceAuth )
			return true;
		break;
		case (SentAuthResp ):
			if ( t->service() == Yahoo::ServiceList ||
				t->service() == Yahoo::ServiceAuthResp )
			return true;
		default:
			return false;
		break;
	}
	return false;
}

void LoginTask::onGo()
{
	kdDebug(YAHOO_RAW_DEBUG) ;
	/* initial state, we have to send a ServiceVerify */
	if (mState == InitialState)
		sendVerify();
	else
		client()->notifyError( "Error in login procedure.", "onGo called while not in initial state", Client::Debug );
}

void LoginTask::reset()
{
	mState = InitialState;
}

void LoginTask::sendVerify()
{
	/* send a ServiceVerify */
	kdDebug(YAHOO_RAW_DEBUG) ;
	YMSGTransfer *t = new YMSGTransfer(Yahoo::ServiceVerify);
	send( t );
	mState = SentVerify;
}

void LoginTask::sendAuth(YMSGTransfer* transfer)
{
	kdDebug(YAHOO_RAW_DEBUG) ;
	// transfer is the verify ack transfer, no useful data in it.
	Q_UNUSED(transfer);

	/* got ServiceVerify ACK, send a ServiceAuth with username */
	kdDebug(YAHOO_RAW_DEBUG) ;
	YMSGTransfer *t = new YMSGTransfer( Yahoo::ServiceAuth );
	t->setParam( 1 , client()->userId().local8Bit() );
	send(t);
	mState = SentAuth;
}

void LoginTask::sendAuthResp(YMSGTransfer* t)
{
	kdDebug(YAHOO_RAW_DEBUG) ;

	QString sn = t->firstParam( 1 );
	QString seed = t->firstParam( 94 );
	m_challengeString = seed;
	QString version_s = t->firstParam( 13 );
	m_sessionID = t->id();
	int version = version_s.toInt();

	switch (version)
	{
		case 0:
		case 1:
		case 2:
			kdDebug(YAHOO_RAW_DEBUG) << "Using version 16 authorization" << endl;
			sendAuthSixteenStage1(sn, seed);
			break;
		default:
			kdDebug(YAHOO_RAW_DEBUG) << "Unknown authentication method used!"
			                         << "Attempting current authentication anyways" << endl;
			sendAuthSixteenStage1(sn, seed);
		break;
	}
	mState = SentAuthResp;

	emit haveSessionID( m_sessionID );
}

void LoginTask::sendAuthSixteenStage1(const QString& sn, const QString& seed)
{
	const QString YahooTokenUrl = "https://login.yahoo.com/config/pwtoken_get?src=ymsgr&ts=&login=%1&passwd=%2&chal=%3";
	kdDebug(YAHOO_RAW_DEBUG) << "seed:" << seed << endl;
	m_stage1Data= QString::null;
	/* construct a URL from the seed and request tokens */
	QByteArray encodedUrl;
	QString fullUrl = YahooTokenUrl.arg(sn, client()->password(), seed);
	KURL tokenUrl(fullUrl);
	KIO::Job* job = KIO::get(tokenUrl, true, false);
	connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
	        this, SLOT(handleAuthSixteenStage1Data(KIO::Job*, const QByteArray&)));
	connect(job, SIGNAL(result(KIO::Job*)),
	        this, SLOT(handleAuthSixteenStage1Result(KIO::Job*)));
}

void LoginTask::handleAuthSixteenStage1Data(KIO::Job* job, const QByteArray& data)
{
	kdDebug(YAHOO_RAW_DEBUG) << "data:" << data << endl;
	m_stage1Data.append(data);
}

void LoginTask::handleAuthSixteenStage1Result(KIO::Job* job)
{
	int responseNumber = -1;
	QString token;
	int error = job->error();
	kdDebug(YAHOO_RAW_DEBUG) << "error:" << error << endl;
	if (error == 0)
	{
		QStringList responses = QStringList::split("\r\n", m_stage1Data);
		responseNumber = responses[0].toInt();
		if (responses.count() >= 3)
		{
			token = responses[1];
			token.remove("ymsgr=");
			kdDebug(YAHOO_RAW_DEBUG) << "response is:" << responseNumber << endl;
			kdDebug(YAHOO_RAW_DEBUG) << "token is:" << token << endl;
		}
		if (responseNumber != 0)
		{
			switch(responseNumber)
			{
			case -1:
				/* error in the received stream */
				emit loginResponse(Yahoo::LoginSock, QString());
				kdDebug(YAHOO_RAW_DEBUG) << "unknown error logging in" << endl;
				break;
			case 1212:
				/* password incorrect */
				emit loginResponse(Yahoo::LoginPasswd, QString());
				kdDebug(YAHOO_RAW_DEBUG) << "password incorrect" << endl;
				break;
			case 1213:
				/* security lock */
				emit loginResponse(Yahoo::LoginLock, QString());
				break;
			case 1235:
				/* username does not exist */
				emit loginResponse(Yahoo::LoginUname, QString());
				kdDebug(YAHOO_RAW_DEBUG) << "user does not exist" << endl;
				break;
			case 1214:
			case 1236:
				emit loginResponse(Yahoo::LoginVerify, QString());
				break;
			case 100: /* username or password missing */
				/*FIXME handle this */
				break;
			default:
				/* FIXME unknown error. handle it! */
				break;
			}
		}
		else
		{
			/* start stage 2 here */
			sendAuthSixteenStage2(token);
		}
	}
}

void LoginTask::sendAuthSixteenStage2(const QString& token)
{
	const QString YahooLoginUrl = "https://login.yahoo.com/config/pwtoken_login?src=ymsgr&ts=&token=%1";
	kdDebug(YAHOO_RAW_DEBUG) << "token:" << token << endl;
	m_stage2Data = QString::null;
	QString fullUrl = YahooLoginUrl.arg(token);
	KURL loginUrl(fullUrl);
	KIO::Job* job = KIO::get(loginUrl, true, false);
	connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
	        this, SLOT(handleAuthSixteenStage2Data(KIO::Job*, const QByteArray&)));
	connect(job, SIGNAL(result(KIO::Job*)),
	        this, SLOT(handleAuthSixteenStage2Result(KIO::Job*)));
}

void LoginTask::handleAuthSixteenStage2Data(KIO::Job*, const QByteArray& data)
{
	kdDebug(YAHOO_RAW_DEBUG) << "data:" << data << endl;
	m_stage2Data.append(data);
}

void LoginTask::handleAuthSixteenStage2Result(KIO::Job* job)
{
	QString crumb;
	int responseNumber = -1;
	int error = job->error();
	kdDebug(YAHOO_RAW_DEBUG) << "error:" << error << endl;
	if (error == 0)
	{
		QStringList responses = QStringList::split("\r\n", m_stage2Data);
		kdDebug(YAHOO_RAW_DEBUG) << responses << endl;
		responseNumber = responses[0].toInt();
		if (responseNumber == 0)
		{
			crumb = responses[1];
			crumb.remove("crumb=");
			m_yCookie = responses[2].remove(0,2); /* remove Y= */
			m_tCookie = responses[3].remove(0,2); /* remove T= */
		}

		if (responseNumber != 0)
		{
			switch(responseNumber)
			{
			case -1:
				emit loginResponse(Yahoo::LoginSock, QString());
				break;
			case 100:
				emit loginResponse(Yahoo::LoginSock, QString());
				break;
			default: /* try to login anyways */
				break;
			}
		}
		else
		{
			QString cryptString = crumb;
			cryptString.append(m_challengeString);
			sendAuthSixteenStage3(cryptString);
		}
	}
}

void LoginTask::sendAuthSixteenStage3(const QString& cryptString)
{
	kdDebug(YAHOO_RAW_DEBUG) << " with crypt string" << cryptString << endl;

	//QByteArray cryptStringHash = QCryptographicHash::hash( cryptString.toAscii(),
	//                                                       QCryptographicHash::Md5 );
	//cryptStringHash = cryptStringHash.toBase64();

	QString cryptStringHash = KMD5( cryptString.ascii() ).base64Digest();

	cryptStringHash = cryptStringHash.replace('+', '.');
	cryptStringHash = cryptStringHash.replace('/', '_');
	cryptStringHash = cryptStringHash.replace('=', '-');

	YMSGTransfer *t = new YMSGTransfer(Yahoo::ServiceAuthResp, m_stateOnConnect);
	t->setId( m_sessionID );
   	t->setParam( 1, client()->userId().local8Bit());
	t->setParam( 0 , client()->userId().local8Bit());
	t->setParam( 277, m_yCookie.local8Bit() );
	t->setParam( 278, m_tCookie.local8Bit() );
	t->setParam( 307, cryptStringHash.local8Bit() );
	t->setParam( 244, 2097087 );
	t->setParam( 2 , client()->userId().local8Bit());
	t->setParam( 2, 1 ); // Both parameter 2s wind up in the packet
	t->setParam( 135, YMSG_PROGRAM_VERSION_STRING );

	send(t);

}

void LoginTask::sendAuthResp_pre_0x0b(const QString &/*sn*/, const QString &/*seed*/)
{
	kdDebug(YAHOO_RAW_DEBUG) ;
}

void LoginTask::handleAuthResp(YMSGTransfer *t)
{
	kdDebug(YAHOO_RAW_DEBUG) ;

	switch( t->service() )
	{
		case( Yahoo::ServiceList ):
			kdDebug(YAHOO_RAW_DEBUG) << "Emitting Signal" << endl;
			emit loginResponse( Yahoo::LoginOk, QString() );
		break;
		case( Yahoo::ServiceAuthResp ):
			kdDebug(YAHOO_RAW_DEBUG) << "Emitting Signal" << endl;
			emit loginResponse( t->firstParam( 66 ).toInt(), t->firstParam( 20 ) );
		break;
		default:
		break;
	}
	mState = InitialState;
}

void LoginTask::setStateOnConnect( Yahoo::Status status )
{
	m_stateOnConnect = status;
}

void LoginTask::parseCookies( YMSGTransfer *t )
{
	kdDebug(YAHOO_RAW_DEBUG) ;

	for( int i = 0; i < t->paramCount( 59 ); ++i)
	{
		QString cookie;
		cookie = t->nthParam( 59, i );
        	if( cookie.startsWith( "Y" ) )
		{
			m_yCookie = getcookie( cookie.latin1() );
			m_loginCookie = getlcookie( cookie.latin1() );
		}
		else if( cookie.startsWith( "T" ) )
		{
			m_tCookie = getcookie( cookie.latin1() );
		}
		else if( cookie.startsWith( "C" ) )
		{
			m_cCookie = getcookie( cookie.latin1() );
		}
    	}
	if( !m_yCookie.isEmpty() && !m_tCookie.isEmpty())
		emit haveCookies();
}

void LoginTask::setVerificationWord( const QString &word )
{
	m_verificationWord = word;
}

const QString& LoginTask::yCookie()
{
	return m_yCookie;
}

const QString& LoginTask::tCookie()
{
	return m_tCookie;
}

const QString& LoginTask::cCookie()
{
	return m_cCookie;
}

const QString& LoginTask::loginCookie()
{
	return m_loginCookie;
}
#include "logintask.moc"
