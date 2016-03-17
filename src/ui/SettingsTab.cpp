#include "SettingsTab.h"
#include "ui_SettingsWidget.h"

#include "../Settings.h"

#include <QFileDialog>
#include <QMessageBox>

SettingsTab::SettingsTab( QWidget *parent )
  : QWidget( parent ), mUI( new Ui::SettingsWidget )
{
  mUI->setupUi( this );
  connect( mUI->checkForUpdatesNowButton, &QAbstractButton::clicked, this, &SettingsTab::CheckForUpdatesNow );
  connect( mUI->startAtLogin, &QAbstractButton::clicked, this, &SettingsTab::UpdateAutostart );
  connect( mUI->checkForUpdates, &QAbstractButton::clicked, this, &SettingsTab::UpdateAutoUpdateCheck );
  connect( mUI->checkDebug, &QAbstractButton::clicked, this, &SettingsTab::UpdateDebug );
  connect( mUI->checkOverlay, &QAbstractButton::clicked, this, &SettingsTab::UpdateOverlayEnabled );
  connect( mUI->selectHearthstoneDirectoryPath, &QAbstractButton::clicked, this, &SettingsTab::SelectHearthstoneDirectoryPath );
  LoadSettings();
}

SettingsTab::~SettingsTab() {
  delete mUI;
}

void SettingsTab::CheckForUpdatesNow() {
  Settings::Instance()->CheckForUpdates();
}

void SettingsTab::UpdateAutostart() {
  Settings::Instance()->SetAutostart( mUI->startAtLogin->isChecked() );
}

void SettingsTab::UpdateAutoUpdateCheck() {
  Settings::Instance()->SetAutoUpdateCheck( mUI->checkForUpdates->isChecked() );
}

void SettingsTab::UpdateDebug() {
  Settings::Instance()->SetDebugEnabled( mUI->checkDebug->isChecked() );
}

void SettingsTab::UpdateOverlayEnabled() {
  Settings::Instance()->SetOverlayEnabled( mUI->checkOverlay->isChecked() );
}

void SettingsTab::LoadSettings() {
  Settings *settings = Settings::Instance();

  mUI->startAtLogin->setChecked( settings->Autostart() );
  mUI->checkForUpdates->setChecked( settings->AutoUpdateCheck() );
  mUI->checkDebug->setChecked( settings->DebugEnabled() );
  mUI->checkOverlay->setChecked( settings->OverlayEnabled() );
  mUI->hearthstoneDirectoryPath->setText( settings->HearthstoneDirectoryPath() );
}

void SettingsTab::SelectHearthstoneDirectoryPath() {
  QString currentPath = Settings::Instance()->HearthstoneDirectoryPath();

  QString dir = QFileDialog::getExistingDirectory( this, tr("Select Hearthstone directory"),
     currentPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );

  if( !dir.isEmpty() ) {
    Settings::Instance()->SetHearthstoneDirectoryPath( dir );
    LoadSettings();

    QMessageBox msgBox( QMessageBox::Information,
        "Restart required!",
        "Please restart Track-o-Bot for changes to take effect!", QMessageBox::Ok, this );
    msgBox.exec();
  }
}
