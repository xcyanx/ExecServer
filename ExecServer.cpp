//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ExecServer.h"
#include "Packet.h"
#include <System.IOUtils.hpp>

#include <boost/regex.hpp>
#include <Data.Cloud.CloudAPI.hpp>
#include "memmem.c"
#include "Utils.h"
#include "base64.h"
#include <string.h>
#include <fstream>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	Application->OnException = AppException;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::AppException(TObject *Sender, Exception *E)
{
  Application->ShowException(E);
  Application->Terminate();
}
//---------------------------------------------------------------------------
std::vector<TForm1::Pair> TForm1::findXMLTag(char *XMLString, char* XMLTag, int XMLSize)
{
	void *found = XMLString;
	int XMLTagLen = strlen(XMLTag);
	std::vector<TForm1::Pair> result;
    TForm1::Pair pair;
	AnsiString XMLTagEnd = "</";
	XMLTagEnd += &XMLTag[1];

	while(found = memmem(found, XMLSize - ((char*)found - XMLString), XMLTag, XMLTagLen))
	{
		if(found == NULL)
			return result;

		found = (char*)found + XMLTagLen;

		pair.start = int((char*)found - XMLString);

		found = memmem(found, XMLSize - ((char*)found - XMLString), XMLTagEnd.c_str(), XMLTagEnd.Length());

		pair.end = int((char*)found - XMLString);

		found = (char*)found + XMLTagEnd.Length();

		result.push_back(pair);
    }

	return result;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::dataReqRoutesServe(TCustomIpClient *ClientSocket)
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

	free(xml);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::dataReqRouteDataServe(TCustomIpClient *ClientSocket)
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

	free(xml);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::dataUpload2ServerServe(TCustomIpClient *ClientSocket)
{
	NextPacketSize npk;
	TXMLDocument *xml;
	AnsiString filename;
	String routeName;
	String uname = map.find(ClientSocket->RemoteHost)->second.username;
	int lastRouteId;


	ThreadLock->Enter();

	ClientSocket->ReceiveBuf(&npk, sizeof(NextPacketSize));

	Memo1->Lines->Add(String("Client ")+uname+String(" is trying to upload a route to the server."));

	char *buffer = (char*)malloc( sizeof(char)*(npk.size+1));
	//char *buffer = new char[npk.size+1];

	memset(buffer, NULL, (npk.size+1)*sizeof(char));

	ClientSocket->ReceiveBuf(buffer, (npk.size)*sizeof(char), MSG_WAITALL);

	std::string final_xml(buffer, npk.size);

	xml = new TXMLDocument(this);


	std::vector<TForm1::Pair> pair = findXMLTag(buffer, "<Image>", npk.size+1);

	//Λάθος επειδή αλλάζει ο vector όταν αλλάζει απο binary σε base64.
	for(int i = pair.size() - 1; i >= 0 ; i--)
	{
		int new_size;
		char* tmp = base64(&buffer[pair[i].start], pair[i].end - pair[i].start, &new_size);

		final_xml = binary_replace(final_xml, pair[i].start, pair[i].end, tmp);

		free(tmp);
	}

	pair = findXMLTag(const_cast<char*>(final_xml.c_str()), "<Video>", final_xml.size());

	for(int i = pair.size() - 1; i >= 0; i--)
	{
		int new_size;
		char* tmp = base64(&buffer[pair[i].start], pair[i].end - pair[i].start, &new_size);

		final_xml = binary_replace(final_xml, pair[i].start, pair[i].end, tmp);

		free(tmp);
	}

	pair = findXMLTag(const_cast<char*>(final_xml.c_str()), "<Text>", final_xml.size());

	for(int i = pair.size() - 1; i >= 0; i--)
	{
		int new_size;
		char* tmp = base64(&buffer[pair[i].start], pair[i].end - pair[i].start, &new_size);

		final_xml = binary_replace(final_xml, pair[i].start, pair[i].end, tmp);

		free(tmp);
	}

	xml->LoadFromXML(AnsiString(final_xml.c_str()));


	_di_IXMLNodeList root = xml->ChildNodes->GetNode("root")->GetChildNodes();



	filename = xml->ChildNodes->GetNode("root")->GetAttribute("filename");
	routeName = filename;

	filename += ".xml";

	//Add the route into the database.
	try{
		int userId;

		String SQLQuery = "SELECT idUsers FROM users WHERE Username = :uname";
		TSQLQuery *SQL = new TSQLQuery(NULL);

		SQL->SQLConnection = SQLConnection;
		SQL->SQL->Text = SQLQuery;

		SQL->ParamByName("uname")->AsAnsiString =  map.find(ClientSocket->RemoteHost)->second.username;

		SQL->Open();

		SQL->First();

		userId = SQL->FieldByName("idUsers")->AsInteger;

		SQL->Close();

		SQLQuery = "INSERT INTO routes(routeName, userID) VALUES(:rtname, :userid)";
		SQL->SQL->Text = SQLQuery;
		SQL->ParamByName("rtname")->AsAnsiString = routeName;
		SQL->ParamByName("userid")->AsInteger = userId;

		SQL->ExecSQL();

		SQLQuery = "SELECT MAX(idRoutes) as lastID FROM routes ";
		SQL->SQL->Text = SQLQuery;
		SQL->Open();
		SQL->First();
		lastRouteId = SQL->FieldByName("lastID")->AsInteger;
		SQL->Close();

		delete SQL;
	}
	catch(Exception &e)
	{
        ShowMessage(e.ToString());
	}
	//The route has been added into the database.

	//extract the info about each point(Lon, Lat), Image, Video, Text
	//place those info in the float like class
	//And then perform a douglas-peckeur run on those points.
	//At last, reconstruct the XML file and store it, while updating the sql server.

	int child_count = root->GetCount();
	for(int i = 0; i < child_count; i++)
	{
		 int point_count;

		 _di_IXMLNodeList point = root->GetNode(i)->GetChildNodes();

		 point_count = point->GetCount();

		 _di_IXMLNode node = point->GetNode("Coords");
		 String Lat, Lon, DateTime;

		 Lat = node->GetChildNodes()->GetNode("Lat")->GetText();
		 Lon = node->GetChildNodes()->GetNode("Lon")->GetText();
		 DateTime = point->GetNode("DateTime")->GetText();

		 try
		 {
			String SQLQuery = "INSERT INTO waypoints(routeID, Lon, Lat, DateTime) VALUES(:rtid, :lon, :lat, :dt)";
			TSQLQuery *SQL = new TSQLQuery(NULL);

			SQL->SQLConnection = SQLConnection;
			SQL->SQL->Text = SQLQuery;


			SQL->ParamByName("rtid")->AsInteger = lastRouteId;
			SQL->ParamByName("lon")->AsAnsiString = Lon;
			SQL->ParamByName("lat")->AsAnsiString = Lat;
			SQL->ParamByName("dt")->AsAnsiString = DateTime;

			SQL->ExecSQL();

			delete SQL;
		 }
		 catch (Exception &e)
		 {
			ShowMessage(e.ToString());
		 }

		 String Data = point->GetNode("Text")->GetText();
		 handleData(Data, TypeOfMedia::Text);

		 Data = point->GetNode("Image")->GetText();
		 handleData(Data, TypeOfMedia::Image);

		 Data = point->GetNode("Video")->GetText();
		 handleData(Data, TypeOfMedia::Video);
	}

	xml->SaveToFile(filename.c_str());

    xml->Active = false;


	free(buffer);
	delete xml;

    buildXML(lastRouteId);

	Memo1->Lines->Add(String("Client ")+uname+String(" has uploaded the route successfully."));

	ThreadLock->Leave();
}

//---------------------------------------------------------------------------
void __fastcall TForm1::buildXML(int routeId)
{
try
{
	String SQLQuery = "SELECT * FROM waypoints LEFT OUTER JOIN media ON waypoints.idWaypoints =  media.WaypointID "
					  "WHERE routeId = :rtid";
	TSQLQuery *SQL = new TSQLQuery(NULL);
	int rowsAffected;

	SQL->SQLConnection = SQLConnection;
	SQL->SQL->Text = SQLQuery;
	SQL->ParamByName("rtid")->AsInteger = routeId;
	SQL->Open();
	rowsAffected = SQL->RowsAffected;
	SQL->First();

	TXMLDocument *xml = new TXMLDocument(this);
	_di_IXMLNode node, root;

	xml->Options = TXMLDocOptions(2);
	xml->Active = true;

	root = xml->AddChild("locations");

	String Lat, Lon;
	for(int i = 0; i < rowsAffected; i++)
	{
		String media;
		int mediaId;

		if(Lat != SQL->FieldByName("Lat")->AsAnsiString && Lon != SQL->FieldByName("Lon")->AsAnsiString)
		{
			node = root->AddChild("point");

			node->AddChild("Lat")->SetText(SQL->FieldByName("Lat")->AsAnsiString);
			node->AddChild("Lon")->SetText(SQL->FieldByName("Lon")->AsAnsiString);

			Lat = SQL->FieldByName("Lat")->AsAnsiString;
			Lon = SQL->FieldByName("Lon")->AsAnsiString;
		}

		if(!SQL->FieldByName("idMedia")->IsNull)
		{
			int tom;
			mediaId = SQL->FieldByName("idMedia")->AsInteger;

			tom = SQL->FieldByName("TypeOfMedia")->AsInteger;

			if(tom == TypeOfMedia::Text)
			{
				media = "TextID";
			}
			else if(tom == TypeOfMedia::Image)
			{
				media = "ImgID";
			}
			else
			{
				media = "VidID";
			}

			node->AddChild(media)->SetText(IntToStr(mediaId));
		}

		SQL->Next();
	}

	SQL->Close();

	xml->SaveToFile(IntToStr(routeId));
	xml->Active = false;
	delete xml;
	delete SQL;
}
catch(Exception &e)
{
    ShowMessage(e.ToString());
}
}

//---------------------------------------------------------------------------
int __fastcall TForm1::handleData(String Data, int tom)
{
	if(Data == "")
		return -1;

	int lastWpId;
	String SQLQuery = "INSERT INTO media(URLPath, WaypointID, TypeOfMedia) VALUES(:url, :wpid, :tom)";
	TSQLQuery *SQL = new TSQLQuery(NULL);
	TFileStream *stream = NULL;

	SQL->SQLConnection = SQLConnection;
	SQL->SQL->Text = SQLQuery;

	switch(tom)
	{
		case TypeOfMedia::Text:
		{
			SQL->ParamByName("tom")->AsAnsiString = TypeOfMedia::Text;
			SQL->ParamByName("url")->AsAnsiString = Base64Dec->DecodeString(Data);
			break;
		}
		case TypeOfMedia::Image:
		{
			stream = new TFileStream("testfile.jpg", fmCreate);
		}
		case TypeOfMedia::Video:
		{
			if(stream == NULL)
				stream = new TFileStream("testfile.m4v", fmCreate);

			Base64Dec->DecodeBegin(stream);
			Base64Dec->DecodeStream(Data, stream);
			Base64Dec->DecodeEnd();

			SQL->ParamByName("tom")->AsAnsiString = tom;
			SQL->ParamByName("url")->AsAnsiString = stream->FileName; //path

			delete stream;
			break;
		}
	}

	lastWpId = lastId("idWaypoints", "waypoints");

	SQL->ParamByName("wpid")->AsAnsiString = IntToStr(lastWpId);


	SQL->ExecSQL();

	delete SQL;

	return lastId("idMedia", "media");
}

//---------------------------------------------------------------------------
int __fastcall TForm1::lastId(String fieldName, String tableName)
{
	int result;
	String SQLQuery = String("SELECT MAX(")+fieldName+String(") as last FROM ")+tableName;
	TSQLQuery *SQL = new TSQLQuery(NULL);

	SQL->SQLConnection = SQLConnection;
	SQL->SQL->Text = SQLQuery;

	SQL->Open();
	SQL->First();

	result = SQL->FieldByName("last")->AsInteger;

	SQL->Close();

	delete SQL;

	return result;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::LoadXML()
{
	path = ExtractFilePath(Application->ExeName);

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

	XML->Active = false;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
	Memo1->Lines->Add("Initiating Server.");

	ThreadLock = new TCriticalSection();

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

	//String ipport = ClientSocket->RemoteHost+String(":")+ClientSocket->RemotePort;

	if(!ClientSocket->WaitForData(3000))
	{
		Memo1->Lines->Add("Error on receiving data from the client.");

		return;
	}

	CoInitialize(NULL);

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

	CoUninitialize();
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


