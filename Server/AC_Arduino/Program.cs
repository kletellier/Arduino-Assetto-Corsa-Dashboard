using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using AssettoCorsaSharedMemory;
using System.IO.Ports;
using System.Threading;
using System.Diagnostics;

namespace AC_Arduino
{
    class Program
    {
        static void Main(string[] args)
        {
            string filename = AppDomain.CurrentDomain.BaseDirectory + @"\" + DateTime.Now.ToString("yyyyMMdd") + ".txt";
            TextWriterTraceListener listener = new TextWriterTraceListener(filename);
            Debug.Listeners.Add(listener); 
            List<string> lPorts = cUtils.GetCOMPort();
            if(lPorts.Count==0)
            {
                Console.WriteLine("No Arduino detected");
                ConsoleKeyInfo exit = Console.ReadKey();
            }
            else
            {
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
                    cTransmitter obj = new cTransmitter(sPort);
                    obj.Start();
                    Console.WriteLine("Press key to stop");
                    ConsoleKeyInfo ret = Console.ReadKey();
                    obj.Stop();
                    obj = null;
                }                  
            }
            listener.Flush();
        }         
    }
}
