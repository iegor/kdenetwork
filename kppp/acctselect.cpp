//---------------------------------------------------------------------------
//
//             kPPP: A pppd front end for the KDE project
//
//---------------------------------------------------------------------------
//
// (c) 1997-1998 Bernd Johannes Wuebben <wuebben@kde.org>
// (c) 1997-1999 Mario Weilguni <mweilguni@kde.org>
// (c) 1998-1999 Harri Porten <porten@kde.org>
//
// derived from Jay Painters "ezppp"
//
//---------------------------------------------------------------------------
//
//  $Id: acctselect.cpp 352686 2004-10-07 14:25:22Z porten $
//
//---------------------------------------------------------------------------
//
//  This program is free software; you can redistribute it and-or
//  modify it under the terms of the GNU Library General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Library General Public License for more details.
//
//  You should have received a copy of the GNU Library General Public
//  License along with this program; if not, write to the Free
//  Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//---------------------------------------------------------------------------


#include <qcombobox.h>
#include <qlabel.h>
#include <kurllabel.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qdir.h>
#include <qregexp.h>
#include <qwmatrix.h>
#include <qcheckbox.h>
#include <kdialog.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kiconloader.h>
#include <krun.h>

#include "acctselect.h"
#include "pppdata.h"


AccountingSelector::AccountingSelector(QWidget *parent, bool _isnewaccount, const char *name)
  :  QWidget(parent, name),
    isnewaccount(_isnewaccount)
{
  QVBoxLayout *l1 = new QVBoxLayout(parent, 0, KDialog::spacingHint());

  enable_accounting = new QCheckBox(i18n("&Enable accounting"), parent);
  l1->addWidget(enable_accounting, 1);
  connect(enable_accounting, SIGNAL(toggled(bool)), this, SLOT(enableItems(bool)));

  // insert the tree widget
  tl = new QListView(parent, "treewidget");

  connect(tl, SIGNAL(selectionChanged(QListViewItem*)), this,
          SLOT(slotSelectionChanged(QListViewItem*)));
  tl->setMinimumSize(220, 200);
  l1->addWidget(tl, 1);

  KURLLabel *up = new KURLLabel(parent);
  up->setText(i18n("Check for rule updates"));
  up->setURL("http://developer.kde.org/~kppp/rules.html");
  connect(up, SIGNAL(leftClickedURL(const QString&)), SLOT(openURL(const QString&)));

  l1->addWidget(up, 1);

  // label to display the currently selected ruleset
  QHBoxLayout *l11 = new QHBoxLayout;
  l1->addSpacing(10);
  l1->addLayout(l11);
  QLabel *lsel = new QLabel(i18n("Selected:"), parent);
  selected = new QLabel(parent);
  selected->setFrameStyle(QFrame::Sunken | QFrame::WinPanel);
  selected->setLineWidth(1);
  selected->setFixedHeight(selected->sizeHint().height() + 16);
  l11->addWidget(lsel, 0);
  l11->addSpacing(10);
  l11->addWidget(selected, 1);

  // volume accounting
  l1->addStretch(1);
  QHBoxLayout *l12 = new QHBoxLayout;
  l1->addLayout(l12);
  QLabel *usevol_l = new QLabel(i18n("Volume accounting:"), parent);
  use_vol = new QComboBox(parent);
  use_vol->insertItem(i18n("No Accounting"), 0);
  use_vol->insertItem(i18n("Bytes In"), 1);
  use_vol->insertItem(i18n("Bytes Out"), 2);
  use_vol->insertItem(i18n("Bytes In & Out"), 3);
  use_vol->setCurrentItem(gpppdata.VolAcctEnabled());
  l12->addWidget(usevol_l);
  l12->addWidget(use_vol);

  // load the pmfolder pixmap from KDEdir
  pmfolder = UserIcon("folder");

  // scale the pixmap
  if(pmfolder.width() > 0) {
    QWMatrix wm;
    wm.scale(16.0/pmfolder.width(), 16.0/pmfolder.width());
    pmfolder = pmfolder.xForm(wm);
  }

  // load the pmfolder pixmap from KDEdir
  pmfile = UserIcon("phone");

  // scale the pixmap
  if(pmfile.width() > 0) {
    QWMatrix wm;
    wm.scale(16.0/pmfile.width(), 16.0/pmfile.width());
    pmfile = pmfile.xForm(wm);
  }

  enable_accounting->setChecked(gpppdata.AcctEnabled());

  setupTreeWidget();

  l1->activate();
}


QString AccountingSelector::fileNameToName(QString s) {

  s.replace('_', " ");
  return KURL::decode_string(s);

}


