using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CollabWpf
{
	public class WrapWpf
	{
		public static int CallWpfDlg()
		{
			var dlg = new WpfDlg();
			dlg.ShowDialog();
			return dlg.ButtonValue;
		}
	}
}
