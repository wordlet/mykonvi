/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  prefspagelog.cpp  -  Provides a user interface to customize logfile settings
  begin:     Son Okt 27 2002
  copyright: (C) 2002 by Dario Abatianni
  email:     eisfuchs@tigress.com

  $Id$
*/

#include <qlabel.h>
#include <qlayout.h>
#include <qhbox.h>

#include "prefspagelog.h"

PrefsPageLog::PrefsPageLog(QFrame* newParent,Preferences* newPreferences) :
              PrefsPage(newParent,newPreferences)
{
  // Add a Layout to the Log Settings pane
  QGridLayout* logSettingsLayout=new QGridLayout(parentFrame,4,2,marginHint(),spacingHint(),"log_settings_layout");

  useLog=new QCheckBox(i18n("Enable Logging"),parentFrame,"use_log_checkbox");
  lowerLog=new QCheckBox(i18n("Use lower case logs"),parentFrame,"lower_log_checkbox");
  logFollowsNick=new QCheckBox(i18n("Follow nickchanges"),parentFrame,"follow_nickchanges_checkbox");

  QHBox* logPathBox=new QHBox(parentFrame);
/*  QLabel* logPathLabel= */ new QLabel("Logfile Path:",logPathBox);
  logPathInput=new KLineEdit(preferences->getLogPath(),logPathBox,"log_path_input");

  useLog->setChecked(preferences->getLog());
  lowerLog->setChecked(preferences->getLowerLog());
  logFollowsNick->setChecked(preferences->getLogFollowsNick());

  QHBox* logSpacer=new QHBox(parentFrame);

  int row=0;

  logSettingsLayout->addMultiCellWidget(useLog,row,row,0,1);
  row++;

  logSettingsLayout->addWidget(lowerLog,row,0);
  logSettingsLayout->addWidget(logFollowsNick,row,1);
  row++;

  logSettingsLayout->addMultiCellWidget(logPathBox,row,row,0,1);
  row++;

  logSettingsLayout->addMultiCellWidget(logSpacer,row,row,0,1);
  logSettingsLayout->setRowStretch(row,10);

  // Set up signals / slots for Log Setup page
  connect(useLog,SIGNAL (stateChanged(int)),this,SLOT (useLogChanged(int)) );
  connect(lowerLog,SIGNAL (stateChanged(int)),this,SLOT (lowerLogChanged(int)) );
  connect(logFollowsNick,SIGNAL (stateChanged(int)),this,SLOT (logFollowsNickChanged(int)) );
  connect(logPathInput,SIGNAL (textChanged(const QString&)),this,SLOT (logPathInputChanged(const QString&)) );

  useLogChanged(useLog->state());
}

PrefsPageLog::~PrefsPageLog()
{
}

void PrefsPageLog::useLogChanged(int state)
{
  preferences->setLog(state==2);

  lowerLog->setEnabled(state==2);
  logFollowsNick->setEnabled(state==2);
  logPathInput->setEnabled(state==2);
}

void PrefsPageLog::lowerLogChanged(int state)
{
  preferences->setLowerLog(state==2);
}

void PrefsPageLog::logFollowsNickChanged(int state)
{
  preferences->setLogFollowsNick(state==2);
}

void PrefsPageLog::logPathInputChanged(const QString& path)
{
  preferences->setLogPath(path);
}
