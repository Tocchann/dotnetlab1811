using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;

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
	}
}
