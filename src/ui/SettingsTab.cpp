#include "SettingsTab.h"
#include "ui_SettingsWidget.h"

#include "../Settings.h"

SettingsTab::SettingsTab( QWidget *parent )
  : QWidget( parent ), mUI( new Ui::SettingsWidget )
{
  mUI->setupUi( this );
  connect( mUI->checkForUpdatesNowButton, &QAbstractButton::clicked, this, &SettingsTab::CheckForUpdatesNow );
  connect( mUI->startAtLogin, &QAbstractButton::clicked, this, &SettingsTab::UpdateAutostart );
  connect( mUI->checkForUpdates, &QAbstractButton::clicked, this, &SettingsTab::UpdateAutoUpdateCheck );
  connect( mUI->checkUploadMetadata, &QAbstractButton::clicked, this, &SettingsTab::UpdateMetadataUpload );
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

void SettingsTab::UpdateMetadataUpload() {
  Settings::Instance()->SetUploadMetadataEnabled( mUI->checkUploadMetadata->isChecked() );
}

void SettingsTab::LoadSettings() {
  Settings *settings = Settings::Instance();

  mUI->startAtLogin->setChecked( settings->Autostart() );
  mUI->checkForUpdates->setChecked( settings->AutoUpdateCheck() );
  mUI->checkUploadMetadata->setChecked( settings->UploadMetadataEnabled() );
}
