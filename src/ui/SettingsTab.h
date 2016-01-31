#pragma once

#include <QWidget>

namespace Ui { class SettingsWidget; }

class SettingsTab : public QWidget
{
  Q_OBJECT

private:
  Ui::SettingsWidget *mUI;

private slots:
  void CheckForUpdatesNow();

public:
  explicit SettingsTab( QWidget *parent = 0 );
  ~SettingsTab();

public slots:
  void UpdateAutostart();
  void UpdateAutoUpdateCheck();
  void UpdateUploadMetadata();
  void SelectHearthstoneDirectoryPath();
  void LoadSettings();
};

