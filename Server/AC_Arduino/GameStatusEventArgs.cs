using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

/*
// https://github.com/mdjarv/assettocorsasharedmemory
*/

namespace AssettoCorsaSharedMemory
{
    public class GameStatusEventArgs : EventArgs
    {
        public AC_STATUS GameStatus {get; private set;}

        public GameStatusEventArgs(AC_STATUS status)
        {
            GameStatus = status;
        }
    }
}
