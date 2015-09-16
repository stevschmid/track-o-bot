#pragma once

#include <QWidget>

namespace Ui { class AccountWidget; }

class AccountTab : public QWidget
{
  Q_OBJECT

private:
  Ui::AccountWidget *mUI;

private slots:
  void ExportAccount();
  void ImportAccount();

public:
  explicit AccountTab( QWidget *parent = 0 );
  ~AccountTab();

public slots:
  void LoadAccount();
};

