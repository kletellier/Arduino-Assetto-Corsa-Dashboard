using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using AssettoCorsaSharedMemory;


namespace AC_Arduino
{
    class CarStatus
    {
        const string m_FormatMessage = "{0}${1}|";

        private Graphics m_Graphics;
        private Physics m_Physics;
        private StaticInfo m_Static;
       
        private bool m_Graphics_Filled;
        private bool m_Physics_Filled;
        private bool m_Static_Filled;
        
        public enum MessageType
        {
            RPM,
            RPM_TICK,
            SPEED,
            GEAR,
            TYRE_STATUS,
            TYRE_TEMP,
            TIME,
            INIT,
            FUEL,
            LAP,
            CURRENT_TIME,
            FUEL_TICK,
            CURRENT_POSITION,
            PERFORMANCE_METER,
            FUELXLAP,
            TC,
            ABS,
            ENGINE_MAP,
            FUEL_AUTONOMY
        }

        public enum TyrePosition
        {
            LEFT,
            RIGHT
        }

        public struct TyreTemp
        {
            public int Inside;
            public int Outside;
            public int Middle;
        }
               
        public int Rpm
        {
            get { return m_Physics.Rpms; }
        }

        public int RpmTick
        {
            get
            {
                return GetRpmTicke();
            }            
        }

        public int RpmPercent
        {
            get
               {
                return GetRpmPercent();
            }
        }

        public int FuelPercent
        {
            get
            {
                return GetFuelPercent();
            }
        }

        public float Speed
        {
            get { return m_Physics.SpeedKmh; }
        }

        public string Gear
        {
            get {
                string iRet = "-1";
                switch (m_Physics.Gear)
                {
                    case 0:
                        iRet = "R";
                        break;
                    case 1:
                        iRet = "N";
                        break;
                    default:
                        int iSpeed = m_Physics.Gear - 1;
                        iRet = iSpeed.ToString();
                        break;
                }
                return iRet; 
            }
        }

        public int Gas
        {
            get { return Convert.ToInt32(m_Physics.Gas*100) ; }
        }

        public int Brake
        {
            get { return Convert.ToInt32(m_Physics.Brake * 100); }
        }

        public int BestTime
        {
            get { return m_Graphics.iBestTime < int.MaxValue ? m_Graphics.iBestTime: 0; }
        }

        public int LastTime
        {
            get { return m_Graphics.iLastTime < int.MaxValue ? m_Graphics.iLastTime : 0; }
        }

        public int CurrentTime
        {
            get { return m_Graphics.iCurrentTime < int.MaxValue ? m_Graphics.iCurrentTime:0; }
        }

        public int CurrentPosition
        {
            get
            {
                return m_Graphics.Position;
            }
        }
        public int Lap
        {
            get { return m_Graphics.CompletedLaps; }
        }

        public float Fuel
        {
            get { return m_Physics.Fuel; }
        }

        public float PerformanceMeter
        {
            get {                            
                return m_Graphics.iDeltaLapTime / 1000.0f; 
            }
        }

        public int[] TyreWear
        {
            get
            {
                var d = m_Physics.TyreWear.Select(x => (int)x).ToArray();
                return d;
            }
        }

        public int[] TyreTemperature
        {
            get
            {
                var d = m_Physics.TyreTempM.Select(x => (int)x).ToArray();
                return d;
            }
        }

        public TyreTemp[] TyreTemperatureThreePoint
        {
            get
            {
                TyreTemp[] ret = new TyreTemp[] { };
                for (int i = 0; i < 4; i++)
                {
                    TyreTemp tmp = new TyreTemp();
                    tmp.Inside = Convert.ToInt32(m_Physics.TyreTempI[i]);
                    tmp.Outside = Convert.ToInt32(m_Physics.TyreTempO[i]);
                    tmp.Middle = Convert.ToInt32(m_Physics.TyreTempM[i]);
                    ret[i] = tmp;
                }
                return ret;
            }
        }

