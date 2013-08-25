object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'Command Server'
  ClientHeight = 300
  ClientWidth = 640
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Memo1: TMemo
    Left = 8
    Top = 8
    Width = 625
    Height = 241
    Align = alCustom
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object XML: TXMLDocument
    FileName = 'Options.xml'
    Left = 592
    Top = 8
    DOMVendorDesc = 'MSXML'
  end
  object ServerClient: TTcpServer
    OnAccept = ServerClientAccept
    Left = 104
    Top = 8
  end
  object ServerServer: TTcpServer
    OnAccept = ServerServerAccept
    Left = 24
    Top = 8
  end
  object SQLConnection: TSQLConnection
    ConnectionName = 'MySQLConnection'
    DriverName = 'MySQL'
    GetDriverFunc = 'getSQLDriverMYSQL'
    LibraryName = 'dbxmys.dll'
    Params.Strings = (
      'DriverUnit=Data.DBXMySQL'
      
        'DriverPackageLoader=TDBXDynalinkDriverLoader,DbxCommonDriver160.' +
        'bpl'
      
        'DriverAssemblyLoader=Borland.Data.TDBXDynalinkDriverLoader,Borla' +
        'nd.Data.DbxCommonDriver,Version=16.0.0.0,Culture=neutral,PublicK' +
        'eyToken=91d62ebb5b0d1b1b'
      
        'MetaDataPackageLoader=TDBXMySqlMetaDataCommandFactory,DbxMySQLDr' +
        'iver160.bpl'
      
        'MetaDataAssemblyLoader=Borland.Data.TDBXMySqlMetaDataCommandFact' +
        'ory,Borland.Data.DbxMySQLDriver,Version=16.0.0.0,Culture=neutral' +
        ',PublicKeyToken=91d62ebb5b0d1b1b'
      'GetDriverFunc=getSQLDriverMYSQL'
      'LibraryName=dbxmys.dll'
      'LibraryNameOsx=libsqlmys.dylib'
      'VendorLib=LIBMYSQL.dll'
      'VendorLibWin64=libmysql.dll'
      'VendorLibOsx=libmysqlclient.dylib'
      'MaxBlobSize=-1'
      'DriverName=MySQL'
      'HostName=localhost'
      'Database=ptyxiakh'
      'User_Name=root'
      'ServerCharSet='
      'BlobSize=-1'
      'ErrorResourceFile='
      'LocaleCode=0000'
      'Compressed=False'
      'Encrypted=False'
      'ConnectTimeout=60'
      'Password=6540')
    VendorLib = 'LIBMYSQL.dll'
    Left = 536
    Top = 8
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 480
    Top = 8
  end
  object Base64Dec: TIdDecoderMIME
    FillChar = '='
    Left = 360
    Top = 256
  end
end
