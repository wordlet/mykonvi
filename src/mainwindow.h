/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  Copyright (C) 2002 Dario Abatianni <eisfuchs@tigress.com>
  Copyright (C) 2005 Ismail Donmez <ismail@kde.org>
  Copyright (C) 2005 Peter Simonsson <psn@linux.se>
  Copyright (C) 2005 John Tapsell <johnflux@gmail.com>
  Copyright (C) 2005-2008 Eike Hein <hein@kde.org>
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "common.h"
#include "preferences.h"
#include "ssllabel.h"

#include <QStringList>

#include <kxmlguiwindow.h>
#include <kaction.h>


class KToggleAction;

class Server;
class KonviSettingsDialog;
class ViewContainer;

namespace Konversation
{
    class ServerListDialog;
    class TrayIcon;
    class StatusBar;
}

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT

    public:
        MainWindow();
        ~MainWindow();

        ViewContainer* getViewContainer() { return m_viewContainer; }
        Konversation::TrayIcon* systemTrayIcon() const { return m_trayIcon; }

        /** Some errors need to be shown, even when konversation is minimized.
         *  For example, when a kimiface call is received to query a person,
         *  (e.g. the user choses "Chat with X" in kmail) but that person isn't
         *  recognised, we need to give immediate feedback to the user.
         */
        void focusAndShowErrorMessage(const QString &errorMsg);

    signals:
        void showQuickConnectDialog();
        void nicksNowOnline(Server*);
        void endNotification();
        void serverStateChanged(Server* server, Konversation::ConnectionState state);
        void triggerRememberLine();
        void triggerRememberLines(Server*);
        void cancelRememberLine();
        void insertMarkerLine();

    public slots:
        void quitProgram();

        void updateTrayIcon();

        void openServerList();

        void openIdentitiesDialog();
        IdentityPtr editIdentity(IdentityPtr identity);

        void setOnlineList(Server* notifyServer,const QStringList& list, bool changed);

    protected slots:
        /** This is connected to the preferences settingsChanged signal and acts to compress
        *  multiple successively settingsChanged() signals into a single output
        *  appearanceChanged() signal.
        *
        *  Do not connect to the settingsChanged signal elsewhere.  If you want to know when
        *  the settings have changed, connect to:
        *  KonversationApplication::instance(), SIGNAL(appearanceChanged())
        */
        void settingsChangedSlot();

        /** This is connected to the appearanceChanged signal.
        *  @see settingsChangedSlot()
        */
        void resetHasDirtySettings();

        void toggleMenubar(bool dontShowWarning = false);

        void openPrefsDialog();
        void openKeyBindings();
        void openQuickConnectDialog();

        // it seems that moc does not honor #ifs in compile so we create an
        // empty slot in our .cpp file rather than #if this slot out
        void openNotifications();
        void notifyAction(int connectionId,const QString& nick);

        void toggleVisibility();

        void showEvent(QShowEvent* e);
        void hideEvent(QHideEvent* e);
        void leaveEvent(QEvent* e);


    protected:
        virtual QSize sizeHint() const;

        int confirmQuit();
        bool queryClose();
        virtual bool event(QEvent* e);

        ViewContainer* m_viewContainer;
        Konversation::StatusBar* m_statusBar;
        Konversation::TrayIcon* m_trayIcon;

        KToggleAction* m_showMenuBarAction;

        KonviSettingsDialog *m_settingsDialog;
        Konversation::ServerListDialog* m_serverListDialog;

        /** @see settingsChangedSlot() */
        bool m_hasDirtySettings;
        bool m_closeApp;
};

#endif /* MAINWINDOW_H */
