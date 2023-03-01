using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;

namespace symulacja_sygnalow_Windows
{
    internal class DigitalSensor
    {
        private bool state;
        public Button btn;
        private static int currentId;
        public int id { get; }

        public DigitalSensor(Button button)
        {

            state = true;
            btn = button;
            changeState();
            id = currentId++;


        }   

        public void setState(bool newState)
        {
            state = newState;
        }
        public void changeState()
        {
            if (state)
            {       
                btn.BackColor = Color.Gray;
                state = false;
            }
            else
            {
                btn.BackColor = Color.Red;
                state = true;
            }
           

        }
        public bool getState()
        {
            return state;
        }
    }
}
