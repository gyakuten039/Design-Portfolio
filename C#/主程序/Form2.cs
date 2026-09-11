using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net.Http.Headers;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace 大作业
{
    public partial class Form2 : Form
    {
        public Form2()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Form4 frmForm4 = new Form4();
            strValue = textBox1.Text;
            this.Hide();
            frmForm4.Show();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Form5 frmForm5 = new Form5();
            strValue = textBox1.Text;
            this.Hide();
            frmForm5.Show();
        }


        private void button4_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void Form2_Load(object sender, EventArgs e)
        {
            string id = Form1.strValue;
            textBox1.Text = id;
        }

        public static string strValue = string.Empty;
    }
}
