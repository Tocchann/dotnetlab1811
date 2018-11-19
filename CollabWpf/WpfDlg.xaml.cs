using System.Windows;
using System.Windows.Controls;

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
		public int ButtonValue { get; set; }
		private void Button_Click( object sender, RoutedEventArgs e )
		{
			ButtonValue = int.Parse( ((Button)sender).Content.ToString() );
			DialogResult = true;
		}
	}
}
