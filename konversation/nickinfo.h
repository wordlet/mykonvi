/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  nickinfo.h    -  Nick Information
  begin:     Sat Jan 17 2004
  copyright: (C) 2004 by Gary Cramblitt
  email:     garycramblitt@comcast.net
*/

#ifndef NICKINFO_H
#define NICKINFO_H

/*
  @author Gary Cramblitt
*/

#include <qstringlist.h>

class Server;

/*
  The NickInfo object is a data container for information about a single nickname.
  It is owned by the Server object and should NOT be deleted by anything other than Server.
  If using code alters the NickInfo object, it should call Server::nickInfoUpdated to
  let Server know that the object has been modified.
*/

class NickInfo
{
  public:
    NickInfo(Server* server);
    ~NickInfo();
     
    // Get properties of NickInfo object.
    QString getHostmask();
    bool isAway();
    QString getAwayMessage();
    QString getIdentdInfo();
    QString getVersionInfo();
    bool isNotified();
     
    // Return the Server object that owns this NickInfo object.
    Server* getServer();
     
    // Set properties of NickInfo object.
    // If any of these are called, call Server::nickInfoUpdated to let Server know about the change.
    void setHostmask(const QString& newMask);
    void setAway(bool state);
    void setAwayMessage(const QString& newMessage);
    void setIdentdInfo(const QString& newIdentdInfo);
    void setVersionInfo(const QString& newVersionInfo);
    void setNotified(bool state);

  protected:
    Server* owningServer;
    QString hostmask;
    bool away;
    QString awayMessage;
    QString identdInfo;
    QString versionInfo;
    bool notified;
};

#endif
