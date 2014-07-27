#ifndef STRINGTOJSON_H
#define STRINGTOJSON_H

#include <QMainWindow>
#include "json.h"

namespace Ui {
class stringToJSON;
}

class stringToJSON : public QMainWindow
{
    Q_OBJECT

public:
    explicit stringToJSON(QWidget *parent = 0);
    JSON json;
    Ui::stringToJSON *ui;
    ~stringToJSON();

public slots:
    QString generateJSONText();

private:
    QString jsonString;

signals:
    void finishedRetrievingTitle(QString);

};

#endif // STRINGTOJSON_H
