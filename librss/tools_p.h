/*
 * tools_p.h
 *
 * Copyright (c) 2001, 2002, 2003, 2004 Frerich Raabe <raabe@kde.org>
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. For licensing and distribution details, check the
 * accompanying file 'COPYING'.
 */
#ifndef LIBRSS_TOOLS_P_H
#define LIBRSS_TOOLS_P_H

class QDomNode;
class QString;

namespace RSS
{
	struct Shared
	{
		Shared() : count(1) { }
		void ref() { count++; }
		bool deref() { return !--count; }
		unsigned int count;
	};

	QString extractNode(const QDomNode &parent, const QString &elemName);
}

#endif // LIBRSS_TOOLS_P_H
// vim:noet:ts=4
