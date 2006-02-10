/* ============================================================
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2005-06-02
 * Description :
 *
 * Copyright 2005 by Renchi Raju
 * Copyright (C) 2006 Peter Simonsson <psn@linux.se>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "searchbar.h"

#include <qcheckbox.h>
#include <qtimer.h>
#include <qpalette.h>
#include <qaccel.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qobjectlist.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include <qwidgetstack.h>

#include <kdebug.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <klocale.h>

#define SEARCH_FORWARD_MENU 1
#define MATCH_CASE_MENU 2

SearchBar::SearchBar(QWidget* parent)
: SearchBarBase(parent)
{
    m_searchFoward = false;
    m_matchCase = false;

    setFocusProxy(m_searchEdit);
    KIconLoader* iconLoader = kapp->iconLoader();
    m_closeButton->setIconSet(iconLoader->loadIconSet("fileclose", KIcon::Toolbar, 16));
    m_findNextButton->setIconSet(iconLoader->loadIconSet("next", KIcon::Toolbar, 16));

    m_timer = new QTimer(this);

    QAccel* accel = new QAccel(this);
    accel->connectItem( accel->insertItem(Qt::Key_Escape), this, SLOT(hide()));

    connect(m_timer, SIGNAL(timeout()), SLOT(slotFind()));
    connect(m_searchEdit, SIGNAL(textChanged(const QString&)), SLOT(slotTextChanged()));
    connect(m_searchEdit, SIGNAL(returnPressed()), SLOT(slotFindNext()));
    connect(m_findNextButton, SIGNAL(clicked()), SLOT(slotFindNext()));
    connect(m_closeButton, SIGNAL(clicked()), SLOT(hide()));
    connect(m_optionsButton, SIGNAL(clicked()), this, SLOT(showOptionsMenu()));

    m_optionsMenu = new QPopupMenu(m_optionsButton, "options_menu");
    m_optionsMenu->setCheckable(true);
    m_optionsMenu->insertItem(i18n("Search Forward"), this, SLOT(toggleSearchFoward()), 0, SEARCH_FORWARD_MENU);
    m_optionsMenu->insertItem(i18n("Match Case"), this, SLOT(toggleMatchCase()), 0, MATCH_CASE_MENU);

    m_optionsButton->setPopup(m_optionsMenu);
}

SearchBar::~SearchBar()
{
}

void SearchBar::showEvent(QShowEvent *e)
{
    SearchBarBase::showEvent(e);
    m_searchEdit->selectAll();
}

bool SearchBar::focusedChild()
{
    QObjectList *l = queryList("QWidget", 0,0, true);
    QObjectListIt it( *l ); 
    QObject *obj;
    bool has=false;

    while ((obj = it.current()) != 0)
    {
        ++it;
        if (((QWidget*)obj)->hasFocus())
        {
            has=true;
            break;
        }
    }
    delete l;
    return has;
}

void SearchBar::hide()
{
    m_timer->stop();
    SearchBarBase::hide();

    if (focusedChild())
        emit hidden();
}

void SearchBar::slotTextChanged()
{
    m_timer->start(50, true);
}

void SearchBar::slotFind()
{
    if (m_searchEdit->text().isEmpty())
    {
        m_searchEdit->unsetPalette();
        m_findNextButton->setEnabled(false);
        setStatus(QPixmap(), "");
        return;
    }

    emit signalSearchChanged(m_searchEdit->text());
}

void SearchBar::slotFindNext()
{
    if (m_searchEdit->text().isEmpty())
    {
        m_searchEdit->unsetPalette();
        m_findNextButton->setEnabled(false);
        setStatus(QPixmap(), "");
        return;
    }

    emit signalSearchNext();
}

void SearchBar::setHasMatch(bool value)
{
    QPalette pal = m_searchEdit->palette();
    pal.setColor(QPalette::Active, QColorGroup::Base, value ? Qt::green : Qt::red);
    m_searchEdit->setPalette(pal);
    m_findNextButton->setEnabled(value);
}

void SearchBar::setStatus(const QPixmap& pix, const QString& text)
{
    if(!text.isEmpty()) {
        m_statusStack->raiseWidget(1);
    } else {
        m_statusStack->raiseWidget(0);
    }

    m_statusPixLabel->setPixmap(pix);
    m_statusTextLabel->setText(text);
}

QString SearchBar::pattern() const
{
    return m_searchEdit->text();
}

bool SearchBar::searchForward() const
{
    return m_searchFoward;
}

bool SearchBar::caseSensitive() const
{
    return m_matchCase;
}

void SearchBar::toggleSearchFoward()
{
    m_searchFoward = !m_searchFoward;
    m_optionsMenu->setItemChecked(SEARCH_FORWARD_MENU, m_searchFoward);
    slotTextChanged();
}

void SearchBar::toggleMatchCase()
{
    m_matchCase = !m_matchCase;
    m_optionsMenu->setItemChecked(MATCH_CASE_MENU, m_matchCase);
    slotTextChanged();
}

void SearchBar::showOptionsMenu()
{
  m_optionsButton->openPopup();
}

#include "searchbar.moc"
