/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  copyright: (C) 2004, 2009 by Peter Simonsson
  email:     peter.simonsson@gmail.com
  Copyright (C) 2012 Eike Hein <hein@kde.org>
*/
#include "identitydialog.h"
#include "application.h"
#include "awaymanager.h"
#include "irccharsets.h"

#include <KEditListWidget>
#include <KDialog>
#include <KMessageBox>
#include <KMessageWidget>
#include <KInputDialog>
#include <KUser>

namespace Konversation
{

    IdentityDialog::IdentityDialog(QWidget *parent)
        : KDialog(parent)
    {
        setCaption( i18n("Identities") );
        setButtons( Ok|Cancel );
        setDefaultButton( Ok );

        // Initialize the dialog widget
        QWidget* w = new QWidget(this);
        setupUi(w);
        setMainWidget(w);

        QGroupBox* nickGroupBox = new QGroupBox(i18n("Nickname"));
        verticalLayout->insertWidget(1, nickGroupBox);
        QVBoxLayout* nickGroupBoxLayout = new QVBoxLayout(nickGroupBox);
        nickGroupBoxLayout->setContentsMargins(0, 0, 0, 0);
        m_nicknameLBox = new KEditListWidget(nickGroupBox);
        nickGroupBoxLayout->addWidget(m_nicknameLBox);

        m_nicknameLBox->setWhatsThis(i18n("This is your list of nicknames. A nickname is the name that other users will "
                                          "know you by. You may use any name you desire. The first character must be a letter.\n\n"
                                          "Since nicknames must be unique across an entire IRC network, your desired name may be "
                                          "rejected by the server because someone else is already using that nickname. Enter "
                                          "alternate nicknames for yourself. If your first choice is rejected by the server, "
                                          "Konversation will try the alternate nicknames."));

        newBtn->setIcon(KIcon("list-add"));
        connect(newBtn, SIGNAL(clicked()), this, SLOT(newIdentity()));

        copyBtn->setIcon(KIcon("edit-copy"));
        connect(copyBtn, SIGNAL(clicked()), this, SLOT(copyIdentity()));

        m_editBtn->setIcon(KIcon("edit-rename"));
        connect(m_editBtn, SIGNAL(clicked()), this, SLOT(renameIdentity()));

        m_delBtn->setIcon(KIcon("edit-delete"));
        connect(m_delBtn, SIGNAL(clicked()), this, SLOT(deleteIdentity()));

        foreach(const IdentityPtr &id, Preferences::identityList()) {
            m_identityCBox->addItem(id->getName());
            m_identityList.append( IdentityPtr( id ) );
        }

        m_additionalAuthInfo = new KMessageWidget(generalWidget);
        m_additionalAuthInfo->setWordWrap(true);
        m_additionalAuthInfo->setCloseButtonVisible(false);
        m_additionalAuthInfo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

        connect(m_authTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(authTypeChanged(int)));
        m_authTypeCombo->addItem(i18n("Standard NickServ"), "nickserv");
        m_authTypeCombo->addItem(i18n("Server Password"), "serverpw");
        m_authTypeCombo->addItem(i18n("SASL"), "saslplain");
#if KDE_IS_VERSION(4, 8, 3)
m_authTypeCombo->addItem(i18n("SSL Client Certificate"), "pemclientcert");
#endif

        // add encodings to combo box
        m_codecCBox->addItems(Konversation::IRCCharsets::self()->availableEncodingDescriptiveNames());

        // set the suffix for the inactivity time spinbox
        m_awayInactivitySpin->setSuffix(ki18np(" minute", " minutes"));

        // set values for the widgets
        updateIdentity(0);

        // Set up signals / slots for identity page
        //connect(m_identityCBox, SIGNAL(activated(int)), this, SLOT(updateIdentity(int)));

        setButtonGuiItem(KDialog::Ok, KGuiItem(i18n("&OK"), "dialog-ok", i18n("Change identity information")));
        setButtonGuiItem(KDialog::Cancel, KGuiItem(i18n("&Cancel"), "dialog-cancel", i18n("Discards all changes made")));

        AwayManager* awayManager = static_cast<Application*>(kapp)->getAwayManager();
        connect(m_identityCBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateIdentity(int)));
        connect(this, SIGNAL(identitiesChanged()), awayManager, SLOT(identitiesChanged()));
    }

    void IdentityDialog::updateIdentity(int index)
    {
        if (m_currentIdentity && !checkCurrentIdentity())
        {
            return;
        }

        refreshCurrentIdentity();

        m_currentIdentity = m_identityList[index];

        m_realNameEdit->setText(m_currentIdentity->getRealName());
        m_nicknameLBox->clear();
        m_nicknameLBox->insertStringList(m_currentIdentity->getNicknameList());

        m_authTypeCombo->setCurrentIndex(m_authTypeCombo->findData(m_currentIdentity->getAuthType()));
        m_authPasswordEdit->setText(m_currentIdentity->getAuthPassword());
        m_nickservNicknameEdit->setText(m_currentIdentity->getNickservNickname());
        m_nickservCommandEdit->setText(m_currentIdentity->getNickservCommand());
        m_saslAccountEdit->setText(m_currentIdentity->getSaslAccount());
        m_pemClientCertFile->setUrl(m_currentIdentity->getPemClientCertFile());

        m_insertRememberLineOnAwayChBox->setChecked(m_currentIdentity->getInsertRememberLineOnAway());
        m_awayMessageEdit->setText(m_currentIdentity->getAwayMessage());
        m_awayNickEdit->setText(m_currentIdentity->getAwayNickname());
        awayCommandsGroup->setChecked(m_currentIdentity->getRunAwayCommands());
        m_awayEdit->setText(m_currentIdentity->getAwayCommand());
        m_unAwayEdit->setText(m_currentIdentity->getReturnCommand());
        automaticAwayGroup->setChecked(m_currentIdentity->getAutomaticAway());
        m_awayInactivitySpin->setValue(m_currentIdentity->getAwayInactivity());
        m_automaticUnawayChBox->setChecked(m_currentIdentity->getAutomaticUnaway());

        m_sCommandEdit->setText(m_currentIdentity->getShellCommand());
        m_codecCBox->setCurrentIndex(Konversation::IRCCharsets::self()->shortNameToIndex(m_currentIdentity->getCodecName()));
        m_loginEdit->setText(m_currentIdentity->getIdent());
        m_quitEdit->setText(m_currentIdentity->getQuitReason());
        m_partEdit->setText(m_currentIdentity->getPartReason());
        m_kickEdit->setText(m_currentIdentity->getKickReason());

        if(index == 0)
        {
            m_editBtn->setEnabled(false);
            m_delBtn->setEnabled(false);
        }
        else
        {
            m_editBtn->setEnabled(true);
            m_delBtn->setEnabled(true);
        }
    }

    void IdentityDialog::refreshCurrentIdentity()
    {
        if(!m_currentIdentity)
        {
            return;
        }

        m_currentIdentity->setRealName(m_realNameEdit->text());
        const QStringList nicks = m_nicknameLBox->items();
        m_currentIdentity->setNicknameList(nicks);

        m_currentIdentity->setAuthType(m_authTypeCombo->itemData(m_authTypeCombo->currentIndex()).toString());
        m_currentIdentity->setAuthPassword(m_authPasswordEdit->text());
        m_currentIdentity->setNickservNickname(m_nickservNicknameEdit->text());
        m_currentIdentity->setNickservCommand(m_nickservCommandEdit->text());
        m_currentIdentity->setSaslAccount(m_saslAccountEdit->text());
        m_currentIdentity->setPemClientCertFile(m_pemClientCertFile->url());

        m_currentIdentity->setInsertRememberLineOnAway(m_insertRememberLineOnAwayChBox->isChecked());
        m_currentIdentity->setAwayMessage(m_awayMessageEdit->text());
        m_currentIdentity->setAwayNickname(m_awayNickEdit->text());
        m_currentIdentity->setRunAwayCommands(awayCommandsGroup->isChecked());
        m_currentIdentity->setAwayCommand(m_awayEdit->text());
        m_currentIdentity->setReturnCommand(m_unAwayEdit->text());
        m_currentIdentity->setAutomaticAway(automaticAwayGroup->isChecked());
        m_currentIdentity->setAwayInactivity(m_awayInactivitySpin->value());
        m_currentIdentity->setAutomaticUnaway(m_automaticUnawayChBox->isChecked());

        m_currentIdentity->setShellCommand(m_sCommandEdit->text());
        if(m_codecCBox->currentIndex() >= 0 && m_codecCBox->currentIndex() < Konversation::IRCCharsets::self()->availableEncodingShortNames().count())
            m_currentIdentity->setCodecName(Konversation::IRCCharsets::self()->availableEncodingShortNames()[m_codecCBox->currentIndex()]);
        m_currentIdentity->setIdent(m_loginEdit->text());
        m_currentIdentity->setQuitReason(m_quitEdit->text());
        m_currentIdentity->setPartReason(m_partEdit->text());
        m_currentIdentity->setKickReason(m_kickEdit->text());
    }

    void IdentityDialog::accept()
    {
        if (!checkCurrentIdentity())
        {
            return;
        }

        refreshCurrentIdentity();
        Preferences::setIdentityList(m_identityList);
        static_cast<Application*>(kapp)->saveOptions(true);
        emit identitiesChanged();
        KDialog::accept();
    }

    void IdentityDialog::newIdentity()
    {
        bool ok = false;
        QString txt = KInputDialog::getText(i18n("Add Identity"), i18n("Identity name:"), QString(), &ok, this);

        if(ok && !txt.isEmpty())
        {
            KUser user(KUser::UseRealUserID);
            IdentityPtr identity=IdentityPtr(new Identity);
            identity->setName(txt);
            identity->setIdent(user.loginName());
            m_identityList.append(identity);
            m_identityCBox->addItem(txt);
            m_identityCBox->setCurrentIndex(m_identityCBox->count() - 1);
        }
        else if(ok && txt.isEmpty())
        {
            KMessageBox::error(this, i18n("You need to give the identity a name."));
            newIdentity();
        }
    }

    void IdentityDialog::renameIdentity()
    {
        bool ok = false;
        QString currentTxt = m_identityCBox->currentText();
        QString txt = KInputDialog::getText(i18n("Rename Identity"), i18n("Identity name:"), currentTxt, &ok, this);

        if(ok && !txt.isEmpty())
        {
            m_currentIdentity->setName(txt);
            m_identityCBox->setItemText(m_identityCBox->currentIndex(), txt);
        }
        else if(ok && txt.isEmpty())
        {
            KMessageBox::error(this, i18n("You need to give the identity a name."));
            renameIdentity();
        }
    }

    void IdentityDialog::deleteIdentity()
    {
        int current = m_identityCBox->currentIndex();

        if(current <= 0)
        {
            return;
        }

        ServerGroupHash serverGroups = Preferences::serverGroupHash();
        QHashIterator<int, ServerGroupSettingsPtr> it(serverGroups);
        bool found = false;

        while (it.hasNext() && !found)
            if (it.next().value()->identityId() == m_currentIdentity->id()) found = true;

        QString warningTxt;

        if(found)
        {
            warningTxt = i18n("This identity is in use, if you remove it the network settings using it will"
                " fall back to the default identity. Should it be deleted anyway?");
        }
        else
        {
            warningTxt = i18n("Are you sure you want to delete all information for this identity?");
        }

        if(KMessageBox::warningContinueCancel(this, warningTxt, i18n("Delete Identity"),
            KGuiItem(i18n("Delete"), "edit-delete")) == KMessageBox::Continue)
        {
            m_identityList.removeOne(m_currentIdentity);
            m_currentIdentity = 0;
            m_identityCBox->removeItem(current);
        }
    }

    void IdentityDialog::copyIdentity()
    {
        bool ok = false;
        QString currentTxt = m_identityCBox->currentText();
        QString txt = KInputDialog::getText(i18n("Duplicate Identity"), i18n("Identity name:"), currentTxt, &ok, this);

        if(ok && !txt.isEmpty())
        {
            IdentityPtr identity(new Identity);
            identity->copy(*m_currentIdentity);
            identity->setName(txt);
            m_identityList.append(identity);
            m_identityCBox->addItem(txt);
            m_identityCBox->setCurrentIndex(m_identityCBox->count() - 1);
        }
        else if(ok && txt.isEmpty())
        {
            KMessageBox::error(this, i18n("You need to give the identity a name."));
            renameIdentity();
        }
    }

    void IdentityDialog::setCurrentIdentity(int index)
    {
        if (index >= m_identityCBox->count())
            index = 0;

        m_identityCBox->setCurrentIndex(index);
    }

    IdentityPtr IdentityDialog::setCurrentIdentity(IdentityPtr identity)
    {
        int index = Preferences::identityList().indexOf(identity);
        setCurrentIdentity(index);

        return m_currentIdentity;
    }

    IdentityPtr IdentityDialog::currentIdentity() const
    {
        return m_currentIdentity;
    }

    bool IdentityDialog::checkCurrentIdentity()
    {
        if(m_nicknameLBox->count() == 0)
        {
            KMessageBox::error(this, i18n("You must add at least one nick to the identity."));
            bool block = m_identityCBox->blockSignals(true);
            m_identityCBox->setCurrentIndex(m_identityCBox->findText(m_currentIdentity->getName()));
            m_identityCBox->blockSignals(block);
            tabWidget->setCurrentIndex(0);
            m_nicknameLBox->lineEdit()->setFocus();
            return false;
        }

        if(m_realNameEdit->text().isEmpty())
        {
            KMessageBox::error(this, i18n("Please enter a real name."));
            bool block = m_identityCBox->blockSignals(true);
            m_identityCBox->setCurrentIndex(m_identityCBox->findText(m_currentIdentity->getName()));
            m_identityCBox->blockSignals(block);
            tabWidget->setCurrentIndex(0);
            m_realNameEdit->setFocus();
            return false;
        }

        return true;
    }

    void IdentityDialog::authTypeChanged(int index)
    {
        QString authType = m_authTypeCombo->itemData(index).toString();

        bool isNickServ = (authType == "nickserv");
        bool isSaslPlain = (authType == "saslplain");
        bool isServerPw = (authType == "serverpw");
        bool isPemClientCert = (authType == "pemclientcert");

        nickservNicknameLabel->setVisible(isNickServ);
        m_nickservNicknameEdit->setVisible(isNickServ);
        nickservCommandLabel->setVisible(isNickServ);
        m_nickservCommandEdit->setVisible(isNickServ);
        saslAccountLabel->setVisible(isSaslPlain);
        m_saslAccountEdit->setVisible(isSaslPlain);
        authPasswordLabel->setVisible(!isPemClientCert);
        m_authPasswordEdit->setVisible(!isPemClientCert);
        pemClientCertFileLabel->setVisible(isPemClientCert);
        m_pemClientCertFile->setVisible(isPemClientCert);
        m_additionalAuthInfo->setVisible(isServerPw || isPemClientCert);

        for (int i = 0; i < autoIdentifyLayout->count(); ++i)
            autoIdentifyLayout->removeItem(autoIdentifyLayout->itemAt(0));

        autoIdentifyLayout->addRow(authTypeLabel, m_authTypeCombo);

        if (isNickServ)
        {
            autoIdentifyLayout->addRow(nickservNicknameLabel, m_nickservNicknameEdit);
            autoIdentifyLayout->addRow(nickservCommandLabel, m_nickservCommandEdit);
            autoIdentifyLayout->addRow(authPasswordLabel, m_authPasswordEdit);
        }
        else if (isServerPw)
        {
            autoIdentifyLayout->addRow(authPasswordLabel, m_authPasswordEdit);

            m_additionalAuthInfo->setText(i18n("The password entered here will override the one set in the server settings, if any."));
            autoIdentifyLayout->addRow(0, m_additionalAuthInfo);
        }
        else if (isSaslPlain)
        {
            autoIdentifyLayout->addRow(saslAccountLabel, m_saslAccountEdit);
            autoIdentifyLayout->addRow(authPasswordLabel, m_authPasswordEdit);
        }
        else if (isPemClientCert)
        {
            autoIdentifyLayout->addRow(pemClientCertFileLabel, m_pemClientCertFile);

            m_additionalAuthInfo->setText(i18n("SSL Client Certificate authentication forces SSL to be enabled for a connection, overriding any server settings."));
            autoIdentifyLayout->addRow(0, m_additionalAuthInfo);
        }
    }
}
