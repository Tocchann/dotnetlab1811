using System;
using System.Diagnostics;
using System.Windows.Forms;

namespace CollabLib
{
	public partial class CsView : UserControl
	{
		public CsView()
		{
			InitializeComponent();
			DisplayMessage = label1.Text;	//	デフォルト値
		}

		private void CsView_SizeChanged( object sender, EventArgs e )
		{
			Debug.WriteLine( $"Call SizeChanged:{this.Size}" );
		}
		public string DisplayMessage
		{
			get
			{
				return label1.Text;
			}
			set
			{
				label1.Text = value;
			}
		}
	}
}
