using System;
using System.IO.Pipes;
using System.Text;
using System.Threading;
using System.Windows.Forms;


// keep_connect を定義すると、パイプを接続しっぱなしにしてしまうので、サーバー側を先に終了できなくなる(やらなきゃいいんだが難しい。。。)

namespace PipeClientApp
{
	public partial class Form1 : Form
	{
		private string pipeName;
		private CancellationTokenSource cts;
		private UnicodeEncoding encoder;
#if keep_connect
		private NamedPipeClientStream pipeStream;
#endif
		public Form1( string name )
		{
			InitializeComponent();
			button1.Enabled = string.IsNullOrWhiteSpace( name ) == false ;
			//	失敗してればここで例外
			encoder = new UnicodeEncoding( false, false );
			pipeName = name;
			cts = new CancellationTokenSource();
		}
#if keep_connect
		private async void Form1_Load( object sender, EventArgs e )
		{
			if( !string.IsNullOrWhiteSpace( pipeName ) )
			{
				pipeStream = new NamedPipeClientStream( ".", pipeName, PipeDirection.Out );
				await pipeStream.ConnectAsync( cts.Token );
			}
		}
#else
		private void Form1_Load( object sender, EventArgs e )
		{
			//	ここでは特に何かをすることはしない
		}
#endif
		private void Form1_FormClosing( object sender, FormClosingEventArgs e )
		{
			cts.Cancel();	//	クローズしようとしたところでキャンセルを送る(どこかで非同期処理があるかもしれない)
		}
		private void Form1_FormClosed( object sender, FormClosedEventArgs e )
		{
#if keep_connect
			if( pipeStream != null )
			{
				pipeStream.Close();
				pipeStream.Dispose();
				pipeStream = null;
			}
#endif
			cts.Dispose();
		}
		private async void button1_Click( object sender, EventArgs e )
		{
			try
			{
				var token = cts.Token;
#if !keep_connect
				using( var pipeStream = new NamedPipeClientStream( ".", pipeName, PipeDirection.Out ) )
				{
					await pipeStream.ConnectAsync( token );
#endif
					var bytes = encoder.GetBytes( textBox1.Text );
					var sendLengths = BitConverter.GetBytes( bytes.Length );
					await pipeStream.WriteAsync( sendLengths, 0, sendLengths.Length );
					await pipeStream.WriteAsync( bytes, 0, bytes.Length, token );
					await pipeStream.FlushAsync( token );
#if !keep_connect
				}
#endif
			}
			catch( Exception exp )
			{
				MessageBox.Show( $"送信できませんでした。サーバーが動作しているか確認してください。\n{exp.Message}", "PipeClientApp", MessageBoxButtons.OK, MessageBoxIcon.Exclamation );
				//	送信できないので送信ボタンを強制的に無効化
				pipeName = "";
				button1.Enabled = false;
			}
		}
		private void button2_Click( object sender, EventArgs e )
		{
			//	キャンセルボタンを押した
			Close();
		}

		private void textBox1_TextChanged( object sender, EventArgs e )
		{
			button1.Enabled = textBox1.Text.Length > 0 && string.IsNullOrWhiteSpace( pipeName ) == false ;
		}
	}
}
