using System;
using System.Collections.Generic;
using System.Linq;
using System.Text; 
using System.IO.Ports;
using System.Threading;
using System.Diagnostics;
using System.Management;
using System.Timers;
using System.Threading.Tasks;
using System.IO;

namespace AC_Test
{
    class cTransmitter
    {
        public const char MESSAGE_SEPARATOR = '$';
        public const char MESSAGE_DELIMITER = '|';
         
        SerialPort pSerialPort;
        System.Timers.Timer pTimer; 
        int iTicks;
        bool bSendingAllowed;
        string sFile;
        FileStream oFs;
        StreamReader oSr;
        Dictionary<int, string> oDicLasts = new Dictionary<int, string>();

        public bool PortIsOpen
        {
            get { return pSerialPort.IsOpen; }
        }

        public cTransmitter(string sPort = "",string pFile = "")
        {
            sFile = pFile;
            Init();
            InitSerialPort(sPort);
            OpenPort();
            InitTimer();
            StartTimer();
        }

        private void InitSerialPort(string sPort)
        {
            // select correct Port
            if(sPort=="")
            {
                Console.WriteLine("Retrieving serial port");
                sPort = GetArduinoPort();
            }              
            Console.WriteLine("Serial Port : " + sPort);
            if (sPort!="")
            {
                Console.WriteLine("Instantiate serial port");
                pSerialPort = new SerialPort(sPort, 115200);
                pSerialPort.DataReceived += PSerialPort_DataReceived;
            }
        }

        private void PSerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            SerialPort sp = (SerialPort)sender;
            string sData = sp.ReadExisting();
            string sLast = sData.Substring(sData.Length - 1).Trim().ToUpper();
            bSendingAllowed = (sLast == "O") ? false : true;
        }

        private void OpenPort()
        {
            if(pSerialPort != null)
            {
                Console.WriteLine("Open serial port");
                pSerialPort.Open();
            }            
        }

        private void InitTimer()
        {
            Console.WriteLine("Init timer");
            pTimer = new System.Timers.Timer(50);
            pTimer.Elapsed += PTimer_Elapsed;
        }

        private void StartTimer()
        {             
            pTimer.Start();
        }

        private void StopTimer()
        {
            pTimer.Stop();
        }

        private void PTimer_Elapsed(object sender, ElapsedEventArgs e)
        {
            StopTimer();
            // Send messages 
            foreach (string str in MessagesToSend())
            {
                string sTest = str;
                string[] aTest = str.Split('$');
                if(aTest.Length==2)
                {
                    int iMsgType = Convert.ToInt32(aTest[0]);
                    string sMessage = aTest[1];
                    bool bSend = true;
                    if (oDicLasts.ContainsKey(iMsgType))
                    {
                        // send message only if data is refreshed
                        string sLast = oDicLasts[iMsgType];
                        if (sLast == sMessage) { bSend = false; }
                    }
                    if (bSendingAllowed && bSend)
                    {
                        oDicLasts[iMsgType] = str;
                        pSerialPort.Write(str);
                    }
                }
                               
            } 
            StartTimer();
        }

        private List<string> MessagesToSend()
        {
            List<string> oRet = new List<string>();
            bool bOk = true;
            do
            {
                if(oSr.EndOfStream)
                {
                    bOk = false;
                }
                if(bOk)
                {
                    string sLine = oSr.ReadLine();
                    if(sLine == "##TICK##")
                    {
                        bOk = false;
                    }
                    else
                    {
                        oRet.Add(sLine);
                    }
                }
            } while (bOk);
            return oRet;
        }

        private void ClosePort()
        {
            if (pSerialPort != null && pSerialPort.IsOpen) { Console.WriteLine("Close serial port");  pSerialPort.Close(); }
        }

        

        private string GetArduinoPort()
        {
            string sRet = "";
            try
            {
                ManagementObjectCollection mReturn;
                ManagementObjectSearcher mSearch;
                mSearch = new ManagementObjectSearcher("Select * from Win32_SerialPort");
                mReturn = mSearch.Get();

                foreach (ManagementObject mObj in mReturn)
                {
                    Console.WriteLine("Serial port found : " + mObj["Name"].ToString());
                    if(mObj["Name"].ToString().ToLower().Contains("arduino"))
                    { 
                        string[] sMots = mObj["Name"].ToString().Split(' ');
                        string sPort = sMots[sMots.Length - 1].Replace("(", "").Replace(")", "");
                        sRet = sPort;
                    }
                }
            }
            catch (Exception ex)
            {
                 
            }
            return sRet;
        }

        private void Init()
        {
            iTicks = 0;
            bSendingAllowed = true;
            oFs = new FileStream(sFile, FileMode.Open, FileAccess.Read);
            oSr = new StreamReader(oFs);
        } 

        public void Start()
        {
             
        }

        public void Stop()
        {
            oFs.Close();
            oSr.Close();

            try
            {
                oFs = null;
                oSr = null;
            }
            catch (Exception)
            {
                 
            }          
        }

        public void Close()
        {
            StopTimer();
            ClosePort();
            Stop();           
        }
    }
}
