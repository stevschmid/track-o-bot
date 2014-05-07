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

  QGridLayout *layout = new QGridLayout;
  setLayout(layout);
  layout->setVerticalSpacing(15);

  layout->addWidget(usernameLabel, 0, 0);
  layout->addWidget(username, 0, 1);

  layout->addWidget(passwordLabel, 1, 0);
  layout->addWidget(password, 1, 1);

  QPushButton *revealButton = new QPushButton(tr("Reveal"));
  connect(revealButton, SIGNAL(clicked()), this, SLOT(reveal()));
  layout->addWidget(revealButton, 1, 2);

  QFrame *line = new QFrame;
  line->setObjectName(QString::fromUtf8("line"));
  line->setGeometry(QRect(320, 150, 118, 30));
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  layout->addWidget(line, 2, 0, 1, 3);


  QLabel *systemLabel = new QLabel(tr("System:"));
  layout->addWidget(systemLabel, 4, 0);

  startAtLogin = new QCheckBox(tr("Start at Login"));
  layout->addWidget(startAtLogin, 4, 1);

  QSpacerItem *spacer = new QSpacerItem(40, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
  layout->addItem(spacer, 5, 0);

  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));
  layout->addWidget(buttonBox, 6, 0, 1, 3);
}

void SettingsTab::reveal() {
  password->setEchoMode(QLineEdit::Normal);
}

void SettingsTab::conceal() {
  password->setEchoMode(QLineEdit::Password);
}

void SettingsTab::updateSettings() {
  Autostart autostart;
  QSettings settings;

  username->setText(settings.value("username").toString());
  password->setText(settings.value("password").toString());
  conceal();

  startAtLogin->setChecked(autostart.IsActive());
}

void SettingsTab::applySettings() {
  QSettings settings;
  Autostart autostart;
  settings.setValue("username", username->text());
  settings.setValue("password", password->text());
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
}

void LogTab::addLogEntry(QtMsgType type, const char *msg) {
  logText->moveCursor(QTextCursor::End);
  logText->insertPlainText(msg);
  logText->moveCursor(QTextCursor::End);
}

void Window::addLogEntry(QtMsgType type, const char *msg) {
  logTab->addLogEntry(type, msg);
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

  layout->addItem(topSpacer);
  layout->addWidget(name);
  layout->addWidget(version);
  layout->addItem(bottomSpacer);

  setLayout(layout);
}

Window::Window()
{
  Qt::WindowFlags flags = windowFlags();
  flags |= Qt::CustomizeWindowHint;
  flags &= ~Qt::WindowMaximizeButtonHint;
  setWindowFlags(flags);

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
  setFixedSize(400, 250);
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

  QIcon icon = QIcon(":/icons/paw.svg");
#ifdef Q_WS_MAC
  icon.addFile(":/icons/paw_inverted.svg", QSize(), QIcon::Selected);
#endif
  trayIcon->setIcon(icon);
  trayIcon->show();
}

void Window::riseAndShine() {
  show();
  raise();
}

void Window::openProfile() {
  core.OpenProfile();
}
