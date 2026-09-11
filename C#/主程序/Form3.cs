using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace 大作业
{
    public partial class Form3 : Form
    {
        public Form3()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Form7 frmForm7 = new Form7();
            strValue = textBox1.Text;
            this.Hide();
            frmForm7.Show();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            Form9 frmForm9 = new Form9();
            strValue = textBox1.Text;
            this.Hide();
            frmForm9.Show();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void Form3_Load(object sender, EventArgs e)
        {
            string id = Form1.strValue;
            textBox1.Text = id;
        }

        public static string strValue = string.Empty;
    }
}
