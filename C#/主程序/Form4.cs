using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Data.OleDb;
using System.Drawing;
using System.Linq;
using System.Net.Http.Headers;
using System.Security.Policy;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

namespace 大作业
{
    public partial class Form4 : Form
    {
        public Form4()
        {
            InitializeComponent();
        }

        public String GetSqlStr(String str)
        {
            return str.Replace("'", "'");
        }

        public void ShowGood()
        {
            String connStr, selectCmd1;
            connStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=E:\\C#\\42324055程博石\\源代码\\Shopping.accdb";
            selectCmd1 = "Select goods as 商品 , price as 价格 , seller as 卖家 From Shop1";
            OleDbConnection conn;
            OleDbDataAdapter myAdapter1;
            DataSet myDataSet = new DataSet();
            conn = new OleDbConnection(connStr);
            conn.Open();
            myAdapter1 = new OleDbDataAdapter(selectCmd1, conn);
            myAdapter1.Fill(myDataSet, "good1");
            dataGridView1.DataSource = myDataSet.Tables["good1"];
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Form2 frmForm2 = new Form2();
            this.Hide();
            frmForm2.Show();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            string id = Form2.strValue;
            String connStr,insertCmd1;
            connStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=E:\\C#\\42324055程博石\\源代码\\Shopping.accdb";
            int i = dataGridView1.CurrentRow.Index;
            string cellValues = dataGridView1.Rows[i].Cells["商品"].Value.ToString();
            insertCmd1 = "Insert Into Shop1(buyer) Values ('" + GetSqlStr(textBox1.Text) + "') Where goods = ('" + GetSqlStr(cellValues) + "')";
            OleDbConnection conn;
            OleDbCommand cmd;
            conn = new OleDbConnection(connStr);
            conn.Open();
            cmd = new OleDbCommand(insertCmd1, conn);
            conn.Close();
            MessageBox.Show("购买成功");
            ShowGood();
        }

        private void Form4_Load(object sender, EventArgs e)
        {
            ShowGood();
            string id = Form2.strValue;
            textBox1.Text = id;
        }
    }
}