QString AccountingSelector::nameToFileName(QString s) {

  s.replace(' ', "_");
  return s;

}

QListViewItem *AccountingSelector::findByName(QString name)
{
  QListViewItem *ch = tl->firstChild();
  while(ch) {
    if(ch->text(0) == name)
      return ch;
    ch = ch->nextSibling();
  }
  return 0;
}


void AccountingSelector::insertDir(QDir d, QListViewItem *root) {

  QListViewItem* tli = 0;

  // sanity check
  if(!d.exists() || !d.isReadable())
    return;


  // set up filter
  d.setNameFilter("*.rst");
  d.setFilter(QDir::Files);
  d.setSorting(QDir::Name);

  // read the list of files
  const QFileInfoList *list = d.entryInfoList();
  QFileInfoListIterator it( *list );
  QFileInfo *fi;

  // traverse the list and insert into the widget
  while((fi = it.current())) {
    ++it;

    QString samename = fi->fileName();

    QListViewItem *i = findByName(samename);

    // skip this file if already in tree
    if(i)
      continue;

    // check if this is the file we should mark
    QString name = fileNameToName(fi->baseName(true));
    if(root)
      tli = new QListViewItem(root, name);
    else
      tli = new QListViewItem(tl, name);

    tli->setPixmap(0, pmfile);

    // check if this is the item we are searching for
    // (only in "Edit"-mode, not in "New"-mode
    if(!isnewaccount && !edit_s.isEmpty() &&
       (edit_s == QString(fi->filePath()).right(edit_s.length()))) {
      edit_item = tli;
    }
  }

  // set up a filter for the directories
  d.setFilter(QDir::Dirs);
  d.setNameFilter("*");
  const QFileInfoList *dlist = d.entryInfoList();
  QFileInfoListIterator dit(*dlist);

  while((fi = dit.current())) {
    // skip "." and ".." directories
    if(fi->fileName().left(1) != ".") {
      // convert to human-readable name
      QString name = fileNameToName(fi->fileName());

      // if the tree already has an item with this name,
      // skip creation and use this one, otherwise
      // create a new entry
      QListViewItem *i = findByName(name);
      if(!i) {
        QListViewItem* item;

        if(root)
          item = new QListViewItem(root, name);
        else
          item = new QListViewItem(tl, name);

        item->setPixmap(0, pmfolder);

	insertDir(QDir(fi->filePath()), item);
      } else
	insertDir(QDir(fi->filePath()), i);
    }
    ++dit;
  }
}


void AccountingSelector::setupTreeWidget() {
  // search the item
  edit_item = 0;
  if(!isnewaccount) {
    edit_s = gpppdata.accountingFile();
  }
  else
    edit_s = "";

  tl->addColumn( i18n("Available Rules") );
  tl->setColumnWidth(0, 205);
  tl->setRootIsDecorated(true);

  // look in ~/.kde/share/apps/kppp/Rules and $KDEDIR/share/apps/kppp/Rules
  QStringList dirs = KGlobal::dirs()->resourceDirs("appdata");
  for (QStringList::ConstIterator it = dirs.begin();
       it != dirs.end(); it++) {
    insertDir(QDir((*it) + "Rules"), 0);
  }

  // when mode is "Edit", then hightlight the
  // appropriate item
  if(!isnewaccount && edit_item) {
    tl->setSelected(edit_item, true);
    tl->setOpen(edit_item->parent(), true);
    tl->ensureItemVisible(edit_item);
  }

  enableItems(enable_accounting->isChecked());
}


void AccountingSelector::enableItems(bool enabled) {

  tl->setEnabled(enabled);

  if(!enabled || (!tl->currentItem()))
    selected->setText(i18n("(none)"));
  else {
    QListViewItem* i = tl->currentItem();
    QString s;
    while(i) {
      s = "/" + i->text(0) + s;
      i = i->parent();
    }
    selected->setText(s.mid(1));

    s += ".rst";
    edit_s = nameToFileName(s);
  }
}


void AccountingSelector::slotSelectionChanged(QListViewItem* i) {

  if(!i || i->childCount())
    return;

  if(!enable_accounting->isChecked())
    return;

  enableItems(true);
}


bool AccountingSelector::save() {

  if(enable_accounting->isChecked()) {
    gpppdata.setAccountingFile(edit_s);
    gpppdata.setAcctEnabled(true);
  } else {
    gpppdata.setAccountingFile("");
    gpppdata.setAcctEnabled(false);
  }

  gpppdata.setVolAcctEnabled(use_vol->currentItem());

  return true;
}

void AccountingSelector::openURL(const QString &url) {
  new KRun( KURL( url ) );
}

#include "acctselect.moc"

