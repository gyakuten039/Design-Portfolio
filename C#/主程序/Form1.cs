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
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        public string a
        {
            get
            {
                return user_box.Text;
            }
        }

        public String GetSqlStr(String str)
        {
            return str.Replace("'", "''");
        }

        private void output(string info)
        {
            textBox1.Text = "";
            textBox1.AppendText(info + "\r\n");
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (progressBar1.Value < 100)
            {
                progressBar1.Value++;
                output("系统初始化中:" + progressBar1.Value.ToString() + "%");
            }
            else
            {
                timer1.Enabled = false;
                groupBox2.Visible = true;
                output("系统初始化完成，请用户登录:" + progressBar1.Value.ToString() + "%");
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        public static string strValue = string.Empty;

        public Boolean login()
        {
            if(comboBox1.Text == "用户")
            {
                Shopping shopping = new Shopping();
                string sql = "Select * From Data Where id = '" + GetSqlStr(user_box.Text) + "'and password='" + GetSqlStr(password_box.Text) + "'";
                IDataReader dc = shopping.read(sql);
                if(dc.Read())
                {
                    label6.Visible = true;
                    Form2 frmForm2 = new Form2();
                    strValue = user_box.Text;
                    this.Hide();
                    frmForm2.Show();
                    return true;
                }
                else
                {
                    label5.Visible = true;
                    return false;
                }
                shopping.Close();
            }
            if (comboBox1.Text == "卖家")
            {
                Shopping shopping = new Shopping();
                string sql = "Select * From Data Where id='" + GetSqlStr(user_box.Text) + "'and password='" + GetSqlStr(password_box.Text) + "'";
                IDataReader dc = shopping.read(sql);
                if (dc.Read())
                {
                    label6.Visible = true;
                    Form3 frmForm3 = new Form3();
                    strValue = user_box.Text;
                    this.Hide();
                    frmForm3.Show();
                    return true;
                }
                else
                {
                    label5.Visible = true;
                    return false;
                }
                shopping.Close();
            }
            return false;
        }

        private void button4_Click(object sender, EventArgs e)
        {
            string connStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=E:\\C#\\42324055程博石\\源代码\\Shopping.accdb";
            OleDbConnection conn = new OleDbConnection(connStr);
            conn.Open();
            if(user_box.Text != "" && password_box.Text != "")
            {
                login();
            }
            else
            {
                MessageBox.Show("输入有空项，请重新输入");
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            timer1.Enabled = true;
            this.comboBox1.Items.Add("用户");
            this.comboBox1.Items.Add("卖家");
            this.comboBox2.Items.Add("用户");
            this.comboBox2.Items.Add("用户");
        }

        private void button5_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            groupBox2.Visible = false;
            groupBox3.Visible = true;
        }

        private void button7_Click(object sender, EventArgs e)
        {
            String connStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=E:\\C#\\42324055程博石\\源代码\\源代码\\Shopping.accdb";
            String insertCmd;
            insertCmd = "Insert Into [Data] ([id], [password], [name], [telephone_number], [ID_No], [identity]) Values ('" +
                GetSqlStr(textBox2.Text) + "','" + GetSqlStr(textBox3.Text) + "','" + GetSqlStr(textBox4.Text) +
                "','" + GetSqlStr(textBox5.Text) + "','" +  GetSqlStr(textBox6.Text) + "','" + GetSqlStr(comboBox2.Text) + "')";
            OleDbConnection conn = new OleDbConnection(connStr);
            OleDbCommand cmd;
            conn.Open();
            cmd = new OleDbCommand(insertCmd, conn);
            cmd.ExecuteNonQuery();
            conn.Close();
            MessageBox.Show("注册成功");
        }

        private void button8_Click(object sender, EventArgs e)
        {
            groupBox3.Visible = false;
            groupBox2.Visible = true;
        }

        private void button6_Click(object sender, EventArgs e)
        {
            groupBox2.Visible = false;
            groupBox4.Visible = true;
        }

        private void button9_Click(object sender, EventArgs e)
        {
            string connStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=E:\\C#\\42324055程博石\\源代码\\源代码\\Shopping.accdb";
            OleDbConnection conn = new OleDbConnection(connStr);
            conn.Open();
            string id = "Select id from Data";
            string telephone_number = "Select telephone_number from Data";
            if (textBox7.Text == id && textBox8.Text == telephone_number)
            {
                label17.Visible = true;
                string password = "Select password from Data where id = textBox7.Text";
                label17.Text = "密码为：" + password;
            }
            else
            {
                label17.Visible = true;
                label17.Text = "*账号或电话号码错误";
            }
        }

        private void button10_Click(object sender, EventArgs e)
        {
            groupBox4.Visible = false;
            groupBox2.Visible = true;
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
