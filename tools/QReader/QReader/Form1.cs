using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Collections;

namespace QReader
{
    public partial class Form1 : Form
    {
        //private Stream file;
        private string filePath;
        public ushort validVersion;

        public Form1(string[] args)
        {
            InitializeComponent();
            this.richTextBox1.DragEnter += new System.Windows.Forms.DragEventHandler(this.richTextBox1_DragEnter);
            this.richTextBox1.DragDrop += new System.Windows.Forms.DragEventHandler(this.richTextBox1_DragDrop);
       
            if (args.Length>0 && File.Exists(args[0])) {
                this.filePath = args[0];
            }
        }

        private void setText(string str)
        {
            richTextBox1.Text = str;
        }

        private void appendText(string str)
        {
            richTextBox1.AppendText(str);
        }


        private void reLoadFile()
        {

            Stream s = File.OpenRead(filePath);
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
            ArrayList highlights = new ArrayList();
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

                    //appendText(sb.ToString());
                    //sb = new StringBuilder();
                    double currentMax = -999999;
                    int index = 0;
                    int length = 0;
                    int previousLength = sb.Length;
                    for (int a = 0; a < numActions; a++)
                    {
                        double value = br.ReadDouble();
                        sb.AppendFormat(" {0,12:F2}", value);
                        if (value > currentMax) {
                            currentMax = value;
                            length = sb.Length - previousLength;
                            index = previousLength;
                        }
                        previousLength = sb.Length;
                    }
                    highlights.Add(new int[2] { index, length });
                    //int oldLength = richTextBox1.Text.Length;
                    //appendText(sb.ToString());
                    //sb = new StringBuilder();
                    //richTextBox1.Select(oldLength + index, length);
                    //richTextBox1.SelectionColor = Color.Red;
                    //richTextBox1.SelectionLength = 0;
                    sb.AppendFormat("\n");
                }
                sb.AppendFormat("\n");
            }
            br.Close();
            s.Close();
            setText(sb.ToString());

            richTextBox1.Hide();
            Font ourFont = new Font(richTextBox1.Font, FontStyle.Bold | FontStyle.Italic);
            for (int i = 0 ; i < highlights.Count;i++) {
                int[] highlight = (int[])highlights[i];
                //richTextBox1.Select(highlight[0], highlight[1]);
                richTextBox1.SelectionStart = highlight[0];
                richTextBox1.SelectionLength = highlight[1];
                richTextBox1.SelectionColor = Color.Green;
                richTextBox1.SelectionFont = ourFont;
            }
            richTextBox1.BackColor = Color.White;
            richTextBox1.Show();

        }

        private void Form1_Shown(object sender, EventArgs e)
        {
            this.Focus();
            richTextBox1.Font = new Font("Courier New", 12);
            OpenFileDialog op = new OpenFileDialog();

            if (filePath=="") {
                StringBuilder sb = new StringBuilder();
                sb.Append("C:\\Program Files\\Spring\\AI\\Skirmish\\brAIn\\0.1");
                
                sb.Append("\\qh.bin");
                filePath = sb.ToString();
            }

            Boolean b = File.Exists(filePath);
            if (!b)
            {         
                DialogResult res = op.ShowDialog();
                if (DialogResult.OK != res)
                    return;
                filePath = op.FileName;                
            }
            
            this.Focus();
            reLoadFile();
            this.Focus();
        }

        private void update_btn_Click(object sender, EventArgs e)
        {
            reLoadFile();
        }

        private void button_load_Click(object sender, EventArgs e)
        {
            OpenFileDialog op;
            op = new OpenFileDialog();
            DialogResult res = op.ShowDialog();
            if (DialogResult.OK != res)
                return;
            this.Focus();
            filePath = op.FileName;
            reLoadFile();
        }
         
        private void richTextBox1_DragEnter(object sender, DragEventArgs e)
        {
            // make sure they're actually dropping files (not text or anything else)
            if (e.Data.GetDataPresent(DataFormats.FileDrop, false) == true)
            {
                // allow them to continue
                // (without this, the cursor stays a "NO" symbol
                e.Effect = DragDropEffects.All;
            }
        }

        private void richTextBox1_DragDrop(object sender, DragEventArgs e)
        {
            // transfer the filenames to a string array
            // (yes, everything to the left of the "=" can be put in the 
            // foreach loop in place of "files", but this is easier to understand.)
            string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);

            // loop through the string array, adding each filename to the ListBox
            if (files.Length>0) {
                filePath = files[0];
                reLoadFile();
            }
            /*foreach (string file in files)
            {
                Console.WriteLine(file);
            }*/
            e.Effect = DragDropEffects.None;

        }
    }
}
