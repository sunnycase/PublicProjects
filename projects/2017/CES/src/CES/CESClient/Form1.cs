using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace CESClient
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        bool _isScanning = false;

        private void bn_ScanOrTakePicture_Click(object sender, EventArgs e)
        {
            if(_isScanning)
            {
                bn_ScanOrTakePicture.Text = "开始扫描";
                ax_CES.TakePicture();
                _isScanning = false;
            }
            else
            {
                bn_ScanOrTakePicture.Text = "拍照";
                ax_CES.StartScanning();
                _isScanning = true;
            }
        }

        private void bn_SetOptions_Click(object sender, EventArgs e)
        {
            ax_CES.ShowPropertyPages();
        }
    }
}
