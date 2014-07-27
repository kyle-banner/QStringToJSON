#include "stringtojson.h"
#include "ui_stringtojson.h"

#include <QMessageBox>

stringToJSON::stringToJSON(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::stringToJSON)
{
    ui->setupUi(this);
    QSslSocket qsslsock;
    bool currentlySupportsSSL = qsslsock.supportsSsl(); //SSL and openssl are required for many websites. If the computer somehow didn't get the openssl-libs then the app should notify the user.
    if(!currentlySupportsSSL){
        QMessageBox msgBox;
        msgBox.setText("You are missing the openssl-bin libraries. They should be in your present working directory. Some websites may not publish titles until you fix this.");
        msgBox.exec();
    }
    connect(ui->textEntry, SIGNAL(returnPressed()), this, SLOT(generateJSONText()));            //Set up slot for return pressed upon entering chat text
    connect(ui->convertToJSONPushButton, SIGNAL(clicked()), this, SLOT(generateJSONText()));    //Set up slot for alternate button option upon entering chat text
    json.setQLabelToUpdate(ui->JSONText);
}

QString stringToJSON::generateJSONText(){
    json.empty();           //Remove all text from existing or initialized JSON object
    QString JSONString;     //Initialize a string to hold JSON text
    bool containsMentions = json.findJSONContent(ui->textEntry->text(),"\\@+([a-z^\W]+)","mentions",false);          //findJSONContent is a regex parsing method in my JSON class for recognizing JSON variables
    bool containsEmoticons = json.findJSONContent(ui->textEntry->text(),"\\(([^@][a-0]+)\\)","emoticons",false,15);  //findJSONContent returns a bool that tells the API user whether their regex value is present
    bool containsLinks = json.findJSONContent(ui->textEntry->text(),"https?://[^()\"' ]+","links",true);             //the bool is not used here, but could be used in other contexts
//    bool containsHexColors = json.findSpecialContent(ui->textEntry->text(),"#+([a-0]+)","hexColors",true,7);          //un-comment this line for hex color recognition in JSON
    JSONString = json.makeJSONString(); //Make the JSON text for the JSON object. Returns a QString.
    ui->JSONText->setText(JSONString);  //Set the QLabel in the test harness ui to the generated JSON text
    return JSONString;                  //return the QString as asked in the exercise
}

stringToJSON::~stringToJSON()
{
    delete ui;
}
