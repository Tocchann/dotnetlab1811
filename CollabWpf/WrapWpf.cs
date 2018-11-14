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
			if( dlg.ShowDialog() ?? false )
			{
				return dlg.ButtonValue;
			}
			return -1;
		}
	}
}
