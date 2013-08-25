//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ExecServer.h"
#include "Packet.h"

#include <boost/regex.hpp>
#include <Data.Cloud.CloudAPI.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void _fastcall TForm1::dataReqRoutesServe(TCustomIpClient *ClientSocket)
{
	char *test_xml = "<ROUTES><NAME RID=\"1\">TEST</NAME><NAME RID=\"2\">TEST2</NAME></ROUTES>";
	//Build the routes xml.
	String ipPort = ClientSocket->RemoteHost;
	BasicPacket packet;

	Memo1->Lines->Add(String("Client ")+map.find(ipPort)->second.username+String(" requested the routes."));

	ClientSocket->ReceiveBuf(&packet, sizeof(BasicPacket));

	NextPacketSize packetSize;

	packetSize.PacketID = BasicPacket::DATA_PACKETSIZE;
	packetSize.size = strlen(test_xml);

	ClientSocket->SendBuf(&packetSize, sizeof(NextPacketSize));

			//Remove it.
	Sleep(100);

	XMLPacket *xml;

	xml = (XMLPacket*)malloc(sizeof(XMLPacket)+strlen(test_xml));

	xml->PacketID = BasicPacket::DATA_XMLDATA;

	memcpy(xml->xmlData, test_xml, strlen(test_xml));

	ClientSocket->SendBuf(xml, sizeof(XMLPacket)+strlen(test_xml));
}
//---------------------------------------------------------------------------
void _fastcall TForm1::dataReqRouteDataServe(TCustomIpClient *ClientSocket)
{
	char *test_xml = "<ROUTES><NAME RID=\"1\">TEST</NAME><NAME RID=\"2\">TEST2</NAME></ROUTES>";
	RRouteData test;
	String ipPort = ClientSocket->RemoteHost;

	ClientSocket->ReceiveBuf(&test, sizeof(RRouteData));

	Memo1->Lines->Add(String("Client ")+map.find(ipPort)->second.username+String(" Requesting route data for route ")+String(test.route_name));

	NextPacketSize npk;

	npk.PacketID = BasicPacket::DATA_PACKETSIZE;
	npk.size = 10;

	ClientSocket->SendBuf(&npk, sizeof(NextPacketSize));

	Sleep(100);

	XMLPacket *xml;

	xml = (XMLPacket*)malloc(sizeof(XMLPacket)+strlen(test_xml));

	xml->PacketID = BasicPacket::DATA_ROUTEDATA;

	memcpy(xml->xmlData, test_xml, strlen(test_xml));

	//Memo1->Lines->Add(test.route_name);

	ClientSocket->SendBuf(xml, sizeof(XMLPacket)+strlen(test_xml));
}
//---------------------------------------------------------------------------
void _fastcall TForm1::dataUpload2ServerServe(TCustomIpClient *ClientSocket)
{
	NextPacketSize npk;

	ClientSocket->ReceiveBuf(&npk, sizeof(NextPacketSize));

	Memo1->Lines->Add(String("Client ")+map.find(ClientSocket->RemoteHost)->second.username+String(" is trying to upload a route to the server."));
	Memo1->Lines->Add("Size: "+IntToStr(npk.size));

	char *buffer = new char[npk.size+1];

	memset(buffer, NULL, (npk.size+1)*sizeof(char));

	ClientSocket->ReceiveBuf(buffer, (npk.size)*sizeof(char));

	//Memo1->Lines->Add(TIdTextEncoding::ASCII->GetString(DecodeBytes64(String(buffer))));

	ShowMessage(buffer);

	//Memo1->Lines->Add(buffer);

	Memo1->Lines->Add(IntToStr((int)strlen(buffer)));

	//Memo1->Lines->Add(TEncoding::GetString(DecodeBytes64(String(buffer))));
}
//---------------------------------------------------------------------------
void _fastcall TForm1::LoadXML()
{
	String path = ExtractFilePath(Application->ExeName);

	XML->FileName = path+(String)"Options.xml";
	XML->Active = true;

	_di_IXMLNodeList root = XML->ChildNodes->GetNode("Options")->GetChildNodes();

	ServerServer->LocalHost = root->GetNode("ServerToServer")->GetChildNodes()->GetNode("Host")->Text;
	ServerServer->LocalPort = StrToInt(root->GetNode("ServerToServer")->GetChildNodes()->GetNode("Port")->Text);

	ServerClient->LocalHost = root->GetNode("ServerToClient")->GetChildNodes()->GetNode("Host")->Text;
	ServerClient->LocalPort = StrToInt(root->GetNode("ServerToClient")->GetChildNodes()->GetNode("Port")->Text);

	String uname = root->GetNode("DB")->GetChildNodes()->GetNode("uname")->Text,
		   host = root->GetNode("DB")->GetChildNodes()->GetNode("Host")->Text,
		   pass = root->GetNode("DB")->GetChildNodes()->GetNode("pass")->Text;

	SQLConnection->Params->Add("User_Name="+uname);
	SQLConnection->Params->Add("HostName="+host);
	SQLConnection->Params->Add("Password="+pass);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
	Memo1->Lines->Add("Initiating Server.");

	Memo1->Lines->Add("Loading the options file.");

	LoadXML();

	Memo1->Lines->Add("Initiating the socket for the Server to Server communication.");

	ServerServer->Active = true;

	Memo1->Lines->Add("Initiating the socket for the Server to Client communication.");

	ServerClient->Active = true;

	Memo1->Lines->Add("Initiating the connection to the database.");

	SQLConnection->Connected = true;

	Memo1->Lines->Add("Server initiation completed.");

	Memo1->Lines->Add("");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ServerServerAccept(TObject *Sender, TCustomIpClient *ClientSocket)

{
	//TGUID guid;
	//_sessionKey key;

	std::string toReceive;
	std::list<std::string> l;
	_sessionKey key;

	//Memo1->Lines->Add("GUID sent from the login Server.");

	if(ClientSocket->WaitForData(3000) == false)
	{
		Memo1->Lines->Add("Failed to read data.");

		return;
    }

	toReceive = ClientSocket->Receiveln().c_str();

	boost::regex_split(std::back_inserter(l), toReceive);

	std::string ipPort = *(l.begin());

	l.pop_front();

	std::string uname = *(l.begin());

	key.username = uname.c_str();
	key.timeLeft = time(NULL);

	map[ipPort.c_str()] = key;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ServerClientAccept(TObject *Sender, TCustomIpClient *ClientSocket)

{
	BasicPacket packet;
	//_sessionKey key;

	String ipport = ClientSocket->RemoteHost+String(":")+ClientSocket->RemotePort;

	if(!ClientSocket->WaitForData(3000))
	{
		Memo1->Lines->Add("Error on receiving data from the client.");

		return;
    }

	ClientSocket->PeekBuf(&packet, sizeof(BasicPacket));

	switch(packet.PacketID)
	{
		case BasicPacket::DATA_REQUESTROUTES:
		{
			  dataReqRoutesServe(ClientSocket);

			  break;
		}
		case BasicPacket::DATA_REQUESTROUTEDATA:
		{
			  dataReqRouteDataServe(ClientSocket);

			  break;
		}
		case BasicPacket::DATA_UPLOAD:
		{
			dataUpload2ServerServe(ClientSocket);

			break;
        }
		default:
		{
            Memo1->Lines->Add("Unknown PacketID: "+IntToStr(packet.PacketID));
        }
	}  //switch end
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
	const static day = 60*60*24;

	std::map<String, _sessionKey>::iterator it;

	for(it = map.begin(); it != map.end(); it++)
	{
		_sessionKey *key;

		key = const_cast<_sessionKey*>(&(it->second));

		if(difftime(time(NULL), key->timeLeft) >= day)
		{
        	Memo1->Lines->Add(String("Removed ")+it->second.username+String(" due to inactivity"));

			map.erase(it);
		}
	}
}
//---------------------------------------------------------------------------

