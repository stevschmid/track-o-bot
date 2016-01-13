#include "AccountTab.h"
#include "ui_AccountWidget.h"

#include "../Settings.h"

#include <QFileDialog>
#include <QMessageBox>

AccountTab::AccountTab( QWidget *parent )
  : QWidget( parent ), mUI( new Ui::AccountWidget )
{
  mUI->setupUi( this );
  connect( mUI->exportAccountButton, &QAbstractButton::clicked, this, &AccountTab::ExportAccount );
  connect( mUI->importAccountButton, &QAbstractButton::clicked, this, &AccountTab::ImportAccount );
  connect( Settings::Instance(), &Settings::AccountChanged, this, &AccountTab::LoadAccount );
  LoadAccount();
}

AccountTab::~AccountTab() {
  delete mUI;
}

void AccountTab::LoadAccount() {
  Settings *settings = Settings::Instance();

  bool accountSetUp = settings->HasAccount();
  if( accountSetUp ) {
    mUI->account->setText( settings->AccountUsername() );
  }

  mUI->importAccountButton->setEnabled( accountSetUp );
  mUI->exportAccountButton->setEnabled( accountSetUp );
}

void AccountTab::ExportAccount() {
  QString fileName = QFileDialog::getSaveFileName( this,
      tr( "Export Track-o-Bot Account Data" ), "",
      tr( "Account Data (*.track-o-bot);; All Files (*)" ) );

  if( fileName.isEmpty() ) {
    return;
  } else {
    QFile file( fileName );
    if( !file.open( QIODevice::WriteOnly ) ) {
      QMessageBox::information( this, tr( "Unable to open file" ), file.errorString() );
      return;
    }

    Settings *settings = Settings::Instance();

    QDataStream out( &file );
    out.setVersion( QDataStream::Qt_4_8 );
    out << settings->AccountUsername();
    out << settings->AccountPassword();
    out << settings->WebserviceURL();

    LOG( "Account %s exported in %s", qt2cstr( settings->AccountUsername() ), qt2cstr( fileName ) );
  }
}

void AccountTab::ImportAccount() {
  QString fileName = QFileDialog::getOpenFileName( this,
      tr( "Import Track-o-Bot Account Data" ), "",
      tr( "Account Data (*.track-o-bot);; All Files (*)" ) );

  if( fileName.isEmpty() ) {
    return;
  } else {
    QFile file( fileName );
    if( !file.open( QIODevice::ReadOnly ) ) {
      QMessageBox::information( this, tr( "Unable to open file" ), file.errorString() );
      return;
    }

    QDataStream in( &file );
    QString username, password, webserviceUrl;
    in.setVersion( QDataStream::Qt_4_8 );
    in >> username;
    in >> password;
    in >> webserviceUrl;

    if( !username.isEmpty() && !password.isEmpty() && !webserviceUrl.isEmpty() ) {
      Settings::Instance()->SetAccount( username, password );
      Settings::Instance()->SetWebserviceURL( webserviceUrl );

      LOG( "Account %s imported from %s", qt2cstr( username ), qt2cstr( fileName ) );
      LoadAccount();
    } else {
      ERR( "Import failed" );
    }
  }
}

