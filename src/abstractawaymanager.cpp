/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
  Copyright (C) 2008 Eike Hein <hein@kde.org>
  Copyright (c) 2010 Martin Blumenstingl <darklight.xdarklight@googlemail.com>
*/

#include "abstractawaymanager.h"
#include "application.h"
#include "connectionmanager.h"
#include "server.h"
#include "preferences.h"

#include <KActionCollection>
#include <KToggleAction>

AbstractAwayManager::AbstractAwayManager(QObject* parent) : QObject(parent)
{
    m_connectionManager = static_cast<Application*>(kapp)->getConnectionManager();
    
    connect(this, SIGNAL(toggleAway(QList<int>, bool)), this, SLOT(toggleIdentitiesAwayStatus(QList<int>, bool)));
}

void AbstractAwayManager::identitiesChanged()
{
    QList<int> newIdentityList;

    const QList<Server*> serverList = m_connectionManager->getServerList();

    foreach (Server* server, serverList)
    {
        IdentityPtr identity = server->getIdentity();

        if (identity && identity->getAutomaticAway() && server->isConnected())
            newIdentityList.append(identity->id());
    }

    m_identitiesOnAutoAway = newIdentityList;

    emit identitiesOnAutoAwayChanged();
}

void AbstractAwayManager::identityOnline(int identityId)
{
    IdentityPtr identity = Preferences::identityById(identityId);

    if (identity && identity->getAutomaticAway() &&
        !m_identitiesOnAutoAway.contains(identityId))
    {
        m_identitiesOnAutoAway.append(identityId);

        emit identitiesOnAutoAwayChanged();
    }
}

void AbstractAwayManager::identityOffline(int identityId)
{
    if (m_identitiesOnAutoAway.removeOne(identityId))
    {
        emit identitiesOnAutoAwayChanged();
    }
}

void AbstractAwayManager::setIdentitiesAway(QList<int> identityList)
{
    const QList<Server*> serverList = m_connectionManager->getServerList();
    
    foreach (Server* server, serverList)
    {
        if (identityList.contains(server->getIdentity()->id()) && server->isConnected() && !server->isAway())
            server->requestAway();
    }
}

void AbstractAwayManager::setManagedIdentitiesAway()
{
    setIdentitiesAway(m_identitiesOnAutoAway);
}

void AbstractAwayManager::setIdentitiesUnaway(QList<int> identityList)
{
    const QList<Server*> serverList = m_connectionManager->getServerList();
    
    foreach (Server* server, serverList)
    {
        IdentityPtr identity = server->getIdentity();
        
        if (identityList.contains(identity->id()) && identity->getAutomaticUnaway()
            && server->isConnected() && server->isAway())
        {
            server->requestUnaway();
        }
    }
}

void AbstractAwayManager::setManagedIdentitiesUnaway()
{
    // set the "not away" status for all identities which have
    // auto-away enabled
    setIdentitiesUnaway(m_identitiesOnAutoAway);
}

void AbstractAwayManager::requestAllAway(const QString& reason)
{
    const QList<Server*> serverList = m_connectionManager->getServerList();

    foreach (Server* server, serverList)
        if (server->isConnected()) server->requestAway(reason);
}

void AbstractAwayManager::requestAllUnaway()
{
    const QList<Server*> serverList = m_connectionManager->getServerList();

    foreach (Server* server, serverList)
        if (server->isConnected() && server->isAway()) server->requestUnaway();
}

void AbstractAwayManager::toggleGlobalAway(bool away)
{
    if (away)
        requestAllAway();
    else
        requestAllUnaway();
}

void AbstractAwayManager::toggleIdentitiesAwayStatus(QList<int> identityList, bool away)
{
    if (away)
    {
        setIdentitiesAway(identityList);
    }
    else
    {
        setIdentitiesUnaway(identityList);
    }
}

void AbstractAwayManager::updateGlobalAwayAction(bool away)
{
    // FIXME: For now, our only triggers for resetting the idle time
    // are mouse movement and the screensaver getting disabled. This
    // means that typing '/unaway' or '/back' does not reset the idle
    // time and won't prevent AbstractAwayManager from setting a connection
    // away again shortly after when its identity's maximum auto-away
    // idle time, counted from the last mouse movement or screensaver
    // deactivation rather than the actual last user activity (the key
    // presses), has been exceeded. We work around this here by reset-
    // ting the idle time whenever any connection changes its state to
    // unaway in response to the server until we find a better solu-
    // tion (i.e. a reliable way to let keyboard activity in the sys-
    // tem reset the idle time).
    if (!away)
    {
        resetIdle();
    }

    Application* konvApp = static_cast<Application*>(kapp);
    KToggleAction* awayAction = qobject_cast<KToggleAction*>(konvApp->getMainWindow()->actionCollection()->action("toggle_away"));

    if (!awayAction) return;

    if (away)
    {
        const QList<Server*> serverList = m_connectionManager->getServerList();
        int awayCount = 0;

        foreach (Server* server, serverList)
        {
            if (server->isAway())
                awayCount++;
        }

        if (awayCount == serverList.count())
        {
            awayAction->setChecked(true);
            awayAction->setIcon(KIcon("im-user-away"));
        }
    }
    else
    {
        awayAction->setChecked(false);
        awayAction->setIcon(KIcon("im-user"));
    }
}

#include "abstractawaymanager.moc"
