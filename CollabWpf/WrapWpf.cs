using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CollabWpf
{
	public class WrapWpf
	{
		public string TelNumber { get; private set; }
		public bool CallDlg()
		{
			var dlg = new WpfDlg();
			if( dlg.ShowDialog() ?? false )
			{
				TelNumber = dlg.TelePhone;
				return true;
			}
			return false;
		}
	}
}
