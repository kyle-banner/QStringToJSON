#include "json.h"
#include <QNetworkRequest>
#include <QNetworkReply>

JSON::JSON()
{
    retrying301Error = false;
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));  //Setup a QNetworkAccessManager for accessing website data. Notify a slot in the JSON class when it is finished receiving data.
}

void JSON::add(QString currentContent, QString contentType){
    parameterStorageHash.insertMulti(contentType,currentContent);               //This QHash is the one storage place for all of our JSON <key, value, ...> types. Provides faster lookup than QMap.
    if(!typesHolder.contains(contentType)) typesHolder.append(contentType);     //This QString List holds all of the QHash keys.
}

void JSON::empty(){
    parameterStorageHash.clear();                                               //Erases the JSON key/value storage.
    typesHolder.clear();                                                        //Erases the QHash key storage.
}

void JSON::setQLabelToUpdate(QLabel *uiElement){                                //If the user wishes to update a QLabel (as I do) I've provided functionality.
    uiLabel = uiElement;
}

void JSON::updateUI(){
    uiLabel->setText(makeJSONString());
}

bool JSON::findJSONContent(QString stringIn, QString regex_parameter, QString parameter_type, bool includeRegex){
    return findSpecialContent(stringIn, regex_parameter, parameter_type, includeRegex);                //public call to private member, hides method in API
}

bool JSON::findJSONContent(QString stringIn, QString regex_parameter, QString parameter_type, bool includeRegex, int max_characters){
    return findSpecialContent(stringIn, regex_parameter, parameter_type, includeRegex, max_characters); //overloaded public call to private member, hides method in API, provides max_characters
}

QString JSON::makeJSONString(){
    //This is the one method to format our json string. It considers all abstracted <key, value> pairs from
    //QHash and makes special considerations for links, since it is a <key, value, value> triplet.
    QString returnString = "{\n";
    QString linksString = "";
    if(!typesHolder.isEmpty()){
        for(int j=0;j<typesHolder.length();j++){                                //Read through the OHash keys in QStringList typesHolder
            if(typesHolder.at(j)=="links"){                                     //Links are triplets, must be considered individually.
                returnString+="  \"links\": [\n    {\n";
                urlType = parameterStorageHash.values("links");                 //Returns urls in QList<QString>
                for(int i=urlType.length()-1;i>=0;i--){                         //Scan url QList and add to JSON text
                    QString url = urlType.at(i);
                    returnString+="      \"url\": \""+url+"\",\n";
                    if(typesHolder.contains("titles")){
                        titleType = parameterStorageHash.values("titles");
                        if(titleType.length()==urlType.length()){     //If all urls have a title, proceed with populating string with value associated with url key
                            if(i==0) returnString+="      \"title\": \""+parameterStorageHash.value(url)+"\"\n";
                            else returnString+="      \"title\": \""+parameterStorageHash.value(url)+"\",\n";   //If this is the last title, don't include a comma
                        }
                    }
                }
                returnString+="    }\n";
            }
            else if(typesHolder.at(j)=="titles"){                               //If titles are found, ignore titles, as they are part of the Links triplet.
                //ignore
            }
            else if(typesHolder.at(j).contains("http:")||typesHolder.at(j).contains("https:")){ //If urls are found, ignore titles, as they are part of the Links triplet.
                //ignore
            }
            else{
            //Abstracted data types based upon user input.
            QList<QString> valuesOfCurrentType = parameterStorageHash.values(typesHolder.at(j));    //Provides QList of current key (type)
            QString itemType = typesHolder.at(j);
            for(int i=valuesOfCurrentType.length()-1;i>=0;i--){                 //Scan current type
                if(i==valuesOfCurrentType.length()-1){                          //If at the end, do not input comma
                    returnString+="  \""+itemType+"\": [\n";
                }
                QString item = valuesOfCurrentType.at(i);                       //Retrieve value and populate JSON string
                if(i>0){
                    returnString+="    \""+item+"\",\n";
                }
                else{
                    returnString+="    \""+item+"\""+"\n";
                }
            }
            returnString+="  ]\n";
        }
        }
        returnString+="}";
        return returnString;
    }
    else return NULL;
}

