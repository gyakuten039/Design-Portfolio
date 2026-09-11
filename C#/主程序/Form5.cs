using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Data.OleDb;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace 大作业
{
    public partial class Form5 : Form
    {
        public Form5()
        {
            InitializeComponent();
        }

        string id = Form2.strValue;

        public String GetSqlStr(String str)
        {
            return str.Replace("'", "'");
        }

        public void ShowGood()
        {
            String connStr, selectCmd1;
            connStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=E:\\C#\\42324055程博石\\源代码\\Shopping.accdb";
            selectCmd1 = "Select goods as 商品 , price as 价格 , seller as 卖家 From Shop1 Where buyer = ('" + GetSqlStr(id) + "')";
            OleDbConnection conn;
            OleDbDataAdapter myAdapter1;
            DataSet myDataSet = new DataSet();
            conn = new OleDbConnection(connStr);
            conn.Open();
            myAdapter1 = new OleDbDataAdapter(selectCmd1, conn);
            myAdapter1.Fill(myDataSet, "good1");
            dataGridView1.DataSource = myDataSet.Tables["good1"];
        }

        private void button3_Click(object sender, EventArgs e)
        {
            Form2 frmForm2 = new Form2();
            this.Hide();
            frmForm2.Show();
        }

        private void Form5_Load(object sender, EventArgs e)
        {
            ShowGood();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            String connStr,insertCmd;
            connStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=E:\\C#\\42324055程博石\\源代码\\Shopping.accdb";
            string cellValue1 = dataGridView1.CurrentRow.Cells["商品"].Value.ToString();
            insertCmd = "Insert Into Shop1(location) Values ('" + GetSqlStr(textBox1.Text) + "') Where goods = ('" + GetSqlStr(cellValue1) + "')";
            OleDbConnection conn;
            OleDbCommand cmd;
            conn = new OleDbConnection(connStr);
            conn.Open();
            cmd = new OleDbCommand(insertCmd, conn);
            conn.Close();
            MessageBox.Show("地址填写成功");
        }
    }
}
