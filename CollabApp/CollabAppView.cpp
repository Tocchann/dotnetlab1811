
// CollabAppView.cpp : CCollabAppView クラスの実装
//

#include "stdafx.h"
// SHARED_HANDLERS は、プレビュー、縮小版、および検索フィルター ハンドラーを実装している ATL プロジェクトで定義でき、
// そのプロジェクトとのドキュメント コードの共有を可能にします。
#ifndef SHARED_HANDLERS
#include "CollabApp.h"
#endif

#include "CollabAppDoc.h"
#include "CollabAppView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCollabAppView

IMPLEMENT_DYNCREATE(CCollabAppView, CParentView )

BEGIN_MESSAGE_MAP(CCollabAppView, CParentView )
	// 標準印刷コマンド
	ON_COMMAND(ID_FILE_PRINT, &CParentView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CParentView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CParentView::OnFilePrintPreview)
	ON_COMMAND( IDM_TEST, &CCollabAppView::OnTest )
END_MESSAGE_MAP()


// CCollabAppView コンストラクション/デストラクション

CCollabAppView::CCollabAppView() noexcept
#ifdef USE_FORMS_VIEW
: CParentView( CollabLib::CsView::typeid )
#endif
{
	// TODO: 構築コードをここに追加します。

}

CCollabAppView::~CCollabAppView()
{
}

BOOL CCollabAppView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

	return CParentView::PreCreateWindow(cs);
}

// CCollabAppView 描画

void CCollabAppView::OnDraw(CDC* pDC)
{
	CCollabAppDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect	rc;
	GetClientRect( &rc );
	CString msg( _T( "C++で直接描画。昔ながらのセンタリング" ) );
	CSize sizeText = pDC->GetTextExtent( msg );
	pDC->TextOut( rc.left+(rc.Width()-sizeText.cx)/2, rc.top + (rc.Height()-sizeText.cy)/2, msg );
}


// CCollabAppView の印刷

BOOL CCollabAppView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 既定の印刷準備
	return DoPreparePrinting(pInfo);
}

void CCollabAppView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷前の特別な初期化処理を追加してください。
}

void CCollabAppView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷後の後処理を追加してください。
}


// CCollabAppView の診断

#ifdef _DEBUG
void CCollabAppView::AssertValid() const
{
	CParentView::AssertValid();
}

void CCollabAppView::Dump(CDumpContext& dc) const
{
	CParentView::Dump(dc);
}

CCollabAppDoc* CCollabAppView::GetDocument() const // デバッグ以外のバージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCollabAppDoc)));
	return (CCollabAppDoc*)m_pDocument;
}
#endif //_DEBUG


// CCollabAppView メッセージ ハンドラー
void CCollabAppView::OnInitialUpdate()
{
	//	CWinFormsView の OnInitialUpdateは呼び出さないようにして、C++側のサイズを強制させないように修正
	//	UserControlは、ScrollableControl の派生クラスなのでAutoScrollMinSizeを設定してしまうので設定しないように修正。
	//	そういう制御は、View本体にやらせるほうがいい。
	//	詳細は、 src\mfcm\wfrmview.cpp を参照
#ifdef USE_FORMS_VIEW
	auto pCtrl = GetControl();	//	System::Windows::Forms::Control^ pCtrl = *this;
	auto pIView = dynamic_cast<Microsoft::VisualC::MFC::IView^>(pCtrl);
	if( pIView != nullptr )
	{
		pIView->OnInitialUpdate();
	}
	CView::OnInitialUpdate();

	//	ボタンのクリックイベントを受け取る
	auto pView = dynamic_cast<CollabLib::CsView^>(pCtrl);
	pView->button1->Click += MAKE_DELEGATE( System::EventHandler, OnClick );
	//	パイプ待機スレッドを起動する
	//	GetDocument()->CreatePipeThread();
	pView->AddMsg( L"ボタンを押すと通信します。押しちゃだめなんだよ？" );
#else
	CParentView::OnInitialUpdate();
#endif
}
#include <msclr\marshal_atl.h>
//#include <vcclr.h>
inline System::String^ FromString( const TCHAR* const& _from_object )
{
	return msclr::interop::marshal_as<System::String^>( _from_object );
//	return gcnew System::String(_from_object);
}
inline CString ToString( System::String^ const& _from_object )
{
	return msclr::interop::marshal_as<CString>( _from_object );
	//cli::pin_ptr<const TCHAR> _pinned_ptr = PtrToStringChars( _from_object );
	//return CString( static_cast<const TCHAR *>(_pinned_ptr), _from_object->Length );
}
#ifdef USE_FORMS_VIEW
void CCollabAppView::OnClick( System::Object^ sender, System::EventArgs^ e )
{
	GetDocument()->ExecuteOutProcessWork();
}
#endif
void CCollabAppView::OnTest()
{
	auto wrapWpf = gcnew CollabWpf::WrapWpf();
	if( wrapWpf->CallDlg( System::IntPtr( GetParentFrame()->GetSafeHwnd() ) ) )
	//if( wrapWpf->CallDlg( System::IntPtr::Zero ) )
	{
		CString msg;
		msg = _T( "Result : " );
		msg += ToString( wrapWpf->Text );
		AfxMessageBox( msg );
	}
}
void CCollabAppView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
{
	CParentView::OnUpdate( pSender, lHint, pHint );
#ifdef USE_FORMS_VIEW
	//
	//	パイプのデータが送られてきてるのでそれをさらに流し込む
	//
	if( lHint == CCollabAppDoc::UpdateHint::GetPipeMsg )
	{
		auto pDoc = GetDocument();
		auto pView = dynamic_cast<CollabLib::CsView^>(GetControl());
		CString msg;
		while( pDoc->GetPipeMessage( msg ) )
		{
			pView->AddMsg( FromString( msg ) );
		}
	}
#endif
}
