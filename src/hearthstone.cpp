#include "hearthstone.h"

#include <QFile>
#include <QDesktopServices>

#ifdef Q_WS_MAC
#include "osx_window_capture.h"
#elif defined Q_WS_WIN
#include "win_window_capture.h"
#include "Shlobj.h"
#endif

DEFINE_SINGLETON_SCOPE(Hearthstone)

Hearthstone::Hearthstone() {
#ifdef Q_WS_MAC
  capture = new OSXWindowCapture("Hearthstone");
#elif defined Q_WS_WIN
  capture = new WinWindowCapture("Hearthstone");
#endif
}

Hearthstone::~Hearthstone() {
  if(capture != NULL)
    delete capture;
}

bool Hearthstone::IsRunning() {
  return capture->WindowFound();
}

#ifdef Q_WS_WIN
inline float roundf(float x) {
   return x >= 0.0f ? floorf(x + 0.5f) : ceilf(x - 0.5f);
}
#endif

QPixmap Hearthstone::Capture(int vx, int vy, int vw, int vh) {
  int x, y, w, h;

  int realCanvasWidth = capture->GetWidth();
  int realCanvasHeight = capture->GetHeight();

  int virtualCanvasWidth = VIRTUAL_CANVAS_WIDTH;
  int virtualCanvasHeight = VIRTUAL_CANVAS_HEIGHT;

  float dx = (vx - virtualCanvasWidth/2);
  float dy = (vy - virtualCanvasHeight/2);
  float scale = (float)realCanvasHeight / virtualCanvasHeight;

  // Rounding here is important for dhash calc
  x = roundf(realCanvasWidth/2 + dx * scale);
  y = roundf(realCanvasHeight/2 + dy * scale);
  w = roundf(vw * scale);
  h = roundf(vh * scale);

  return capture->Capture(x, y, w, h);
}

void Hearthstone::SetWindowCapture(WindowCapture *wc) {
  if(capture != NULL)
    delete capture;

  capture = wc;
}

void Hearthstone::EnableLogging() {
  string path = LogConfigPath();
  QFile file(path.c_str());
  if(!file.exists()) {
    LOG("Enable Hearthstone logging by creating file %s", path.c_str());
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      LOG("Couldn't create file");
    } else {
      QTextStream out(&file);
      out << "[Zone]\n";
      out << "LogLevel=1\n";
      out << "ConsolePrinting=true\n";
      file.close();

      LOG("Ingame Log activated.");
      if(IsRunning()) {
        LOG("Please restart Hearthstone for logging to take effect.");
      }
    }
  }
}

void Hearthstone::DisableLogging() {
  QFile file(LogConfigPath().c_str());
  if(file.exists()) {
    file.remove();
    LOG("Ingame log deactivated.");
  }
}

string Hearthstone::LogConfigPath() {
#ifdef Q_WS_MAC
  QString homeLocation = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
  QString configPath = homeLocation + "/Library/Preferences/Blizzard/Hearthstone/log.config";
#elif defined Q_WS_WIN
  char buffer[MAX_PATH];
  SHGetSpecialFolderPathA(NULL, buffer, CSIDL_LOCAL_APPDATA, FALSE);
  QString localAppData(buffer);
  QString configPath = localAppData + "\\Blizzard\\Hearthstone\\log.config";
#endif
  return configPath.toStdString();
}

string Hearthstone::LogPath() {
#ifdef Q_WS_MAC
  QString homeLocation = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
  QString logPath = homeLocation + "/Library/Logs/Unity/Player.log";
#elif defined Q_WS_WIN
  QString programFiles(getenv("PROGRAMFILES(X86)"));
  if(programFiles.isEmpty()) {
    programFiles = getenv("PROGRAMFILES");
  }
  QString logPath = programFiles + "\\Hearthstone\\Hearthstone_Data\\output_log.txt";
#endif
  return logPath.toStdString();
}

int Hearthstone::GetWidth() {
  return capture->GetWidth();
}

int Hearthstone::GetHeight() {
  return capture->GetHeight();
}
