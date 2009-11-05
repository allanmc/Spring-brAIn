using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using System.IO;
using System.Text;

namespace QReader
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Form1 lala = new Form1();

            OpenFileDialog op = new OpenFileDialog();
            DialogResult res = op.ShowDialog();
            if (DialogResult.OK != res)
                return;
            Stream s =  op.OpenFile();
            System.IO.BinaryReader br = new BinaryReader(s);
            StringBuilder sb = new StringBuilder();
            sb.Append("State| #L #M #S|    Lab     MEX    Solar\r\n");
            for (int i = 0; i < 2000; i++)
            {
                float solar = br.ReadSingle();
                float mex = br.ReadSingle();
                float lab = br.ReadSingle();

                sb.AppendFormat("{3,5}| {4,2} {5,2} {6,2}| {2,7:F2} {1,7:F2} {0,7:F2}\r\n", solar, mex, lab, i, i/400, (i%400)/20, i%20);

            }

            lala.setText(sb.ToString());
            Application.Run(lala);
        }
    }
}
