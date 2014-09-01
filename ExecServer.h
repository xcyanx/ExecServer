//---------------------------------------------------------------------------

#ifndef ExecServerH
#define ExecServerH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Web.Win.Sockets.hpp>
#include <Xml.Win.msxmldom.hpp>
#include <Xml.XMLDoc.hpp>
#include <Xml.xmldom.hpp>
#include <Xml.XMLIntf.hpp>
#include <Data.DB.hpp>
#include <Data.SqlExpr.hpp>
#include <Data.DBXMySQL.hpp>
#include <Vcl.ExtCtrls.hpp>

//#define _HAS_STRICT_CONFORMANCE

#include <time.h>
#include <IdBaseComponent.hpp>
#include <IdCoder.hpp>
#include <IdCoder3to4.hpp>
#include <IdCoderMIME.hpp>
#include <Xml.adomxmldom.hpp>
#include <Data.FMTBcd.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Data.Bind.Components.hpp>
#include <Data.Bind.EngExt.hpp>
#include <System.Bindings.Outputs.hpp>
#include <System.Rtti.hpp>
#include <Vcl.Bind.DBEngExt.hpp>
#include <Vcl.Bind.Editors.hpp>
#include <map>
#include <vector>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TXMLDocument *XML;
	TTcpServer *ServerClient;
	TTcpServer *ServerServer;
	TSQLConnection *SQLConnection;
	TTimer *Timer1;
	TIdDecoderMIME *Base64Dec;
	TIdEncoderMIME *Base64Enc;
	TSQLQuery *SQLQuery1;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TMemo *Memo1;
	TMemo *Memo2;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall ServerServerAccept(TObject *Sender, TCustomIpClient *ClientSocket);
	void __fastcall ServerClientAccept(TObject *Sender, TCustomIpClient *ClientSocket);
	void __fastcall Timer1Timer(TObject *Sender);

private:
	struct Pair
	{
		unsigned long start;
		unsigned long end;
	};

	enum TypeOfMedia { Text = 1, Video, Image };

private:	// User declarations
	void __fastcall LoadXML();
	void __fastcall dataReqRoutesServe(TCustomIpClient *ClientSocket);
	void __fastcall dataReqRouteDataServe(TCustomIpClient *ClientSocket);
	void __fastcall dataUpload2ServerServe(TCustomIpClient *ClientSocket);
	void __fastcall AppException(TObject *Sender, Exception *E);
	int __fastcall lastId(String fieldName, String tableName);
	int __fastcall handleData(String Data, int tom);
	void __fastcall buildXML(int routeId);
	void __fastcall buildRouteList(int routeId);
	void __fastcall updateRouteList(int routeId);
	std::vector<Pair> findXMLTag(char *XMLString, char* XMLTag, int XMLSize);

private:
	struct _sessionKey
	{
		String username;
		time_t timeLeft;
	};

	/*class compareClass
	{
		public:
			bool operator()(const String &L, const String &R) const
			{
				if(L >= R)
					return true;
				else
					return false;
			}
	};*/

	std::map<String, _sessionKey> map;
	UnicodeString path;
	TCriticalSection *ThreadLock;

	//std::map<_sessionKey, String, compareClass> map;

public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
