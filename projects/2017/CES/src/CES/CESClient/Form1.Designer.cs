namespace CESClient
{
    partial class Form1
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.bn_ScanOrTakePicture = new System.Windows.Forms.Button();
            this.ax_CES = new AxCESLib.AxCES();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ax_CES)).BeginInit();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Margin = new System.Windows.Forms.Padding(4);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.bn_ScanOrTakePicture);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.ax_CES);
            this.splitContainer1.Size = new System.Drawing.Size(937, 602);
            this.splitContainer1.SplitterDistance = 311;
            this.splitContainer1.SplitterWidth = 5;
            this.splitContainer1.TabIndex = 0;
            // 
            // bn_ScanOrTakePicture
            // 
            this.bn_ScanOrTakePicture.Location = new System.Drawing.Point(12, 12);
            this.bn_ScanOrTakePicture.Name = "bn_ScanOrTakePicture";
            this.bn_ScanOrTakePicture.Size = new System.Drawing.Size(97, 30);
            this.bn_ScanOrTakePicture.TabIndex = 0;
            this.bn_ScanOrTakePicture.Text = "开始扫描";
            this.bn_ScanOrTakePicture.UseVisualStyleBackColor = true;
            this.bn_ScanOrTakePicture.Click += new System.EventHandler(this.bn_ScanOrTakePicture_Click);
            // 
            // ax_CES
            // 
            this.ax_CES.Dock = System.Windows.Forms.DockStyle.Fill;
            this.ax_CES.Enabled = true;
            this.ax_CES.Location = new System.Drawing.Point(0, 0);
            this.ax_CES.Name = "ax_CES";
            this.ax_CES.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("ax_CES.OcxState")));
            this.ax_CES.Size = new System.Drawing.Size(621, 602);
            this.ax_CES.TabIndex = 0;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(937, 602);
            this.Controls.Add(this.splitContainer1);
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "Form1";
            this.Text = "Form1";
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.ax_CES)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Button bn_ScanOrTakePicture;
        private AxCESLib.AxCES ax_CES;
    }
}

