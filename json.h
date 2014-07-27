#ifndef JSON_H
#define JSON_H

#include <QString>
#include <QStringList>
#include <QHash>
#include <QSslSocket>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QWebView>
#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>

class JSON : public QObject
{
    Q_OBJECT
public:
    JSON();
    void add(QString currentContent, QString contentType);
    void empty();
    QString makeJSONString();
    bool verifyLinks(QString urlInput);
    void findTitleOfValidLinks(QString validLink);
    void setQLabelToUpdate(QLabel *uiLabel);
    void setUIElementToUpdate(QTextEdit *uiTextEdit);
    bool findJSONContent(QString stringIn, QString regex_parameter, QString parameter_type, bool includeRegex);
    bool findJSONContent(QString stringIn, QString regex_parameter, QString parameter_type, bool includeRegex, int max_characters);

private:
    QStringList typesHolder;
    bool titleTypePresent;
    QNetworkAccessManager *manager;
    QList<QString> urlType, titleType;
    QMainWindow *ui;
    QLabel *uiLabel;
    QHash<QString, QString> parameterStorageHash;
    void updateUI();
    bool findSpecialContent(QString stringIn, QString regex_parameter, QString parameter_type, bool includeRegex);
    bool findSpecialContent(QString stringIn, QString regex_parameter, QString parameter_type, bool includeRegex, int max_characters);
    QString handleResponseErrors(int errorEnum);
    bool retrying301Error;
    QString tempUrl;

public slots:
    void replyFinished(QNetworkReply* managerReply);
};

#endif // JSON_H
