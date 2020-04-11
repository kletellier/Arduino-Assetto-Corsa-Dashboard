using System;
using System.Collections.Generic;
using System.Linq;
using System.Management;
using System.Text;
using System.Threading.Tasks;

namespace AC_Test
{
    static class cUtils
    {
 

        public static List<string> GetCOMPort()
        {
            List<string> oCOMS = new List<string>();
            ManagementObjectCollection mReturn;
            ManagementObjectSearcher mSearch;
            mSearch = new ManagementObjectSearcher("Select * from Win32_SerialPort");
            mReturn = mSearch.Get();

            foreach (ManagementObject mObj in mReturn)
            {
                Console.WriteLine("Serial port found : " + mObj["Name"].ToString());
                if (mObj["Name"].ToString().ToLower().Contains("arduino"))
                {
                    string[] sMots = mObj["Name"].ToString().Split(' ');
                    string sPort = sMots[sMots.Length - 1].Replace("(", "").Replace(")", "");
                    oCOMS.Add(sPort);
                }
            }
            return oCOMS;
        }
      
    }
}
