#include <QtGui>
#include "window.h"

TabLogHandler::TabLogHandler(LogTab *logTab)
  :tab(logTab)
{
}

void TabLogHandler::HandleLogEntry(const string& entry) {
  tab->addLogEntry(entry);
}

SettingsTab::SettingsTab(QWidget *parent)
  : QWidget(parent)
{
  QVBoxLayout *layout = new QVBoxLayout;

  layout->addWidget(new QLabel(tr("Username:")));
  username = new QLineEdit;
  layout->addWidget(username);

  layout->addWidget(new QLabel(tr("Password:")));
  password = new QLineEdit;

  QHBoxLayout *row = new QHBoxLayout;
  QPushButton *revealButton = new QPushButton(tr("Reveal"));
  connect(revealButton, SIGNAL(clicked()), this, SLOT(reveal()));
  row->addWidget(password);
  row->addWidget(revealButton);

  layout->addItem(row);

  startAtLogin = new QCheckBox(tr("Start at Login"));
  layout->addWidget(startAtLogin);

  setLayout(layout);
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

LogTab::LogTab(QWidget *parent)
  : QWidget(parent), logHandler(this)
{
  QVBoxLayout *layout = new QVBoxLayout;

  logText = new QTextEdit;
  logText->setReadOnly(true);
  layout->addWidget(logText);

  setLayout(layout);

  gLogger.RegisterObserver(&logHandler);
}

LogTab::~LogTab() {
  gLogger.UnregisterObserver(&logHandler);
}

void LogTab::addLogEntry(const string& entry) {
  logText->moveCursor(QTextCursor::End);
  logText->insertPlainText(entry.c_str());
  logText->moveCursor(QTextCursor::End);
}

Window::Window()
{
  setWindowTitle(APP_NAME);

  Qt::WindowFlags flags = windowFlags();
  flags |= Qt::CustomizeWindowHint;
  flags &= ~Qt::WindowMaximizeButtonHint;
  setWindowFlags(flags);

  createActions();
  createTrayIcon();

  tabWidget = new QTabWidget;
  settingsTab = new SettingsTab;
  logTab = new LogTab;

  tabWidget->addTab(settingsTab, tr("Settings"));
  tabWidget->addTab(logTab, tr("Log"));

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->setSizeConstraint(QLayout::SetNoConstraint);
  mainLayout->addWidget(tabWidget);
  mainLayout->addWidget(buttonBox);

  setLayout(mainLayout);
  setFixedSize(400, 300);
}

Window::~Window() {
}

void Window::showEvent(QShowEvent *event) {
  QDialog::showEvent(event);
  settingsTab->updateSettings();
}

void Window::ok() {
  hide();
  settingsTab->applySettings();
}

void Window::cancel() {
  hide();
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
  core.Tracker().OpenProfile();
}
