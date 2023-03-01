using System;
using System.Collections.Generic;
using System.Configuration;
using System.Drawing;
using System.Linq;
using System.Security.Cryptography;
using System.Security.Policy;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PaneTestowy_3_0
{
    internal class AnalogSensor
    {
        public double value { get; set; }   
        public string name { get; set; }    
        public int minValue { get; set; }
        public int maxValue { get; set; }
        public string unit { get; set; }
        public int maxRange { get; set; }
        public Label lblValue { get; set; }
        public Label lblUnit { get; set; }
        public Label lblName { get; set; }
        public ProgressBar pbProgres{ get; set; }


        public bool enabled { get; set; }   

        public AnalogSensor(Label _name, Label _value, Label _unit, ProgressBar _progres)
        {
            
            this.value = 0;
            minValue = 20;
            maxValue = 70;
            name = _name.Text;
            unit = "V";
            enabled = true;
            lblValue = _value;
            lblUnit = _unit;    
            lblName = _name;
            pbProgres = _progres;
            maxRange = 100;

            lblName.Text = name;
            lblValue.Text = value.ToString();
            lblUnit.Text = unit.ToString();
            pbProgres.Maximum = maxRange;
            pbProgres.Value = (int)value;

        }
        public void setValue(double value)
        {
            this.value = value;
            setValue(); 
        }
        public void setValue()
        {
            setAnalogColer();
            double a = ((100 / (float)maxRange) * value);
            if (a < maxRange)
                pbProgres.Value = (int)a;
            pbProgres.Value = (int)value;
            lblValue.Text = value.ToString();
        }
        public void setValue(String valueStr)
        {
            value = Convert.ToInt32(valueStr, 16);
            value = value / 10;
            setValue();
        }
        private void setAnalogColer()
        {
            if (value < minValue)
                pbProgres.ForeColor = Color.Yellow;
            else if (value > maxValue)
                pbProgres.ForeColor = Color.Red;
            else
                pbProgres.ForeColor = Color.Green;
        }
        public void setNewInstelingen(string _name, int _minValue, int _maxValue, int _range,  string _unit)
        {
            maxRange = _range;
            maxValue = _maxValue;
            minValue = _minValue ;
            name = _name;
            lblName.Text = name;
            unit = _unit;
            lblUnit.Text = unit;
            pbProgres.Maximum = _range;
            Console.WriteLine(value);
            setValue();
        }

    }
}