        public float FuelXLap
        {
            get
            {
                return m_Graphics.FuelXLap;
            }
        }

        public int TC
        {
            get
            {
                return m_Graphics.TC;
            }
        }

        public int ABS
        {
            get
            {
                return m_Graphics.ABS;
            }
        }

        public int Engine_Map
        {
            get
            {
                return m_Graphics.EngineMap;
            }
        }

        public float FuelAutonomy
        {
            get
            {
                return m_Graphics.FuelEstimatedLaps;
            }
        }

        public CarStatus()
        {            
            m_Graphics_Filled = false;
            m_Physics_Filled = false;
            m_Static_Filled = false;
             
        }

        public void FillStatic(StaticInfo  pStatic)
        {
            m_Static = pStatic;
            m_Static_Filled = true;
        }

        public void FillPhysic(Physics pPhysic)
        {
            m_Physics = pPhysic;
            m_Physics_Filled = true;

        }

        public void FillGraphic(Graphics pGraphic)
        {
            m_Graphics = pGraphic;
            m_Graphics_Filled = true;
        }       
        
        private int GetRpmPercent()
        {
            int iRpm = m_Physics.Rpms;
            int iPercent = 0;
            int iMaxRpm = m_Static.MaxRpm;
            if (iMaxRpm > 0)
            {
                double dRatio = Convert.ToDouble(iRpm) / Convert.ToDouble(iMaxRpm);
                double dDisplay = dRatio * 100;

                iPercent = Convert.ToInt32(dDisplay);
            }

            return iPercent;
        } 

        private int GetFuelPercent()
        {
            float iRpm = m_Physics.Fuel;
            int iPercent = 0;
            float iMaxRpm = m_Static.MaxFuel;
            if (iMaxRpm > 0)
            {
                double dRatio = Convert.ToDouble(iRpm) / Convert.ToDouble(iMaxRpm);
                double dDisplay = dRatio * 100;

                iPercent = Convert.ToInt32(dDisplay);
            }

            return iPercent;
        }

        private int GetRpmTicke()
        {           
            return Convert.ToInt32(GetRpmPercent() * 0.17f); ;
        }

        private string FormatMessage(MessageType pMessage, String pValue)
        {
            return String.Format(m_FormatMessage, MessageTypeToString(pMessage), pValue);
        }

        private string FormatMessage(MessageType pMessage, int pValue)
        {
            return String.Format(m_FormatMessage, MessageTypeToString(pMessage), pValue);
        }

        private string FormatMessage(MessageType pMessage, long pValue)
        {
            return String.Format(m_FormatMessage, MessageTypeToString(pMessage), pValue);
        }

        private string FormatMessage(MessageType pMessage, float pValue)
        {
            return String.Format(m_FormatMessage, MessageTypeToString(pMessage), pValue);
        }


        private string FormatMessage(MessageType pMessage, float pValue, string pFloatFormat)
        {
            string sValue = pValue.ToString(pFloatFormat);
            return String.Format(m_FormatMessage, MessageTypeToString(pMessage), sValue);
        }

        public bool IsOk()
        {
            if (m_Graphics_Filled && m_Physics_Filled && m_Static_Filled)
            {
                return (m_Graphics.Status == AC_STATUS.AC_LIVE);
            }
            return false;
        }

