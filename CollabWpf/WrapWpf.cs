using System;

namespace CollabWpf
{
	public class WrapWpf
	{
		public string Text { get; private set; }
		public bool CallDlg( IntPtr hwndOwner )
		{
			var dlg = new WpfDlg();
			var result = ((hwndOwner != IntPtr.Zero) ? dlg.ShowDialog( hwndOwner ) : dlg.ShowDialog()) ?? false;
			if( result )
			{
				Text = dlg.Text;
				return true;
			}
			return false;
		}
	}
}
