# Native アプリと .NET アプリを連動するいくつかの方法

高萩 俊行（とっちゃん) Microsoft MVP for Development Technologies  
[Twitter http://twitter.com/tocchann](http://twitter.com/tocchann)  
[Facebook https://www.facebook.com/toshiyuki.takahagi](https://www.facebook.com/toshiyuki.takahagi)  
[blog http://blogs.wankuma.com/tocchann/default.aspx](http://blogs.wankuma.com/tocchann/default.aspx)  
[GitHub https://github.com/tocchann/](https://github.com/tocchann/)

## アジェンダ

* はじめに
* WPF を利用した簡単な例
* CWinFormsView を利用した簡単な例
* パイプを使ったプロセス間通信の例
* おわりに

## はじめに

C++ アプリと .NET Framework の連携機能から比較的単純なものをいくつか紹介します。

* WPF を利用した簡単な例
* CWinFormsView を利用した簡単な例
* パイプを使ったプロセス間通信の例

今回は、MFC アプリから .NET Framework の様々な機能を呼び出すための足掛かりとなるサンプルとしています。

## WPF を利用した簡単な例

仕様

* ホストプロセス側にWPFの参照を持たせない
* クラスオブジェクト呼び出し例も兼ねる
* モーダルダイアログで操作した結果を受け取る処理を入れる
* ウィンドウクラスと外部連動クラスは別に定義する

WPF が公開するクラス

```C#
public class WrapWpf
{
    public string Text { get; private set; }
    public bool CallDlg();
}
```

C++ で呼び出しているコード

```C++
auto wrapWpf = gcnew CollabWpf::WrapWpf();
if( wrapWpf->CallDlg() )
{
    CString msg;
    msg = _T( "Succeeded : " );
    msg += ToString( wrapWpf->Text );
    AfxMessageBox( msg );
}
```

## CWinFormsView を利用した簡単な例

仕様

* MVC スタイルで作る
  * M=CDocument(C++)
  * C=CWinFormsView(C++)
  * V=UserControl(C#)
* View エリアに凝った仕組みを持ってこれる布石とできるようなものにする

WinFormsが公開するクラス

```C#
public class CsView : pulic UserControl
{
    public Button button1;
    public void AddMsg( string msg );
}
```

C++ 側のコード(抜粋)

```CPP
CCollabAppView::CCollabAppView()
: CWinFormsView( CollabLib::CsView::typeid )
{
}
```

```CPP
auto pDoc = GetDocument();
auto pView = dynamic_cast<CollabLib::CsView^>(GetControl());
...
pView->AddMsg( FromString( pDoc->GetPipeMessage() ) );
```

### MFC は互換性を優先するところがある

* MFC の .NET サポートは WinForms のみ
* mfcmifc80.dll に IView などのインターフェース定義がある(インターフェース定義DLL)
  * CWinFormsXxxx を使う場合のメタ定義が含まれているだけなので使わない場合不要
  * WinForms でできる範囲はほとんどが C++ ベースでもできてしまう
* どうせ作るなら WPF 版を作ったほうがずっと効果的

### WPF を張り付けることはできるの？

* [WPF と Win32 の相互運用性](https://docs.microsoft.com/ja-jp/dotnet/framework/wpf/advanced/wpf-and-win32-interoperation)

HWND と WPF ウィンドウの連動(それぞれを子ウィンドウとして貼る)相互運用方法が出ています。この仕組み、残念ながら MFC/ATL にはポーティングされていません(そのまま利用は可能)。

## パイプを使ったプロセス間通信の例

仕様

* .NET で実装したほうが良いという部分をプロセス分離させて処理する事例として用意する。
* 実装難易度は気にしないことにする

C# 側送信部

```C#
CancellationTokenSource cts;
...
var token = cts.Token;
var bytes = encoder.GetBytes( textBox1.Text );
var sendLengths = BitConverter.GetBytes( bytes.Length );
await pipeStream.WriteAsync( sendLengths, 0, sendLengths.Length );
await pipeStream.WriteAsync( bytes, 0, bytes.Length, token );
await pipeStream.FlushAsync( token );
```

C++側受信部

```C++
DWORD sendLength;
DWORD readed;
auto result = pipe.Read( &sendLength, sizeof( sendLength ), readed );
if( !CheckResult( result, readed ) )
{
    break;
}
result = pipe.Read( buffer, sendLength, readed );
if( !CheckResult( result, readed ) )
{
    break;
}
buffer[sendLength/sizeof(wchar_t)] = L'\0';
```

## おわりに

### 既存 C++ アプリの .NET 対応

* .NET オブジェクトの作成は gcnew を使う
* .NET クラスを作る場合は ref class
* .NET オブジェクトはハンドルで持つ(System::Object^)
* 変換かな？と思ったら vcclr.h と msclr\\*.h をチェック

### C++/CLI 新規プロジェクトの種類

* CLR クラス ライブラリ
  * LIB プロジェクトのラップ
  * Shell操作など C++ じゃないと作りにくい機能などのラップ
* CLR コンソール アプリケーション
  * C# より余計なライブラリが必要
  * リリース版の最適化が強力以外のメリットがない
* 空の CLR プロジェクト
  * C++ の Windows Forms アプリはサポートされなくなりました

### C++/CLI 対応時の注意点

* LIB のCLI化はリンク先のプロジェクトもCLIに！
  * CLRオプションはプロジェクトの設定
* DLL の初期化の注意点
  * ローダーロック
    * めったに発生しないだけでNativeDLLにも潜在的に存在する
  * 外部変数のコンストラクタ呼び出しで発生する

### /clr スイッチを使う場合の注意事項

* ランタイム(MFCを含む)はDLLのみ利用可能
  * /MD オプションにする
    * VS2017ではデフォルト
* 文字コードをUNICODEにする
  * VS2017ではデフォルト
* エディットコンティニューをOFFにする
  * VS2017ではデフォルト
* EXEの /clr スイッチの有効化
  * 自分で設定
* TargetVersion を指定する
  * 省略時は v4.0 相当になる
  * 利用する .NET DLL のビルドバージョンに合わせる

### まとめ？

* .NET Core の場合は C# を使います。C++ は使えません。
* UWP アプリの場合は C++/CX を使います。
* Windows 10 API を利用する場合は C++/WinRT を使います。
* MFC の連携機能は、互換性重視
  * C++/CLI と共に登場(.NET 2.0)
  * WPF より前の世代なので当然サポートしていない
* .NET 連携部分は極力疎結合に！
* COM を使うというもっとべたな方法もある

