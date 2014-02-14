/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  Copyright (C) 2002 Dario Abatianni <eisfuchs@tigress.com>
  Copyright (C) 2014 Travis McHenry <me@travisjmchenry.com>
*/

#ifndef NICKSONLINEMODEL_H
#define NICKSONLINEMODEL_H

#include <QSortFilterProxyModel>
#include <QStringList>

#include <KToolBar>

#include "chatwindow.h"
#include "nicklistmodel.h"
#include "connectionmanager.h"
#include "servergroupmodel.h"
#include "servergroupsettings.h"

#include "ui_nicksonlinepanelui.h"

            //nick, patterns
typedef QHash<QString, Nick2*> WatchedNicks;
            //monitor type, watched nick
typedef QHash<int, WatchedNicks> WatchedNickList;
            //connection id, watched nick list
typedef QHash<int, WatchedNickList> WatchedNickConnections;
            //server group id, watched nick connections
typedef QHash<int, WatchedNickConnections> WatchedNickListHash;

class ConnectionManager;
class NickListModel;

class NicksOnlineFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
        explicit NicksOnlineFilterModel(QObject* parent = 0);
        ~NicksOnlineFilterModel();

        QVariant data(const QModelIndex& index, int role) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        bool isWatchedNickOnline(int cId, const QString& nick) const;
        Nick2* getWatchedNick(int cId, const QString& nick) const;

        void setNickHostmask(int sgId, int cId, const QString& nick, const QString& hostmask);
        void setNickIdentified(int sgId, int cId, const QString& nick, bool identified);
        void setNickRealName(int sgId, int cId, const QString& nick, const QString& realName);
        void setNickSecureConnection(int sgId, int cId, const QString& nick, bool secure);
        void setNickOnlineSince(int sgId, int cId, const QString& nick, const QDateTime& onlineSince);
        void setNickNetServer(int sgId, int cId, const QString& nick, const QString& netServer);
        void setNickNetServerInfo(int sgId, int cId, const QString& nick, const QString& netServerInfo);
        void setNickMode(int sgId, int cId, const QString& channel, const QString& nick, char mode, bool state);
        void setNickAway(int sgId, int cId, const QString& nick, bool away, const QString& awayMessage = QString());

    signals:
        void requestWhois(int cId, const QString& nick);

    public slots:
        void removeNotifyNick(int sgId, const QString& nick);
        void addNotifyNick(int sgId, const QString& nick);
        void updateNotifyConnection(int sgId, int cId);

        void notifyResponse(int cId, const QString& newIson);
        void nickOnline(int sgId, int cId, const QString& nick);
        void nickOffline(int sgId, int cId, Nick2* nick);

        void slotRequestWhois(int cId, const QString& nick);
        void whoisReceived(int cId, const QString& nick);

    protected:
        bool lessThan(const QModelIndex& left, const QModelIndex& right) const;
        bool filterAcceptsColumn(int column, const QModelIndex& parent) const;
        bool filterAcceptsRow(int row, const QModelIndex& parent) const;
        int columnCount(const QModelIndex& parent = QModelIndex()) const;

        void removeNotifyNick(int sgId, int cId, const QString& nick);
        void addNotifyNick(int sgId, int cId, Nick2* nick);
        void replaceNotifyNick(int sgId, int cId, Nick2* nick);
        bool isWatchTypeEmpty(int type) const;
        bool isNickWatched(int sgId, int cId, const QString& nick) const;
        bool isWatchedNickOnline(int sgId, int cId, const QString& nick) const;
        Nick2* getWatchedNick(int sgId, int cId, const QString& nick) const;

        QModelIndex getNotifyNickIndex(int sgId, const QString& nick) const;

    protected slots:
        void isonCheck();

    private:
        void updateMinimumRowHeight();

        int m_minimumRowHeight;

        QHash<int, QStringList> m_isonList;

        QMultiHash<int, QString> m_whoisRequested;

        QIcon m_onlineIcon;
        QIcon m_offlineIcon;
        QString m_whatsThis;
        bool m_hostmask;
        int m_column;
        NickListModel* m_nickListModel;
        ConnectionManager* m_connectionManager;

        WatchedNickListHash m_watchedNicks;
        QTimer* m_timer;
};

class NicksOnline : public ChatWindow, private Ui::NicksOnlineWidgetUI
{
    Q_OBJECT

    public:
        explicit NicksOnline(QWidget* parent = 0);
        ~NicksOnline();

        virtual bool canBeFrontView() { return true; }

        bool isInsertSupported() { return true; }
        QString getTextInLine();

    signals:
        void doubleClicked(int sgId, const QString& nick);
        void showView(ChatWindow* view);

    public slots:
        virtual void appendInputText(const QString&, bool fromCursor);

    protected:
        /** Called from ChatWindow adjustFocus */
        virtual void childAdjustFocus(){}

    protected slots:
        //Used to disable functions when not connected
        virtual void serverOnline(bool online);

        void addNickname();
        void slotAddNickname(int cId, const QString& nick);
        void removeNickname();
        void createContact();
        void editContact();
        void changeAssociation();
        void openQuery();

        void doubleClicked(const QModelIndex& index);
        void currentChanged(const QModelIndex& current, const QModelIndex& previous);
        void collapsed(const QModelIndex& index);
        void expanded(const QModelIndex& index);
        void contextMenu(const QPoint& pos);

    private:
        NicksOnlineFilterModel* m_nicksOnlineModel;

        KToolBar* m_toolBar;
        QAction* m_addNickname;
        QAction* m_removeNickname;
        QAction* m_newContact;
        QAction* m_editContact;
        QAction* m_changeAssociation;
        QAction* m_openQuery;

};

#endif