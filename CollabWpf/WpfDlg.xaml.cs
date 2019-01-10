using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interop;

namespace CollabWpf
{
	/// <summary>
	/// WpfDlg.xaml の相互作用ロジック
	/// </summary>
	public partial class WpfDlg : Window
	{
		public WpfDlg()
		{
			InitializeComponent();
		}
		public string Text { get; set; }
		private void Button_Click( object sender, RoutedEventArgs e )
		{
			Data.Text += ((Button)sender).Content.ToString();
		}

		private void EnterButton_Click( object sender, RoutedEventArgs e )
		{
			Text = Data.Text;
			DialogResult = true;
		}
		public bool? ShowDialog( IntPtr hwndOwner )
		{
			Debug.Assert( hwndOwner != IntPtr.Zero );
			var helper = new WindowInteropHelper( this );
			helper.Owner = hwndOwner;
			return ShowDialog();
		}
	}
}
