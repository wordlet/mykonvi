/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  Copyright (C) 1999 Martin R. Jones <mjones@kde.org>
  Copyright (C) 2008 Eike Hein <hein@kde.org>
  Copyright (C) 2010 Martin Blumenstingl <darklight.xdarklight@googlemail.com>
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

    identitiesOnAutoAwayChanged();
}

void AbstractAwayManager::identityOnline(int identityId)
{
    IdentityPtr identity = Preferences::identityById(identityId);

    if (identity && identity->getAutomaticAway() &&
        !m_identitiesOnAutoAway.contains(identityId))
    {
        m_identitiesOnAutoAway.append(identityId);

        identitiesOnAutoAwayChanged();
    }
}

void AbstractAwayManager::identityOffline(int identityId)
{
    if (m_identitiesOnAutoAway.removeOne(identityId))
        identitiesOnAutoAwayChanged();
}

void AbstractAwayManager::implementManagedAway(int identityId)
{
    const QList<Server*> serverList = m_connectionManager->getServerList();

    foreach (Server* server, serverList)
    {
        if (server->getIdentity()->id() == identityId && server->isConnected() && !server->isAway())
            server->requestAway();
    }
}

void AbstractAwayManager::setManagedIdentitiesAway()
{
    foreach (int identityId, m_identitiesOnAutoAway)
        implementManagedAway(identityId);
}

void AbstractAwayManager::implementManagedUnaway(const QList<int>& identityList)
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
    // Set the "not away" status for all identities which have
    // auto-away enabled.
    implementManagedUnaway(m_identitiesOnAutoAway);
}

void AbstractAwayManager::requestAllAway(const QString& reason)
{
    const QList<Server*> serverList = m_connectionManager->getServerList();

    foreach (Server* server, serverList)
        if (server->isConnected())
            server->requestAway(reason);
}

void AbstractAwayManager::requestAllUnaway()
{
    const QList<Server*> serverList = m_connectionManager->getServerList();

    foreach (Server* server, serverList)
        if (server->isConnected() && server->isAway())
            server->requestUnaway();
}

void AbstractAwayManager::toggleGlobalAway(bool away)
{
    if (away)
        requestAllAway();
    else
        requestAllUnaway();
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
    // NOTE: This statement is only true for the old implementation
    // (which does not use KIdleTime).
    // Regardless of any implementation: If the given parameter indicates
    // that the user is not away we should simulate user activity to
    // ensure that the away-status of the user is really reset.
    if (!away)
        simulateUserActivity();

    Application* konvApp = static_cast<Application*>(kapp);
    KToggleAction* awayAction = qobject_cast<KToggleAction*>(konvApp->getMainWindow()->actionCollection()->action("toggle_away"));

    if (!awayAction)
        return;

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