bool JSON::findSpecialContent(QString stringIn, QString regex_parameter, QString parameter_type, bool includeRegex){
    bool containsParameter = false;                                             //initialize containsParameter to default false
    int includeRegexInt = 0;                                                    //initialize include regex option to default true
    if(includeRegex) includeRegexInt = 0;
    else includeRegexInt = 1;                                                   //if the user does not want to include the regex string, set regex to 1
    QRegExp specialContent(regex_parameter,Qt::CaseInsensitive);                //initialize regex parameter. Assumed case not sensitive.
    int pos = 0;                                                                //start at the beginning of stringIn
    while ((pos = specialContent.indexIn(stringIn, pos)) != -1) {               //While not at the end of stringIn
        if(parameter_type=="links"){                                            //special considerations have to be made for links since it is a <key, value, value> triplet and not a <key, value> pair
            bool validLink = verifyLinks(specialContent.cap(includeRegexInt));  //verify link structure
            QString linkName = specialContent.cap(includeRegexInt);
            if(validLink){
                findTitleOfValidLinks(linkName);                                //attempt to find the title of the link
                add(linkName, parameter_type);                                  //add the link to the JSON object
            }
        }
        else add(specialContent.cap(includeRegexInt), parameter_type);          //add an abstracted parameter type based upon user input
        containsParameter = true;                                               //the JSON object now contains this abstracted type of parameter
        pos += specialContent.matchedLength();                                  //update the position in stringIn
    }
    return containsParameter;                                                   //return true if content was found, return false if not found
}

bool JSON::findSpecialContent(QString stringIn, QString regex_parameter, QString parameter_type, bool includeRegex, int max_characters){
    //Overloaded function of the above. Provides max_characters option.
    bool containsParameter = false;
    int includeRegexInt = 0;
    if(includeRegex) includeRegexInt = 0;
    else includeRegexInt = 1;
    QRegExp specialContent(regex_parameter,Qt::CaseInsensitive);
    int pos = 0;
    while ((pos = specialContent.indexIn(stringIn, pos)) != -1) {
        if(specialContent.cap(includeRegexInt).length()<=max_characters){ //max_characters option here
            if(parameter_type=="links"){
                bool validLink = verifyLinks(specialContent.cap(includeRegexInt));
                QString linkName = specialContent.cap(includeRegexInt);
                if(validLink){
                    findTitleOfValidLinks(linkName);
                    add(linkName, parameter_type);
                }
            }
            else add(specialContent.cap(includeRegexInt), parameter_type);
            containsParameter = true;
            pos += specialContent.matchedLength();
        }
        else{
            pos += specialContent.matchedLength();
        }
    }
    return containsParameter;
}

bool JSON::verifyLinks(QString urlInput){
    QUrl urlFromInput;      //Initialize QUrl type.
    bool linkValid = false; //Initialize linkValid to default false.
    if(!urlFromInput.fromUserInput(urlInput).toString(QUrl::FullyEncoded).isEmpty()){   //After formatting the urlInput if the QString generated is not empty (format unsuccessful) then set linkValid to true.
        linkValid = true;
    }
    return linkValid;
}

void JSON::findTitleOfValidLinks(QString validLink){
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();       //Initialize QSsl default configuration
    config.setProtocol(QSsl::AnyProtocol);                                      //Accept any protocol

    QNetworkRequest request;
    request.setUrl(QUrl(validLink));                                            //Set the network request to JSON object's validated URL
    request.setHeader(QNetworkRequest::ContentTypeHeader, "HipChat-Browser");   //Establish bare header information
    request.setSslConfiguration(config);                                        //Set config from above
    QNetworkReply *currentReply = manager->get(request);                        //Http GET Url
}

