#include "tracker.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QTimer>
#include <QDesktopServices>

#include "json.h"

Tracker::Tracker() {
  // delay this func so logging system gets ready
  QTimer::singleShot(0, this, SLOT(EnsureAccountIsSetUp()));
}

void Tracker::EnsureAccountIsSetUp() {
  if(!IsAccountSetUp()) {
    LOG("No account setup. Creating one for you.");
    CreateAndStoreAccount();
  } else {
    LOG("Account %s found", Username().toStdString().c_str());
  }
}

void Tracker::AddResult(GameMode mode, Outcome outcome, GoingOrder order, Class ownClass, Class opponentClass) {
  LOG("Upload Result. Mode: %s. Outcome %s: Order: %s. Class: %s. Opponent %s",
      MODE_NAMES[mode], OUTCOME_NAMES[outcome], ORDER_NAMES[order], CLASS_NAMES[ownClass], CLASS_NAMES[opponentClass]);

  if(mode == MODE_UNKNOWN) {
    LOG("Mode unknown. Skip result");
    return;
  }

  if(outcome == OUTCOME_UNKNOWN) {
    LOG("Outcome unknown. Skip result");
    return;
  }

  if(order == ORDER_UNKNOWN) {
    LOG("Order unknown. Skip result");
    return;
  }

  if(ownClass == CLASS_UNKNOWN) {
    LOG("Own Class unknown. Skip result");
    return;
  }

  if(opponentClass == CLASS_UNKNOWN) {
    LOG("Class of Opponent unknown. Skip result");
    return;
  }

  QUrl url("http://webtracker.dev/results.json");
  url.setUserName(Username());
  url.setPassword(Password());

  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QtJson::JsonObject result;
  result["coin"]     = (order == ORDER_FIRST);
  result["hero"]     = CLASS_NAMES[ownClass];
  result["opponent"] = CLASS_NAMES[opponentClass];
  result["win"]      = (outcome == OUTCOME_VICTORY);
  result["mode"]     = MODE_NAMES[mode];

  QtJson::JsonObject params;
  params["result"] = result;

  QByteArray data = QtJson::serialize(params);
  QNetworkReply *reply = networkManager.post(request, data);
  connect(reply, SIGNAL(finished()), this, SLOT(AddResultHandleReply()));
}

void Tracker::AddResultHandleReply() {
  QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
  if(reply->error() == QNetworkReply::NoError) {
    LOG("Result was uploaded succesfully!");
  } else {
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    LOG("There was a problem uploading the result. Error: %i HTTP Status Code: %i", reply->error(), statusCode);
  }
}

void Tracker::CreateAndStoreAccount() {
  QUrl url("http://webtracker.dev/users.json");
  QNetworkRequest request(url);
  QNetworkReply *reply = networkManager.post(request, "");
  connect(reply, SIGNAL(finished()), this, SLOT(CreateAndStoreAccountHandleReply()));
}

void Tracker::CreateAndStoreAccountHandleReply() {
  QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
  if(reply->error() == QNetworkReply::NoError) {
    LOG("Account creation was successful!");

    QByteArray jsonData = reply->readAll();

    bool ok;
    QtJson::JsonObject user = QtJson::parse(jsonData, ok).toMap();

    if(!ok) {
      LOG("Couldn't parse response");
    } else {
      LOG("Welcome %s", user["username"].toString().toStdString().c_str());

      QSettings settings;
      settings.setValue("username", user["username"].toString());
      settings.setValue("password", user["password"].toString());
    }
  } else {
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    LOG("There was a problem creating an account. Error: %i HTTP Status Code: %i", reply->error(), statusCode);
  }
}

void Tracker::OpenProfile() {
  QUrl url("http://webtracker.dev/one_time_auth.json");
  url.setUserName(Username());
  url.setPassword(Password());
  QNetworkRequest request(url);
  QNetworkReply *reply = networkManager.post(request, "");
  connect(reply, SIGNAL(finished()), this, SLOT(OpenProfileHandleReply()));
}

void Tracker::OpenProfileHandleReply() {
  QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
  if(reply->error() == QNetworkReply::NoError) {
    QByteArray jsonData = reply->readAll();

    bool ok;
    QtJson::JsonObject response = QtJson::parse(jsonData, ok).toMap();

    if(!ok) {
      LOG("Couldn't parse response");
    } else {
      QString url = response["url"].toString();
      QDesktopServices::openUrl(QUrl(url));
    }
  } else {
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    LOG("There was a problem creating an auth token. Error: %i HTTP Status Code: %i", reply->error(), statusCode);
  }
}

QString Tracker::Username() {
  return settings.value("username").toString();
}

QString Tracker::Password() {
  return settings.value("password").toString();
}

void Tracker::SetUsername(const QString& username) {
  settings.setValue("username", username);
}

void Tracker::SetPassword(const QString& password) {
  settings.setValue("password", password);
}

bool Tracker::IsAccountSetUp() {
  return settings.contains("username") && settings.contains("password");
}
