using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AC_Test
{
    class Program
    {
        static void Main(string[] args)
        {
             
            List<string> lPorts = cUtils.GetCOMPort();
            if (lPorts.Count == 0)
            {
                Console.WriteLine("No Arduino detected");
                ConsoleKeyInfo exit = Console.ReadKey();
            }
            else
            {
                int iNumFile = 1;
                Console.WriteLine("Select file");
                List<string> oFiles = new List<string>();
                foreach (var sFile in System.IO.Directory.GetFiles(AppDomain.CurrentDomain.BaseDirectory, "*.txt"))
                {
                    System.IO.FileInfo oFi = new System.IO.FileInfo(sFile);
                    string sListFile = iNumFile.ToString() + ") " + oFi.Name;
                    oFiles.Add(sFile);
                    iNumFile++;
                    Console.WriteLine(sListFile);
                }
                ConsoleKeyInfo file = Console.ReadKey();
                int numberFile = 1;
                Int32.TryParse(file.KeyChar.ToString(), out numberFile);
                string sFileToParse = oFiles.ElementAt(numberFile - 1);

                int iNum = 1;
                Console.WriteLine("Select Arduino Port");
                foreach (var sPort in lPorts)
                {
                    string sList = iNum.ToString() + ") " + sPort;
                    iNum++;
                    Console.WriteLine(sList);
                }

                ConsoleKeyInfo port = Console.ReadKey();
                int number;
                if (!Int32.TryParse(port.KeyChar.ToString(), out number))
                {

                }
                else
                {
                    string sPort = lPorts[number - 1];
                    cTransmitter obj = new cTransmitter(sPort, sFileToParse);
                    obj.Start();
                    Console.WriteLine("Press key to stop");
                    ConsoleKeyInfo ret = Console.ReadKey();
                    obj.Stop();
                    obj = null;
                }
            } 
        }
    }
}
