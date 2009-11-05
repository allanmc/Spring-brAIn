using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;


namespace QReader
{
    public partial class Form1 : Form
    {

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            richTextBox1.Font = new Font("Courier New", 12);
        }


        private void richTextBox1_TextChanged(object sender, EventArgs e)
        {
        }

        public void setText(string str)
        {
            richTextBox1.Text = str;
        }
    }
}
