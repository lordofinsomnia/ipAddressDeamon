/*
* Copyright 2012 Srdjan Milicev (lordofinsomnia)
*
* This file may be used under the terms of of the
* GNU General Public License Version 2 or later (the "GPL"),
* http://www.gnu.org/licenses/gpl.html
*
* Software distributed under the License is distributed on an "AS IS" basis,
* WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
* for the specific language governing rights and limitations under the
* License.
*/

#include <QCoreApplication>
#include <QtNetwork>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include "other/smtpclient.h"
#include "other/mimetext.h"

struct TAppSettings
{
    QString smtp;
    int     smtpPort;
    QString username;
    QString password;
    QString senderMail;
    QString senderName;
    QString reciverMail;
    QString reciverName;
    QString message;
    QString subject;

    bool    monitorIPv4;
    bool    monitorIPv6;
};

void saveConfiguration(QList<QString> xChangedIPAddresses)
{
    QSettings settings("ipAddressDeamon");
    settings.beginWriteArray("ipAddresses");

    for(int i=0; i<xChangedIPAddresses.count();i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("ipAddress",xChangedIPAddresses[i]);
    }
    settings.endArray();
}
bool loadConfiguration(QList<QString> &xIpAddresses,
                       TAppSettings  &xAppSettings)
{
    QSettings settings("ipAddressDeamon");
    bool res;
    xIpAddresses.clear();
    int cnt=settings.beginReadArray("ipAddresses");
    for(int i=0; i<cnt; i++)
    {
        settings.setArrayIndex(i);
        xIpAddresses.push_back(settings.value("ipAddress").toString());
    }
    settings.endArray();
    xAppSettings.monitorIPv4   =settings.value("appSettings/monitorIPv4"   ).toBool  ();
    xAppSettings.monitorIPv6   =settings.value("appSettings/monitorIPv6"   ).toBool  ();

    xAppSettings.smtp          =settings.value("mailSettings/smtp"         ).toString();
    xAppSettings.smtpPort      =settings.value("mailSettings/smtpPort"     ).toInt   ();
    xAppSettings.username      =settings.value("mailSettings/username"     ).toString();
    xAppSettings.password      =settings.value("mailSettings/password"     ).toString();
    xAppSettings.senderMail    =settings.value("mailSettings/senderMail"   ).toString();
    xAppSettings.senderName    =settings.value("mailSettings/senderName"   ).toString();
    xAppSettings.reciverMail   =settings.value("mailSettings/reciverMail"  ).toString();
    xAppSettings.reciverName   =settings.value("mailSettings/reciverName"  ).toString();
    xAppSettings.message       =settings.value("mailSettings/message"      ).toString();
    xAppSettings.subject       =settings.value("mailSettings/subject"      ).toString();

    res=(xAppSettings.monitorIPv4||xAppSettings.monitorIPv6)&&
        (xAppSettings.smtp          !="")&&
        (xAppSettings.smtpPort      !=0 )&&
        (xAppSettings.username      !="")&&
        (xAppSettings.password      !="")&&
        (xAppSettings.senderMail    !="")&&
        (xAppSettings.reciverMail   !="");
    return res;
}

QList<QString> findChangedIPs(QList<QString> xSavedIPs,
                              TAppSettings   xAppSettings)
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    QHostAddress address;
    bool addAddress;
    QList<QString> changedIPs;
    for (int i=0, ipCnt=0; i < list.count(); i++)
    {
        address=list[i];
        if(address!=QHostAddress::Null)
            if(address!=QHostAddress::LocalHost)
                if(address!=QHostAddress::LocalHostIPv6)
                    if(address!=QHostAddress::Broadcast)
                        if(address.protocol()!=QHostAddress::Broadcast)
                        {
                            addAddress=false;
                            if(xAppSettings.monitorIPv4)
                                if(address.protocol()==QAbstractSocket::IPv4Protocol)
                                    addAddress=true;

                            if(xAppSettings.monitorIPv6)
                                if(address.protocol()==QAbstractSocket::IPv6Protocol)
                                    addAddress=true;
                            if (addAddress)
                                if (xSavedIPs.indexOf(address.toString())==-1)
                                {
                                    changedIPs.push_back(address.toString());
                                }
                        }
    }
    return changedIPs;
}

QString packIPs(QList<QString> xIPs)
{
    QString res="";
    QString num;
    for(int i=0;i<xIPs.count();i++)
    {
        if(xIPs.count()>1)
            num=num.number(i+1)+". ";
        res+=(num+xIPs[i]);
        if(i!=xIPs.count()-1)
            res+="\n";

    }
    return res;
}

bool sendMail(QString      xBody,
              TAppSettings xAppSettings)
{
    bool res=false;
    SmtpClient *mail = new SmtpClient(xAppSettings.smtp,
                                      xAppSettings.smtpPort,
                                      SmtpClient::TcpConnection);
    mail->setUser    (xAppSettings.username);
    mail->setPassword(xAppSettings.password);

    MimeMessage msg;
    msg.setSender(new EmailAddress(xAppSettings.senderMail, xAppSettings.senderName));
    msg.addRecipient(new EmailAddress(xAppSettings.reciverMail, xAppSettings.reciverName));
    msg.setSubject(xAppSettings.subject);

    MimeText text;

    text.setText(xBody);
    msg.addPart(&text);

    if (mail->connectToHost())
        if(mail->login())
            if( mail->sendMail(msg) )
            {
                mail->quit();
                res=true;
            }
    return res;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TAppSettings appSettings;

    QList<QString> savedIPs;
    QList<QString> changedIPs;
    QString body;
    QString packedIPs;

    if(loadConfiguration(savedIPs, appSettings))
        changedIPs=findChangedIPs(savedIPs, appSettings);

    if (changedIPs.count()>0)
    {
        body="";
        if (appSettings.message!="")
            body=appSettings.message+"\n";
        body+=packIPs(changedIPs);
        if (sendMail(body, appSettings))
            saveConfiguration(changedIPs);
    }
}
