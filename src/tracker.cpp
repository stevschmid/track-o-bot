#include "tracker.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QTimer>
#include <QDesktopServices>

#include "json.h"

#define DEFAULT_WEBSERVICE_URL "https://trackobot.com"

DEFINE_SINGLETON_SCOPE(Tracker);

Tracker::Tracker() {
}

Tracker::~Tracker() {
}

void Tracker::EnsureAccountIsSetUp() {
  if(!IsAccountSetUp()) {
    LOG("No account setup. Creating one for you.");
    CreateAndStoreAccount();
  } else {
    LOG("Account %s found", Username().toStdString().c_str());
  }
}

void Tracker::AddResult(GameMode mode, Outcome outcome, GoingOrder order, Class ownClass, Class opponentClass, const CardHistoryList& historyCardList) {
#ifndef _DEBUG
  if(mode == MODE_PRACTICE) {
    LOG("Ignore practice game."); // only in Non Debug Versions
    return;
  }
#endif

#ifdef _DEBUG
  string cardHistoryOutput;
  for(CardHistoryList::const_iterator it = historyCardList.begin(); it != historyCardList.end(); ++it) {
    cardHistoryOutput += (*it).myPlay ? "SELF " : "OPPONENT ";
    cardHistoryOutput += (*it).cardId + "\n";
  }
  LOG("Card History: %s", cardHistoryOutput.c_str());
#endif

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

  LOG("Upload %s %s vs %s as %s. Went %s",
      MODE_NAMES[mode], OUTCOME_NAMES[outcome], CLASS_NAMES[opponentClass], CLASS_NAMES[ownClass], ORDER_NAMES[order]);

  QtJson::JsonObject result;
  result["coin"]     = (order == ORDER_FIRST);
  result["hero"]     = CLASS_NAMES[ownClass];
  result["opponent"] = CLASS_NAMES[opponentClass];
  result["win"]      = (outcome == OUTCOME_VICTORY);
  result["mode"]     = MODE_NAMES[mode];

  QtJson::JsonArray card_history;
  for(CardHistoryList::const_iterator it = historyCardList.begin(); it != historyCardList.end(); ++it) {
    QtJson::JsonObject item;
    item["player"] = (*it).myPlay ? "me" : "opponent";
    item["card_id"] = (*it).cardId.c_str();
    card_history.append(item);
  }
  result["card_history"] = card_history;

  QtJson::JsonObject params;
  params["result"] = result;

  QByteArray data = QtJson::serialize(params);

  QNetworkReply *reply = AuthPostJson("/profile/results.json", data);
  connect(reply, SIGNAL(finished()), this, SLOT(AddResultHandleReply()));
}

QNetworkReply* Tracker::AuthPostJson(const QString& path, const QByteArray& data) {
  QString credentials = "Basic " + (Username() + ":" + Password()).toAscii().toBase64();

  QUrl url(WebserviceURL(path));
  QNetworkRequest request(url);

  request.setRawHeader(QByteArray("Authorization"), credentials.toAscii());
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  return networkManager.post(request, data);
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
  QUrl url(WebserviceURL("/users.json"));
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
  QNetworkReply *reply = AuthPostJson("/one_time_auth.json", "");
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

QString Tracker::WebserviceURL(const QString& path) {
  return WebserviceURL() + path;
}

QString Tracker::WebserviceURL() {
  if(!settings.contains("webserviceUrl") || settings.value("webserviceUrl").toString().isEmpty()) {
    SetWebserviceURL(DEFAULT_WEBSERVICE_URL);
  }

  return settings.value("webserviceUrl").toString();
}

void Tracker::SetUsername(const QString& username) {
  settings.setValue("username", username);
}

void Tracker::SetPassword(const QString& password) {
  settings.setValue("password", password);
}

void Tracker::SetWebserviceURL(const QString& webserviceUrl) {
  settings.setValue("webserviceUrl", webserviceUrl);
}

bool Tracker::IsAccountSetUp() {
  return settings.contains("username") && settings.contains("password") &&
    !settings.value("username").toString().isEmpty() && !settings.value("password").toString().isEmpty();
}
