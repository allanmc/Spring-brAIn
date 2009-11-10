using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;


namespace QReader
{
    public partial class Form1 : Form
    {
        private OpenFileDialog op;
        public ushort validVersion;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }


        private void richTextBox1_TextChanged(object sender, EventArgs e)
        {
        }

        private void setText(string str)
        {
            richTextBox1.Text = str;
        }

        private void reLoadFile()
        {
            Stream s = op.OpenFile();
            System.IO.BinaryReader br = new BinaryReader(s);
            byte[] header = br.ReadBytes(2);
            if (!(header[0] == 'Q' && header[1] == 'B'))
            {
                MessageBox.Show("Wrong file type!");
                return;
            }

            ushort type = br.ReadUInt16();
            if (type != validVersion)
            {
                MessageBox.Show("Wrong QB file version! Expected version " + validVersion + ", but this file is version " + type);
                return;
            }

            ushort numQTables = br.ReadUInt16();

            Console.WriteLine("numQTables: " + numQTables);

            StringBuilder sb = new StringBuilder();
            System.Text.ASCIIEncoding enc = new System.Text.ASCIIEncoding();
            for (int q = 0; q < numQTables; q++)
            {
                ushort numStates = br.ReadUInt16();
                ushort numActions = br.ReadUInt16();
                ushort numStateVars = br.ReadUInt16();
                string[] actions = new string[numActions];
                string[] stateVarName = new string[numStateVars];
                ushort[] stateVarStates = new ushort[numStateVars];
                Console.WriteLine("numActions: " + numActions);
                for (int a = 0; a < numActions; a++)
                {
                    actions[a] = enc.GetString(br.ReadBytes(br.ReadUInt16() - 1));
                    Console.WriteLine("Action name: " + actions[a]);
                    br.ReadBytes(1);//Read null
                    ushort id = br.ReadUInt16();
                }
                Console.WriteLine("numStateVars: " + numStateVars);
                sb.AppendFormat("State|");
                for (int a = 0; a < numStateVars; a++)
                {
                    stateVarName[a] = enc.GetString(br.ReadBytes(br.ReadUInt16() - 1));
                    Console.WriteLine("StateVar name: " + stateVarName[a]);
                    br.ReadBytes(1);//Read null
                    stateVarStates[a] = br.ReadUInt16();
                    sb.AppendFormat(" {0,9}", "#" + stateVarName[a].Substring(0,Math.Min(8,stateVarName[a].Length)));
                }

                sb.AppendFormat("|");
                for (int a = 0; a < numActions; a++)
                {
                    sb.AppendFormat(" {0,12}", actions[a]);
                }
                sb.AppendFormat("\r\n");

                for (int i = 0; i < numStates; i++)
                {
                    sb.AppendFormat("{0,5}|", i);

                    //sb.AppendFormat(" {1,2} {2,2} {3,2}", (i % (400*20)) / 400, (i % 400) / 20, (i % 20) / 1);
                    int tempMod = 1;
                    int tempDiv = 1;
                    int[] values = new int[numStateVars];
                    for (int a = numStateVars - 1; a >= 0; a--)
                    {
                        tempDiv = tempMod;
                        tempMod *= stateVarStates[a];
                        values[a] = (i % tempMod) / tempDiv;
                    }
                    for (int a = 0; a < numStateVars; a++)
                    {
                        sb.AppendFormat("        {0,2}", values[a]);
                    }

                    sb.AppendFormat("|");
                    for (int a = 0; a < numActions; a++)
                    {
                        float value = br.ReadSingle();
                        sb.AppendFormat(" {0,12:F2}", value);

                    }
                    sb.AppendFormat("\r\n");
                }
                sb.AppendFormat("\r\n");
            }
            br.Close();
            s.Close();
            setText(sb.ToString());
        }

        private void Form1_Shown(object sender, EventArgs e)
        {
            this.Focus();
            richTextBox1.Font = new Font("Courier New", 12);
            op = new OpenFileDialog();
            DialogResult res = op.ShowDialog();
            if (DialogResult.OK != res)
                return;
            
            this.Focus();
            reLoadFile();
            this.Focus();
        }

        private void update_btn_Click(object sender, EventArgs e)
        {
            reLoadFile();
        }
    }
}