        public string GetMessage(MessageType pMessage)
        {
            string sRet = "";
            string sMessage = MessageTypeToString(pMessage);
            if(IsOk())
            {
                switch (pMessage)
                {
                    case MessageType.RPM:
                        sRet =  FormatMessage(pMessage, Rpm);
                        break;
                    case MessageType.RPM_TICK:
                        sRet = FormatMessage(pMessage, RpmPercent);
                        break;
                    case MessageType.SPEED:
                        sRet = FormatMessage(pMessage, Convert.ToInt32(Speed));
                        break;
                    case MessageType.GEAR:
                        sRet = FormatMessage(pMessage, Gear);
                        break;
                    case MessageType.TYRE_STATUS:
                        StringBuilder sb = new StringBuilder();
                        int i = 0;
                        foreach (int item in TyreWear)
                        {
                            if (i > 0) { sb.Append(";"); }
                            sb.Append(item.ToString());
                            i++;
                        }
                        sRet = FormatMessage(pMessage, sb.ToString());
                        break;
                    case MessageType.TYRE_TEMP:
                        StringBuilder sbTemp = new StringBuilder();
                        int iTemp = 0;
                        foreach (int item in TyreTemperature)
                        {
                            if (iTemp > 0) { sbTemp.Append(";"); }
                            sbTemp.Append(item.ToString());
                            iTemp++;
                        }
                        sRet = FormatMessage(pMessage, sbTemp.ToString());
                        break;
                    case MessageType.TIME:
                        StringBuilder sbTime = new StringBuilder();
                        sbTime.Append(BestTime);
                        sbTime.Append(";");
                        sbTime.Append(LastTime); 
                        sRet = FormatMessage(pMessage, sbTime.ToString());
                        break;
                    case MessageType.CURRENT_TIME:
                        sRet = FormatMessage(pMessage, CurrentTime );
                        break;
                    case MessageType.INIT:
                        sRet = FormatMessage(pMessage, "");
                        break;
                    case MessageType.LAP:
                        sRet = FormatMessage(pMessage, Lap);
                        break;
                    case MessageType.FUEL:
                        sRet = FormatMessage(pMessage, Fuel, "0.00").Replace(',','.');
                        break;
                    case MessageType.FUEL_TICK:
                        sRet = FormatMessage(pMessage, FuelPercent);
                        break;
                    case MessageType.CURRENT_POSITION:
                        string sTmp = (CurrentPosition != 0) ? CurrentPosition.ToString() : "_";
                        sRet = FormatMessage(pMessage, sTmp);
                        break;
                    case MessageType.PERFORMANCE_METER:
                        sRet = FormatMessage(pMessage, PerformanceMeter, "0.00").Replace(',', '.');
                        break;
                    case MessageType.FUELXLAP:
                        sRet = FormatMessage(pMessage, FuelXLap, "0.00").Replace(',', '.');
                        break;
                    case MessageType.TC:
                          sRet = FormatMessage(pMessage, TC);
                        break;
                    case MessageType.ABS:
                        sRet = FormatMessage(pMessage, ABS);
                        break;
                    case MessageType.ENGINE_MAP:
                        sRet = FormatMessage(pMessage, Engine_Map);
                        break;
                    case MessageType.FUEL_AUTONOMY:
                        sRet = FormatMessage(pMessage, FuelAutonomy, "0.0").Replace(',', '.');
                        break;
                    default:
                        sRet = FormatMessage(MessageType.INIT, "");
                        break;
                }
            }
            else
            {
                // Init Message
                sRet = FormatMessage(MessageType.INIT, "");
            }
            // display debug message
            System.Diagnostics.Debug.WriteLine(sRet);
            return sRet;
        }



        public static string TyreTempToString(TyreTemp pTt, TyrePosition pPos)
        {
            string sRet = "";

            switch (pPos)
            {
                case TyrePosition.LEFT:
                    sRet = pTt.Outside.ToString() + ";" + pTt.Middle.ToString() + ";" + pTt.Inside.ToString();
                    break;
                case TyrePosition.RIGHT:
                    sRet = pTt.Inside.ToString() + ";" + pTt.Middle.ToString() + ";" + pTt.Outside.ToString();
                    break;
                default:
                    sRet = "";
                    break;
            }

            return sRet;
        }

        private static string MessageTypeToString(MessageType pMessage)
        {
            int iValue = (int)pMessage;
            return Convert.ToString(iValue);
        }
    }
}
