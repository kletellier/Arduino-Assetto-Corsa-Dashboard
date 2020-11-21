using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using AssettoCorsaSharedMemory;
using System.IO.Ports;
using System.Threading;
using System.Diagnostics;
using System.Management;
using System.Timers;
using System.Threading.Tasks;

namespace AC_Arduino
{
    class cTransmitter
    {
        public const char MESSAGE_SEPARATOR = '$';
        public const char MESSAGE_DELIMITER = '|';

        AssettoCorsa pAc = new AssettoCorsa();
        SerialPort pSerialPort;
        System.Timers.Timer pTimer;
        CarStatus pCarStatus = new CarStatus();
        int iTicks;
        bool bSendingAllowed;
        Dictionary<CarStatus.MessageType, string> oDicLasts = new Dictionary<CarStatus.MessageType, string>();

        public bool PortIsOpen
        {
            get { return pSerialPort.IsOpen; }
        }

        public cTransmitter(string sPort = "")
        {
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
            // Send all messages
            iTicks++;
            System.Diagnostics.Debug.WriteLine("##TICK##");
            bool bSended = false;
            if(iTicks == 25)
            {
                WriteToPort(CarStatus.MessageType.LAP);
                WriteToPort(CarStatus.MessageType.CURRENT_POSITION);
                WriteToPort(CarStatus.MessageType.FUEL_TICK);
                WriteToPort(CarStatus.MessageType.TIME);
                bSended = true;
                iTicks = 0;
            }
            if (iTicks == 19)
            {                      
                WriteToPort(CarStatus.MessageType.FUEL);
                WriteToPort(CarStatus.MessageType.FLAG);
                bSended = true;
            }
            if (iTicks == 17)
            {               
                WriteToPort(CarStatus.MessageType.FUELXLAP);
                WriteToPort(CarStatus.MessageType.FUEL_AUTONOMY);
                bSended = true;
            }
            if (iTicks == 15)
            {               
                WriteToPort(CarStatus.MessageType.TC);
                WriteToPort(CarStatus.MessageType.ABS);
                WriteToPort(CarStatus.MessageType.ENGINE_MAP);
                bSended = true;
            }
            if (iTicks == 5)
            {
                bSended = true;
            }            
            if (!bSended && (iTicks % 2 == 0))
            {
                WriteToPort(CarStatus.MessageType.GEAR);
                WriteToPort(CarStatus.MessageType.RPM);
                WriteToPort(CarStatus.MessageType.SPEED);
                WriteToPort(CarStatus.MessageType.RPM_TICK);
            }            
            
            StartTimer();
        }

        private void ClosePort()
        {
            if (pSerialPort != null && pSerialPort.IsOpen) { Console.WriteLine("Close serial port");  pSerialPort.Close(); }
        }

        private void WriteToPort(CarStatus.MessageType pType)
        {
            // send data only if client required
            if(bSendingAllowed)
            {
                string sMessage = pCarStatus.GetMessage(pType);
                bool bSend = true;
                if(oDicLasts.ContainsKey(pType))
                {
                    // send message only if data is refreshed
                    string sLast = oDicLasts[pType];
                    if (sLast == sMessage) { bSend = false; }
                }
                if (bSend) { pSerialPort.Write(sMessage); oDicLasts[pType] = sMessage; }
            }           
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
            Console.WriteLine("Initialize Assetto Corsa Competizione shared memory component");
            pAc = new AssettoCorsa();
            pAc.StaticInfoUpdated += PAc_StaticInfoUpdated;
            pAc.PhysicsUpdated += PAc_PhysicsUpdated;
            pAc.GraphicsUpdated += PAc_GraphicsUpdated;
        }

        private void PAc_GraphicsUpdated(object sender, GraphicsEventArgs e)
        {
            pCarStatus.FillGraphic(e.Graphics);
        }

        private void PAc_PhysicsUpdated(object sender, PhysicsEventArgs e)
        {
            pCarStatus.FillPhysic(e.Physics);
        }

        private void PAc_StaticInfoUpdated(object sender, StaticInfoEventArgs e)
        {
            pCarStatus.FillStatic(e.StaticInfo);
        }

        public void Start()
        {
            pAc.Start();
        }

        public void Stop()
        {
            pAc.Stop();
        }

        public void Close()
        {
            StopTimer();
            ClosePort();
            Stop();           
        }
    }
}
