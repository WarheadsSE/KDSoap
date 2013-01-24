/****************************************************************************
** Copyright (C) 2010-2013 Klaralvdalens Datakonsult AB.  All rights reserved.
**
** This file is part of the KD Soap library.
**
** Licensees holding valid commercial KD Soap licenses may use this file in
** accordance with the KD Soap Commercial License Agreement provided with
** the Software.
**
**
** This file may be distributed and/or modified under the terms of the
** GNU Lesser General Public License version 2 and version 3 as published by the
** Free Software Foundation and appearing in the file LICENSE.LGPL.txt included.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Contact info@kdab.com if any conditions of this licensing are not
** clear to you.
**
**********************************************************************/

#include "KDSoapClientInterface.h"
#include "KDSoapMessage.h"
#include "KDSoapValue.h"
#include "KDSoapAuthentication.h"
#include "KDDateTime.h"
#include "wsdl_DWService_12.h"
#include "httpserver_p.h"
#include <QtTest/QtTest>
#include <QDebug>

using namespace KDSoapUnitTestHelpers;

class DWServiceTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testLogin()
    {
        HttpServerThread server(loginResponse(), HttpServerThread::Public);
        KDAB::DWService service;
        service.setSoapVersion(KDSoapClientInterface::SOAP1_2);
        service.setEndPoint(server.endPoint());

        KDAB::TNS__Login loginParams;
        loginParams.setUserName(QString::fromLatin1("Foo"));
        loginParams.setPassword(QString::fromLatin1("Bar"));
        loginParams.setOrganization(QString::fromLatin1("KDAB"));
        const KDAB::TNS__LoginResponse resp = service.login(loginParams);

        // Check what we sent
        QByteArray expectedRequestXml =
            QByteArray(xmlEnvBegin12()) +
            "><soap:Body>"
            "<n1:Login xmlns:n1=\"http://tempuri.org/\">"
                "<n1:userName>Foo</n1:userName>"
                "<n1:password>Bar</n1:password>"
                "<n1:organization>KDAB</n1:organization>"
            "</n1:Login>"
            "</soap:Body>" + xmlEnvEnd()
            + '\n'; // added by QXmlStreamWriter::writeEndDocument
        QVERIFY(xmlBufferCompare(server.receivedData(), expectedRequestXml));

        const KDAB::__ClientServiceSession session = resp.loginResult();
        const KDAB::__Guid sessionId = session.sessionID();
        QCOMPARE(sessionId.value(), QString::fromLatin1("65a65c1f-2613-47d0-89ec-1c7b1fe34777"));
    }

    void testLogoff()
    {
        HttpServerThread server(loginResponse(), HttpServerThread::Public);
        KDAB::DWService service;
        service.setSoapVersion(KDSoapClientInterface::SOAP1_2);
        service.setEndPoint(server.endPoint());

        KDAB::__ClientServiceSession session;
        session.setSessionID(KDAB::__Guid(QString::fromLatin1("65a65c1f-2613-47d0-89ec-1c7b1fe34777")));
        KDAB::TNS__Logoff logoffParams;
        //logoffParams.setClientSession(session);
        service.logoff(logoffParams);

        // Check what we sent
        QByteArray expectedRequestXml =
            QByteArray(xmlEnvBegin12()) +
            "><soap:Body>"
            "<n1:Logoff xmlns:n1=\"http://tempuri.org/\" xsi:nil=\"true\"/>"
            "</soap:Body>" + xmlEnvEnd()
            + '\n'; // added by QXmlStreamWriter::writeEndDocument
        QVERIFY(xmlBufferCompare(server.receivedData(), expectedRequestXml));

    }

private:
    static QByteArray loginResponse()
    {
        return QByteArray(xmlEnvBegin12()) + " xmlns:dw=\"http://schemas.novell.com/2005/01/GroupWise/groupwise.wsdl\"><soap:Body>"
              "<dw:LoginResponse>"
                "<dw:LoginResult><dw:SessionID>65a65c1f-2613-47d0-89ec-1c7b1fe34777</dw:SessionID></dw:LoginResult>"
               "</dw:LoginResponse>"
              "</soap:Body>" + xmlEnvEnd();
    }

};

QTEST_MAIN(DWServiceTest)

#include "test_dwservice_12_wsdl.moc"
