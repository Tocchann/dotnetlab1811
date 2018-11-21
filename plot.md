# Native アプリと .NET アプリを連動するいくつかの方法

---

高萩 俊行（とっちゃん)  
Microsoft MVP for Development Technologies  
Twitter <http://twitter.com/tocchann>  
Facebook <https://www.facebook.com/toshiyuki.takahagi>  
blog <http://blogs.wankuma.com/tocchann/default.aspx>  
GitHub <https://github.com/tocchann/>

---

## 参考URL

* この資料は以下のリンクから参照できます。
  * <https://github.com/tocchann/dotnetlab1811/>
* リファレンス：C++/CLI (Visual C++) による .NET プログラミング
  * <https://docs.microsoft.com/ja-jp/cpp/dotnet/>

## アジェンダ

* C++ プログラムと .NET プログラムの連携パターン
* C++/CLI に関連するプロジェクトの種類
* .NET オブジェクトとの連携
* 本当の意味で考えるべきこと

### C++ プログラムと .NET プログラムの連携パターン

#### 互いに別プロセスで呼び出す

* プロセス間通信(IPC)を使うことで実現
  * 既存プログラムの変更範囲は IPC 機能の追加だけ
  * IPCを選べばマシン間の連携も可能
* プロセス割り当て分のメモリをすべて利用可能

#### .exe が .NET Framework 製

* よく目にする 一部に C/C++ を使いたいパターン
* プラットフォーム呼び出し(P/Invoke)する
  * API 呼び出しと同じように呼び出す
* C++/CLI のDLLを作成する
  * 呼び出したいのがLIBの場合
  * APIの直接呼出しが難しい場合

#### .exe が C++ プログラム製

* .NET を独自にホスティングする
  * インストールされているランタイムを動的判断してホスト可能
  * 既存環境をいじれないインストーラ専用
* /clr オプションをつける
  * 既存アプリに部分的な .NET 機能を取り込む
  * MFCアプリなら、CView 部分を Forms.Control に差し替え可能
    * CHtmlView, CFormView ライクな実装。
    * CListView 的な実装も可能
  * ポップアップウィンドウならWPFなども直接利用可能

### C++/CLI に関連するプロジェクトの種類

* 既存アプリに /clr オプションを追加
  * すでにあるアプリに .NET の機能を追加したいならこれ！
* CLR クラス ライブラリ
  * LIBのラップDLLを新たに作るなどの場合に選択
* 一応あるほかの選択肢
  * CLR コンソール アプリケーション
  * 空の CLR プロジェクト

#### 既存 C++ アプリの .NET 対応

* C++/CLI の機能も普通に使える
  * オブジェクトの作成は gcnew を使う
  * 文字列の受け渡しも昔よりはずっと簡単に！
* MFC アプリなら
  * 子ウィンドウとしてUIを張り付け可能

#### C++/CLI で注意しておくべき点

* デフォルトスレッドモデルはMTA
  * メイン関数が CLI の場合の話
  * COM の多くは STA 前提
  * Native のメイン関数の場合COMは初期化されない
* LIB のCLI化はリンク先のプロジェクトもCLIに！
  * CLRオプションはプロジェクトの設定
* DLL の初期化の注意点
  * ローダーロック
    * めったに発生しないだけでNativeDLLにも潜在的に存在する
  * 外部変数のコンストラクタ呼び出しで発生する

#### /clr スイッチを使う場合の注意事項

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

### .NET オブジェクトとの連携

#### クラスライブラリの呼び出し

* 通常のコンポーネントとして呼び出し可能
  * メソッド呼び出し、プロパティ呼び出しもそのまま可能
  * 何も足さない。何もひかない。
* 文字列は互換性がないのでコンバートが必要
  * msclr/marshal*.h の msclr::interop::marshal_as\<T> を使う

#### 画面コンポーネントのサポート

* MFC 側のサポートは3種類
  * 指定できるクラスは System.Windows.Forms.Control の派生クラス
    * WPF を張り付ける場合、ElementHost 派生クラスを経由
  * CWinFormsView
    * CFormView/CHtmlView ライクな使い方が可能
    * サンプルで利用
    * DataGridViewなどを指定すれば CListView ライクに
  * CWinFormsDialog\<TManagedControl>
    * CDHtmlDialogライクな使い方が可能
  * CWinFormsControl\<TManagedControl>
    * CWinFormsViewの下請け実装
    * コントロールとして直接貼り付ける場合に利用

### サンプルプロジェクトの構造

* CollabApp
  * Managed/Unmanaged 混在のEXE
  * CMainFrame/CCollabAppDoc は、.NET を使っていないのでNativeのまま
  * 独自の .NET アクセス部分は CollabAppView に局所化
* CollabLib
  * Windows Forms プロジェクト
  * View 用のユーザーコントロールを定義
* CollabWpf
  * WPF プロジェクト
  * ダイアログ表示代わりに使うWPFウィンドウを定義
  * 呼び出し部分をあえて分離するための間接参照クラスを用意
* PipeClientApp
  * 名前付きパイプによるプロセス間通信アプリサンプル

### 本当の意味で考えるべきこと

* MFC アプリからの脱却
* Windows 10 エコシステム
* .NET Core
* C++/WinRT
