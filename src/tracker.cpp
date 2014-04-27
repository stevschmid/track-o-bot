#include "tracker.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

#include "json.h"

Tracker::Tracker() {
  connect(&networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(OnNetworkReply(QNetworkReply*)));
}

void Tracker::AddResult(GameMode mode, Outcome outcome, GoingOrder order, Class ownClass, Class opponentClass) {
  // TODO DISALLOW API CALL WHEN ANYTHING IS UNKNOWN
  logger << "GameMode " << mode <<
    " Outcome " << outcome <<
    " Order " << order <<
    " OwnCl " << ownClass <<
    " OppCl " << opponentClass << endl;

  QUrl url("http://webtracker.dev/results.json");
  QNetworkRequest request(url);

  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QtJson::JsonObject result;
  result["coin"]     = (order == GOING_FIRST);
  result["hero"]     = CLASS_NAMES[ownClass];
  result["opponent"] = CLASS_NAMES[opponentClass];
  result["win"]      = (outcome == OUTCOME_VICTORY);
  result["mode"]     = MODE_NAMES[mode];

  QtJson::JsonObject params;
  params["result"] = result;

  QByteArray data = QtJson::serialize(params);
  networkManager.post(request, data);
}

void Tracker::OnNetworkReply(QNetworkReply *reply) {
  if(reply->error() == QNetworkReply::NoError) {
    logger << "Adding of the result was a success!" << endl;
  } else {
    logger << "There was a problem adding the result. Code: " << reply->error() << endl;
    logger << QString(reply->readAll()).toStdString() << endl;
  }
}
