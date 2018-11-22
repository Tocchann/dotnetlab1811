﻿using System.Windows;
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
	}
}
