using System;
using System.Windows.Forms;

namespace PipeClientApp
{
	static class Program
	{
		/// <summary>
		/// アプリケーションのメイン エントリ ポイントです。
		/// </summary>
		[STAThread]
		static void Main( string[] args )
		{
			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault( false );
			Application.Run( new Form1( args.Length > 0 ? args[0] : "" ) );
		}
	}
}
