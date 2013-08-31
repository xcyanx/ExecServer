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
	TMemo *Memo1;
	TTimer *Timer1;
	TIdDecoderMIME *Base64Dec;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall ServerServerAccept(TObject *Sender, TCustomIpClient *ClientSocket);
	void __fastcall ServerClientAccept(TObject *Sender, TCustomIpClient *ClientSocket);
	void __fastcall Timer1Timer(TObject *Sender);


private:	// User declarations
	void __fastcall LoadXML();
	void __fastcall dataReqRoutesServe(TCustomIpClient *ClientSocket);
	void __fastcall dataReqRouteDataServe(TCustomIpClient *ClientSocket);
	void __fastcall dataUpload2ServerServe(TCustomIpClient *ClientSocket);

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

	//std::map<_sessionKey, String, compareClass> map;

public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
