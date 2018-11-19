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
		public string TelePhone { get; set; }
		private void Button_Click( object sender, RoutedEventArgs e )
		{
			TelNumber.Text += ((Button)sender).Content.ToString();
		}

		private void EnterButton_Click( object sender, RoutedEventArgs e )
		{
			TelePhone = TelNumber.Text;
			DialogResult = true;
		}
	}
}
