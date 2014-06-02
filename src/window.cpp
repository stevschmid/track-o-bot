#include <QtGui>
#include "window.h"

SettingsTab::SettingsTab(QWidget *parent)
  : QWidget(parent)
{
  QLabel *accountLabel = new QLabel(tr("Account:"));
  account = new QLabel;

  QGridLayout *layout = new QGridLayout;
  setLayout(layout);

  QPushButton *exportAccountButton = new QPushButton(tr("Export..."));
  exportAccountButton->setFixedWidth(100);
  connect(exportAccountButton, SIGNAL(clicked()), this, SLOT(exportAccount()));

  QPushButton *importAccountButton = new QPushButton(tr("Import..."));
  importAccountButton->setFixedWidth(100);
  connect(importAccountButton, SIGNAL(clicked()), this, SLOT(importAccount()));

  QLabel *exportDescription = new QLabel(tr("Use export to back up your account or to import it on another device."));
  exportDescription->setWordWrap(true);
  exportDescription->setStyleSheet("QLabel { font-size: 11px; color: #898989 }");

  QFrame *line = new QFrame;
  line->setObjectName(QString::fromUtf8("line"));
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);

  QLabel *systemLabel = new QLabel(tr("System:"));
  startAtLogin = new QCheckBox(tr("Start at Login"));

  QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));

  layout->addWidget(accountLabel, 0, 0);
  layout->addWidget(account, 0, 1);

  layout->addWidget(exportDescription, 1, 1, 1, 2);

  layout->addWidget(exportAccountButton, 2, 1);
  layout->addWidget(importAccountButton, 2, 2);

  layout->addWidget(line, 3, 0, 1, 3);

  layout->addWidget(systemLabel, 4, 0);
  layout->addWidget(startAtLogin, 4, 1);

  layout->addItem(spacer, 5, 0);

  layout->addWidget(buttonBox, 6, 0, 1, 3);
}

void SettingsTab::exportAccount() {
  QString fileName = QFileDialog::getSaveFileName(this,
      tr("Export Track-o-Bot Account Data"), "",
      tr("Account Data (*.track-o-bot);;"));

  if(fileName.isEmpty()) {
    return;
  } else {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
      QMessageBox::information(this, tr("Unable to open file"),
          file.errorString());
      return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_8);
    out << Tracker::Instance()->Username();
    out << Tracker::Instance()->Password();
    out << Tracker::Instance()->WebserviceURL();
  }
}

void SettingsTab::importAccount() {
  QString fileName = QFileDialog::getOpenFileName(this,
      tr("Import Track-o-Bot Account Data"), "",
      tr("Account Data (*.track-o-bot);;"));

  if(fileName.isEmpty()) {
    return;
  } else {
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) {
      QMessageBox::information(this, tr("Unable to open file"),
          file.errorString());
      return;
    }

    QDataStream in(&file);
    QString username, password, webserviceUrl;
    in.setVersion(QDataStream::Qt_4_8);
    in >> username;
    in >> password;
    in >> webserviceUrl;

    Tracker::Instance()->SetUsername(username);
    Tracker::Instance()->SetPassword(password);
    Tracker::Instance()->SetWebserviceURL(webserviceUrl);
  }
}

void SettingsTab::updateSettings() {
  Autostart autostart;

  account->setText(Tracker::Instance()->Username());
  startAtLogin->setChecked(autostart.IsActive());
}

void SettingsTab::applySettings() {
  Autostart autostart;

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
  QFont font("Monospace");
  font.setStyleHint(QFont::TypeWriter);
  logText->setFont(font);
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

  QLabel *slogan = new QLabel("The friendly Hearthstone Tracker");
  slogan->setStyleSheet("QLabel { font-style: italic; }");
  slogan->setAlignment(Qt::AlignHCenter);

  QLabel *version = new QLabel(VERSION);
  version->setAlignment(Qt::AlignHCenter);

  QPixmap logoImage(":/icons/logo.png");
  QLabel *logo = new QLabel();
  logo->setAlignment(Qt::AlignHCenter);
  logo->setPixmap(logoImage.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));

  layout->addItem(topSpacer);
  layout->addWidget(logo);
  layout->addWidget(name);
  layout->addWidget(slogan);
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
