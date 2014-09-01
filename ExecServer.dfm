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
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 640
    Height = 300
    ActivePage = TabSheet1
    Align = alClient
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'Server Log'
      object Memo1: TMemo
        Left = 0
        Top = 0
        Width = 632
        Height = 272
        Align = alClient
        TabOrder = 0
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Server Errors'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ImageIndex = 1
      ParentFont = False
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Memo2: TMemo
        Left = 0
        Top = 0
        Width = 632
        Height = 272
        Align = alClient
        TabOrder = 0
      end
    end
  end
  object XML: TXMLDocument
    FileName = 'Options.xml'
    Left = 592
    Top = 8
    DOMVendorDesc = 'MSXML'
  end
  object ServerClient: TTcpServer
    OnAccept = ServerClientAccept
    Left = 392
    Top = 16
  end
  object ServerServer: TTcpServer
    OnAccept = ServerServerAccept
    Left = 360
    Top = 16
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
  object Base64Enc: TIdEncoderMIME
    FillChar = '='
    Left = 424
    Top = 256
  end
  object SQLQuery1: TSQLQuery
    Params = <>
    Left = 296
    Top = 256
  end
end
