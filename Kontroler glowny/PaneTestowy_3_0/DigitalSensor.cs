using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PaneTestowy_3_0
{
    internal class DigitalSensor
    {
        public string name { get; set; }
        public bool state { get; set; } 
        public bool enabled { get; set; }
        public Color colorOn { get; set; }
        public Color colorOff { get; set; }
        public Label label { get; set; }
        public Button button { get; set; }

        public DigitalSensor(Label label, Button button)
        {
            this.button = button;
            this.label = label;            

            name = "digital ingangg";
            state = false;
            enabled = true;   
            colorOff = Color.White;
            colorOn = Color.Red;
            this.button.BackColor = colorOff;
            this.label.Text = name;
            
        }
        public DigitalSensor(Label label, Button button, Color colorOn, Color colorOff)
        {
            this.button = button;
            this.label = label;

            name = "digital ingangg";
            state = false;
            enabled = true;
            this.colorOn = colorOn;
            this.colorOff = colorOff;
            this.button.BackColor = colorOff;
            this.label.Text = name;

        }
        public void setNewColors(Color colorOn, Color colorOff)
        {
            this.colorOn = colorOn;
            this.colorOff = colorOff;   
        }
        public void changeState(bool buttonState)
        {
            
            if (buttonState)
                button.BackColor = colorOn;
            else
                button.BackColor = colorOff;
            state = buttonState;

        }
        public void setNewState(bool newState) 
        {            
            if (newState != state)
                changeState(newState);
        }
        public void setName(string name)
        {
            label.Text = name;
            this.name = name;
        }
    }
}
