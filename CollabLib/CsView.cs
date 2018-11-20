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
		}

		private void CsView_SizeChanged( object sender, EventArgs e )
		{
			Debug.WriteLine( $"Call SizeChanged:{this.Size}" );
		}

		public void AddMsg( String msg )
		{
			if( !string.IsNullOrWhiteSpace( msg ) )
			{
				listBox1.Items.Add( msg );
			}
		}
	}
}