QString JSON::handleResponseErrors(int errorEnum){
    //Handle possible errors received from server when attempting to access title.
    //Below is the enum from QNetworkRequest::RedirectionTargetAttribute
    QString errorResponse = "";
    switch(errorEnum){
        case 1:
            errorResponse = "Remote server refused the connection.";
            break;
        case 2:
            errorResponse = "Remote server closed the connection prematurely.";
            break;
        case 3:
            errorResponse = "Remote host name not found.";
            break;
        case 4:
            errorResponse = "Connection to the remote server timed out.";
            break;
        case 5:
            errorResponse = "The operation was canceled via calls to abort() or close() before it was finished.";
            break;
        case 6:
            errorResponse = "The SSL/TLS handshake failed and the encrypted channel could not be established.";
            break;
        case 7:
            errorResponse = "The connection was broken due to disconnection from the network, however the system has initiated roaming to another access point.";
            break;
        default:
            errorResponse = "Verify http or https protocols. You may have used the wrong one.";
            break;
    }
    return errorResponse+" The title was not found.";
}

void JSON::replyFinished(QNetworkReply *managerReply){
    if(retrying301Error){                                                   //If reattempting 301 error
        QString currentLink = managerReply->url().toString();
        QByteArray data = managerReply->readAll();
        QString allHTML = QString::fromUtf8(data.data(), data.size());
        QWebView qwv;
        qwv.setHtml(allHTML);
        QString newLinkTitle = qwv.title();
        add(newLinkTitle,"titles");
        add(newLinkTitle,currentLink);
        retrying301Error = false;
        updateUI();                                                         //update UI calls makeJSONString() again
    }
    else{
        //Slot that receives the finished() signal from QNetworkAccessManager
        int statusCode = managerReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(); //Retrieve attribute status code from QNetworkReply
        QString currentLink = managerReply->url().toString();               //Retrieve current URL from QNetworkReply
        int response = managerReply->error();                               //Retrieve error status code from QNetworkReply
        QString errorString = managerReply->errorString();                  //Retrieve error string from QNetworkReply
        if(statusCode == 200){                                              //If the status code == 200, everything went OK
            QByteArray data = managerReply->readAll();                      //HTML data in QByteArray
            QString allHTML = QString::fromUtf8(data.data(), data.size());  //Convert QByteArray HTML data to QString
            QWebView qwv;
            qwv.setHtml(allHTML);                                           //QWebView parses HTML data
            QString title = qwv.title();                                    //could use findSpecialItems(allHTML, "<title>([a-zA-Z0-9 ]+)</title>", "title", false); instead, but this way is more direct
            add(title,"titles");                                            //Add to QHash as a title
            add(title,currentLink);                                         //Associate title and URL in QHash
            updateUI();                                                     //Provide asynchronous data to UI
        }
        else if(statusCode == 301){
            //This is the error code thrown when there is an unknown protocol.
            //This error happens frequently so I made my code more robust to check for it.
            bool me = true;
            QList<QString> urls = parameterStorageHash.values("links");
            for(int k=0;k<urls.length();k++){
                if(urls.at(k)==currentLink){
                    tempUrl = urls.at(k);
                    if(tempUrl.contains("https://")){
                        tempUrl.replace("https://","https://www.");         //force www. into link
                    }
                    else if(tempUrl.contains("http://")){
                        tempUrl.replace("http://","http://www.");           //force www. into link
                    }
                    urls.removeAt(k);                                       //Clean up local storage and place http://www. and https://www. in instead
                    urlType.removeAt(k);
                    urls.insert(k,tempUrl);
                    urlType.insert(k,tempUrl);
                }
            }
            parameterStorageHash.remove("links");                           //Remove links storage
            foreach(QString url, urls){
                parameterStorageHash.insertMulti("links",url);              //Repopulate links storage
            }
            retrying301Error = true;                                        //Flag for next time entering class use special procedure
            findTitleOfValidLinks(tempUrl);                                 //Start the process over again
            int y = 0;
        }
        else{
            QString title = errorString+". "+handleResponseErrors(response);//Give the user a full error response
            add(title,"titles");                                            //Add to QHash as a title
            add(title,currentLink);                                         //Associate title and URL in QHash
            updateUI();                                                     //Provide asynchronous data to UI
        }
    }
}
