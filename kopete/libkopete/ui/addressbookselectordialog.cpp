/*
    AddressBookSelectorDialog
    Nice Dialog to select a KDE AddressBook contact

    Copyright (c) 2005 by Duncan Mac-Vicar Prett <duncan@kde.org>

    Kopete    (c) 2002-2005 by the Kopete developers  <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/

#include "addressbookselectordialog.h"
#include "addressbookselectorwidget.h"
#include <kdialogbase.h>
#include <qdialog.h>
#include <qlistview.h>
#include <qvbox.h>
#include <klocale.h>
#include <kdialog.h>

namespace Kopete
{
namespace UI
{

AddressBookSelectorDialog::AddressBookSelectorDialog(const QString &title, const QString &message, const QString &preSelectUid, QWidget *parent, const char *name, bool modal ) : KDialogBase(parent, name, modal, title, Help|Ok|Cancel, Ok, true )
{
	QVBox *vbox=new QVBox(this);
	m_addressBookSelectorWidget= new AddressBookSelectorWidget(vbox);
	m_addressBookSelectorWidget->setLabelMessage(message);	

	vbox->setSpacing( KDialog::spacingHint() );
	
	setMainWidget(vbox);
	enableButtonOK(false);
	//setHelp("linkaddressbook");

	connect(m_addressBookSelectorWidget, SIGNAL(addresseeListClicked( QListViewItem * )), SLOT(slotWidgetAddresseeListClicked( QListViewItem * )));

	if ( !preSelectUid.isEmpty() )
		m_addressBookSelectorWidget->selectAddressee(preSelectUid);
}

AddressBookSelectorDialog::~AddressBookSelectorDialog()
{
}

KABC::Addressee AddressBookSelectorDialog::getAddressee( const QString &title, const QString &message, const QString &preSelectUid, QWidget *parent)
{
	AddressBookSelectorDialog dialog(title, message, preSelectUid, parent);
	int result = dialog.exec();

	KABC::Addressee adr;
	if ( result == QDialog::Accepted )
		adr = dialog.addressBookSelectorWidget()->addressee();

	return adr;
}

void AddressBookSelectorDialog::slotWidgetAddresseeListClicked( QListViewItem *addressee )
{
	// enable ok if a valid addressee is selected
	enableButtonOK( addressee ? addressee->isSelected() : false);
}

void AddressBookSelectorDialog::accept()
{
	QDialog::accept();
}

void AddressBookSelectorDialog::reject()
{
	QDialog::reject();
}

} // namespace UI
} // namespace Kopete

#include "addressbookselectordialog.moc"

// vim: set noet ts=4 sts=4 sw=4:

