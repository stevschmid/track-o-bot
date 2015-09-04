#include "SettingsTab.h"
#include "ui_SettingsWidget.h"

#include "../Settings.h"

SettingsTab::SettingsTab( QWidget *parent )
  : QWidget( parent ), mUI( new Ui::SettingsWidget )
{
  mUI->setupUi( this );
  connect( mUI->checkForUpdatesNowButton, SIGNAL( clicked() ), this, SLOT( CheckForUpdatesNow() ) );
  connect( mUI->startAtLogin, SIGNAL( clicked(bool) ), this, SLOT( UpdateAutostart() ) );
  connect( mUI->checkForUpdates, SIGNAL( clicked(bool) ), this, SLOT( UpdateAutoUpdateCheck() ) );
  connect( mUI->replaysEnabled, SIGNAL( clicked(bool) ), this, SLOT( UpdateReplaysEnabled() ) );
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

void SettingsTab::UpdateReplaysEnabled() {
  Settings::Instance()->SetReplaysEnabled( mUI->replaysEnabled->isChecked() );
}

void SettingsTab::LoadSettings() {
  Settings *settings = Settings::Instance();

  mUI->startAtLogin->setChecked( settings->Autostart() );
  mUI->checkForUpdates->setChecked( settings->AutoUpdateCheck() );

  bool replayRequirements = settings->ReplayRequirementsFulfilled();
  bool replaysEnabled = replayRequirements && settings->ReplaysEnabled();
  mUI->replaysEnabled->setEnabled( replayRequirements );
  mUI->replaysEnabled->setChecked( replaysEnabled );
}
