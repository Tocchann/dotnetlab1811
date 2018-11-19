namespace CollabLib
{
	partial class CsView
	{
		/// <summary> 
		/// 必要なデザイナー変数です。
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary> 
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		/// <param name="disposing">マネージド リソースを破棄する場合は true を指定し、その他の場合は false を指定します。</param>
		protected override void Dispose( bool disposing )
		{
			if( disposing && (components != null) )
			{
				components.Dispose();
			}
			base.Dispose( disposing );
		}

		#region コンポーネント デザイナーで生成されたコード

		/// <summary> 
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を 
		/// コード エディターで変更しないでください。
		/// </summary>
		private void InitializeComponent()
		{
			this.label1 = new System.Windows.Forms.Label();
			this.button1 = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.label1.Location = new System.Drawing.Point(0, 0);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(339, 183);
			this.label1.TabIndex = 1;
			this.label1.Text = "C#のコントロール。特に操作はしない。";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// button1
			// 
			this.button1.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.button1.Location = new System.Drawing.Point(0, 160);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(339, 23);
			this.button1.TabIndex = 0;
			this.button1.Text = "お、おすなよ！";
			this.button1.UseVisualStyleBackColor = true;
			// 
			// CsView
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.Controls.Add(this.button1);
			this.Controls.Add(this.label1);
			this.Name = "CsView";
			this.Size = new System.Drawing.Size(339, 183);
			this.SizeChanged += new System.EventHandler(this.CsView_SizeChanged);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Label label1;
		public System.Windows.Forms.Button button1;
	}
}
