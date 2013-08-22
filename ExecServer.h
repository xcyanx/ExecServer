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
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall ServerServerAccept(TObject *Sender, TCustomIpClient *ClientSocket);
	void __fastcall ServerClientAccept(TObject *Sender, TCustomIpClient *ClientSocket);
	void __fastcall Timer1Timer(TObject *Sender);


private:	// User declarations
	void _fastcall LoadXML();

private:
	/*struct _sessionKey
	{
		_GUID guid;
		time_t timeLeft;
	};

	class compareClass
	{
		public:
			bool operator()(const _sessionKey &L, const _sessionKey &R) const
			{
				_GUID Left = L.guid;
				_GUID Right = R.guid;


				if(Left.Data1 > Right.Data1)
					return true;
				else if(Left.Data1 < Right.Data1)
					return false;

				if(Left.Data2 > Right.Data2)
					return true;
				else if(Left.Data2 < Right.Data2)
					return false;

				if(Left.Data3 > Right.Data3)
					return true;
				else if(Left.Data3 < Right.Data3)
					return false;

				if(Left.Data4 > Right.Data4)
					return true;
				else if(Left.Data4 < Right.Data4)
					return false;

				return true;
        	}
	}; */

	//std::map<_sessionKey, String, compareClass> map;

public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
