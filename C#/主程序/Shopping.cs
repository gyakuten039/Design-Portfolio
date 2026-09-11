using System.Data.OleDb;
using System.Data.SqlClient;

namespace 大作业
{
    internal class Shopping
    {
        public OleDbConnection connect()
        {
            string connStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=E:\\C#\\42324055程博石\\源代码\\Shopping.accdb";
            OleDbConnection conn = new OleDbConnection(connStr);
            conn.Open();
            return conn;
        }

        public OleDbCommand command(string sql)
        {
            OleDbCommand cmd = new OleDbCommand(sql, connect());
            return cmd;
        }

        public int Execute(string sql)
        {
            return command(sql).ExecuteNonQuery();
        }

        public OleDbDataReader read(string sql)
        {
            return command(sql).ExecuteReader();
        }
        public void Close()
        {
            connect().Close();
        }
    }
}
