using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PipeClientApp
{
	public partial class Form1 : Form
	{
		private string pipeName;

		public Form1( string name )
		{
			InitializeComponent();
			button1.Enabled = textBox1.Text.Length > 0;
			//	失敗してればここで例外
			pipeName = name;
		}

		private void button1_Click( object sender, EventArgs e )
		{
			UseWaitCursor = true;
			//	接続解放で初めて送信になる。。。パイプストリーム再接続したくないんだけどね。。。なぜか。。。orz
			using( var stream = new NamedPipeClientStream( ".", pipeName, PipeDirection.Out ) )
			{
				stream.Connect();
				//	サーバーアプリはメッセージモードで開けているので、送信はユニコード文字列をそのまま送ればいい
				//	バイナリ転送する場合は、バイトモードで開いてバイナリデータ転送になる。今回は違うのでそのまま。
				using( var writer = new StreamWriter( stream, Encoding.Unicode ) )
				{
					writer.Write( textBox1.Text );
				}
			}
			UseWaitCursor = false;
		}
		private void button2_Click( object sender, EventArgs e )
		{
			//	キャンセルボタンを押した
			Close();
		}

		private void textBox1_TextChanged( object sender, EventArgs e )
		{
			button1.Enabled = string.IsNullOrWhiteSpace( pipeName ) == false && textBox1.Text.Length > 0 ;
		}
	}
}
