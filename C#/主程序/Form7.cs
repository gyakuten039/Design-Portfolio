using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Data.OleDb;
using System.Drawing;
using System.Linq;
using System.Security.Principal;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;
using System.Xml.Linq;

namespace 大作业
{
    public partial class Form7 : Form
    {
        public Form7()
        {
            InitializeComponent();
        }

        string id = Form3.strValue;

        public String GetSqlStr(String str)
        {
            return str.Replace("'", "'");
        }

        public void ShowGood()
        {
            String connStr, selectCmd1;
            connStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=E:\\C#\\42324055程博石\\源代码\\Shopping.accdb";
            selectCmd1 = "Select goods as 商品 , price as 价格 From Shop1 Where seller = '" + GetSqlStr(id) + "'";
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
            Form3 frmForm3 = new Form3();
            this.Hide();
            frmForm3.Show();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            String connStr, insertCmd;
            connStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=E:\\C#\\42324055程博石\\源代码\\Shopping.accdb";
            insertCmd = "Insert Into[Shop1] ([goods], [price], [seller]) Values('" + GetSqlStr(textBox1.Text) + "','" + GetSqlStr(textBox2.Text) + "','" + GetSqlStr(id) + "')";
            OleDbConnection conn;
            OleDbCommand cmd;
            conn = new OleDbConnection(connStr);
            conn.Open();
            cmd = new OleDbCommand(insertCmd, conn);
            cmd.ExecuteNonQuery();
            conn.Close();
            MessageBox.Show("上架成功");
            ShowGood();
        }

        private void Form7_Load(object sender, EventArgs e)
        {
            ShowGood();
        }
    }
}
