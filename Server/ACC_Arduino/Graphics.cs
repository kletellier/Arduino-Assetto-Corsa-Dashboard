using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace AssettoCorsaSharedMemory
{
    public enum AC_FLAG_TYPE
    {
        AC_NO_FLAG = 0,
        AC_BLUE_FLAG = 1,
        AC_YELLOW_FLAG = 2,
        AC_BLACK_FLAG = 3,
        AC_WHITE_FLAG = 4,
        AC_CHECKERED_FLAG = 5,
        AC_PENALTY_FLAG = 6
    }

    public enum AC_STATUS
    {
        AC_OFF = 0,
        AC_REPLAY = 1,
        AC_LIVE = 2,
        AC_PAUSE = 3
    }

    public enum AC_SESSION_TYPE
    {
        AC_UNKNOWN = -1,
        AC_PRACTICE = 0,
        AC_QUALIFY = 1,
        AC_RACE = 2,
        AC_HOTLAP = 3,
        AC_TIME_ATTACK = 4,
        AC_DRIFT = 5,
        AC_DRAG = 6,
        AC_HOTSTINT = 7,
        AC_HOTLAPSUPERPOLE = 8
    }

    public enum AC_PENALTY
    {
        None,
        DriveThrough_Cutting,
        StopAndGo_10_Cutting,
        StopAndGo_20_Cutting,
        StopAndGo_30_Cutting,
        Disqualified_Cutting,
        RemoveBestLaptime_Cutting,
        DriveThrough_PitSpeeding,
        StopAndGo_10_PitSpeeding,
        StopAndGo_20_PitSpeeding,
        StopAndGo_30_PitSpeeding,
        Disqualified_PitSpeeding,
        RemoveBestLaptime_PitSpeeding,
        Disqualified_IgnoredMandatoryPit,
        PostRaceTime,
        Disqualified_Trolling,
        Disqualified_PitEntry,
        Disqualified_PitExit,
        Disqualified_WrongWay,
        DriveThrough_IgnoredDriverStint,
        Disqualified_IgnoredDriverStint,
        Disqualified_ExceededDriverStintLimit,
    }

    public class GraphicsEventArgs : EventArgs
    {
        public GraphicsEventArgs (Graphics graphics)
        {
            this.Graphics = graphics;
        }

        public Graphics Graphics { get; private set; }
    }

    [StructLayout (LayoutKind.Sequential, Pack = 4, CharSet = CharSet.Unicode)]
    [Serializable]
    public struct Graphics
    {
        public int PacketId;
        public AC_STATUS Status;
        public AC_SESSION_TYPE Session;
        [MarshalAs (UnmanagedType.ByValTStr, SizeConst = 15)]
        public String CurrentTime;
        [MarshalAs (UnmanagedType.ByValTStr, SizeConst = 15)]
        public String LastTime;
        [MarshalAs (UnmanagedType.ByValTStr, SizeConst = 15)]
        public String BestTime;
        [MarshalAs (UnmanagedType.ByValTStr, SizeConst = 15)]
        public String Split;
        public int CompletedLaps;
        public int Position;
        public int iCurrentTime;
        public int iLastTime;
        public int iBestTime;
        public float SessionTimeLeft;
        public float DistanceTraveled;
        public int IsInPit;
        public int CurrentSectorIndex;
        public int LastSectorTime;
        public int NumberOfLaps;
        [MarshalAs (UnmanagedType.ByValTStr, SizeConst = 33)]
        public String TyreCompound;

        public float ReplayTimeMultiplier;
        public float NormalizedCarPosition;

        public int ActiveCars;

        // Fix float carCoordinates[60][3];
        [MarshalAs (UnmanagedType.ByValArray, SizeConst = 180)]
        public float[] CarCoordinates;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 60)]
        public int[] CarId;

        public int PlayerCarId;

        public float PenaltyTime;
        public AC_FLAG_TYPE Flag;
        public AC_PENALTY Penalty;

        public int IdealLineOn;

        // since 1.5
        public int IsInPitLane;
        public float SurfaceGrip;
    
        // since 1.13
        public int MandatoryPitDone;

        public float WindSpeed;
        public float WindDirection;


        public int IsSetupMenuVisible;

        public int MainDisplayIndex;
        public int SecondaryDisplayIndex;
        public int TC;
        public int TCCut;
        public int EngineMap;
        public int ABS;
        public int FuelXLap;
        public int RainLights;
        public int FlashingLights;
        public int lightsStage;
        public float ExhaustTemperature;
        public int WiperLV;
        public int DriverStintTotalTimeLeft;
        public int DriverStintTimeLeft;
        public int RainTyres;
        int SessionIndex;
        public float UsedFuel;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 15)]
        public String deltaLapTime;
        public int iDeltaLapTime;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 15)]
        public String estimatedLapTime;
        public int iEstimatedLapTime;
        public int isDeltaPositive;
        public int iSplit;
        public int isValidLap;
        public float FuelEstimatedLaps;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 33)]
        public String trackStatus;
        public int MissingMandatoryPits;
        public int directionLightsLeft;
        public int directionLightsRight;
    }
}
