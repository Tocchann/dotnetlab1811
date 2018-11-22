using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CollabWpf
{
	public class WrapWpf
	{
		public string Text { get; private set; }
		public bool CallDlg()
		{
			var dlg = new WpfDlg();
			if( dlg.ShowDialog() ?? false )
			{
				Text = dlg.Text;
				return true;
			}
			return false;
		}
	}
}
