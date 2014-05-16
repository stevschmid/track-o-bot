#include <QtGui>
#include "window.h"

SettingsTab::SettingsTab(QWidget *parent)
  : QWidget(parent)
{

  QLabel *usernameLabel = new QLabel(tr("Username:"));
  username = new QLineEdit;
  usernameLabel->setBuddy(username);

  QLabel *passwordLabel = new QLabel(tr("Password:"));
  password = new QLineEdit;
  passwordLabel->setBuddy(password);

  QLabel *webserviceUrlLabel = new QLabel(tr("Webservice URL:"));
  webserviceUrl = new QLineEdit;
  webserviceUrlLabel->setBuddy(webserviceUrl);

  QGridLayout *layout = new QGridLayout;
  setLayout(layout);
  layout->setVerticalSpacing(15);

  layout->addWidget(webserviceUrlLabel, 0, 0);
  layout->addWidget(webserviceUrl, 0, 1);

  layout->addWidget(usernameLabel, 1, 0);
  layout->addWidget(username, 1, 1);

  layout->addWidget(passwordLabel, 2, 0);

#ifndef _DEBUG
  webserviceUrlLabel->hide();
  webserviceUrl->hide();
#endif

  QHBoxLayout *row = new QHBoxLayout;
  QPushButton *revealButton = new QPushButton(tr("Reveal"));
  connect(revealButton, SIGNAL(clicked()), this, SLOT(reveal()));

  row->addWidget(password);
  row->addWidget(revealButton);
  layout->addLayout(row, 2, 1);

  QFrame *line = new QFrame;
  line->setObjectName(QString::fromUtf8("line"));
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  layout->addWidget(line, 3, 0, 1, 3);

  QLabel *systemLabel = new QLabel(tr("System:"));
  layout->addWidget(systemLabel, 5, 0);

  startAtLogin = new QCheckBox(tr("Start at Login"));
  layout->addWidget(startAtLogin, 5, 1);

  QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  layout->addItem(spacer, 6, 0);

  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));
  layout->addWidget(buttonBox, 7, 0, 1, 3);
}

void SettingsTab::reveal() {
  password->setEchoMode(QLineEdit::Normal);
}

void SettingsTab::conceal() {
  password->setEchoMode(QLineEdit::Password);
}

void SettingsTab::updateSettings() {
  Autostart autostart;

  username->setText(Tracker::Instance()->Username());
  password->setText(Tracker::Instance()->Password());
  webserviceUrl->setText(Tracker::Instance()->WebserviceURL());
  conceal();

  startAtLogin->setChecked(autostart.IsActive());
}

void SettingsTab::applySettings() {
  Autostart autostart;

  Tracker::Instance()->SetUsername(username->text());
  Tracker::Instance()->SetPassword(password->text());
  Tracker::Instance()->SetWebserviceURL(webserviceUrl->text());
  autostart.SetActive(startAtLogin->isChecked());
}

void SettingsTab::ok() {
  applySettings();
  window()->hide();
}

void SettingsTab::cancel() {
  window()->hide();
}


LogTab::LogTab(QWidget *parent)
  : QWidget(parent)
{
  QVBoxLayout *layout = new QVBoxLayout;

  logText = new QTextEdit;
  logText->setReadOnly(true);
  layout->addWidget(logText);

  setLayout(layout);

  connect(Logger::Instance(), SIGNAL(NewMessage(const string&)), this, SLOT(addLogEntry(const string&)));
}

void LogTab::addLogEntry(const string& msg) {
  logText->moveCursor(QTextCursor::End);
  logText->insertPlainText(msg.c_str());
  logText->moveCursor(QTextCursor::End);
}

AboutTab::AboutTab(QWidget *parent)
  : QWidget(parent)
{
  QVBoxLayout *layout = new QVBoxLayout;

  QSpacerItem *topSpacer = new QSpacerItem(40, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
  QSpacerItem *bottomSpacer = new QSpacerItem(40, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

  QLabel *name = new QLabel(qApp->applicationName());
  name->setStyleSheet("QLabel { font-size: 20px; }");
  name->setAlignment(Qt::AlignHCenter);

  QLabel *version = new QLabel(VERSION);
  version->setAlignment(Qt::AlignHCenter);

  QPixmap logoImage(":/icons/logo.png");
  QLabel *logo = new QLabel();
  logo->setAlignment(Qt::AlignHCenter);
  logo->setPixmap(logoImage.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));

  layout->addItem(topSpacer);
  layout->addWidget(logo);
  layout->addWidget(name);
  layout->addWidget(version);
  layout->addItem(bottomSpacer);

  setLayout(layout);
}

Window::Window()
{
  setWindowTitle(qApp->applicationName());

  createActions();
  createTrayIcon();

  tabWidget = new QTabWidget;
  settingsTab = new SettingsTab;
  logTab = new LogTab;
  aboutTab = new AboutTab;

  tabWidget->addTab(settingsTab, tr("Settings"));
  tabWidget->addTab(logTab, tr("Log"));
  tabWidget->addTab(aboutTab, tr("About"));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->setSizeConstraint(QLayout::SetNoConstraint);
  mainLayout->addWidget(tabWidget);

  setLayout(mainLayout);

  setMinimumSize(450, 250);
  resize(minimumSize());
}

Window::~Window() {
}

void Window::showEvent(QShowEvent *event) {
  QDialog::showEvent(event);
  settingsTab->updateSettings();
}

void Window::closeEvent(QCloseEvent *event) {
  if(trayIcon->isVisible()) {
    hide();
    event->ignore();
  }
}

// prevent esc from closing the app
void Window::reject()
{
  if(trayIcon->isVisible()) {
    hide();
  } else {
    QDialog::reject();
  }
}

void Window::createActions()
{
  openProfileAction = new QAction(tr("Open Profile..."), this);
  connect(openProfileAction, SIGNAL(triggered()), this, SLOT(openProfile()));

  showAction = new QAction(tr("Settings..."), this);
  connect(showAction, SIGNAL(triggered()), this, SLOT(riseAndShine()));

  quitAction = new QAction(tr("Quit"), this);
  connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void Window::createTrayIcon()
{
  trayIconMenu = new QMenu(this);
  trayIconMenu->addAction(openProfileAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(showAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(quitAction);

  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setContextMenu(trayIconMenu);

#ifdef Q_WS_MAC
  QIcon icon = QIcon(":/icons/tray_mac.png");
  icon.addFile(":/icons/tray_mac_selected.png", QSize(), QIcon::Selected);
#elif defined Q_WS_WIN
  QIcon icon = QIcon(":/icons/tray_win.png");
#endif

  trayIcon->setIcon(icon);
  trayIcon->show();
}

void Window::riseAndShine() {
  show();
  raise();
}

void Window::openProfile() {
  Tracker::Instance()->OpenProfile();
}
