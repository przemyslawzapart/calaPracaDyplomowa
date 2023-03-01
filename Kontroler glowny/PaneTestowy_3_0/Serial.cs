using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.Drawing;

namespace PaneTestowy_3_0
{
    class Serial
    {
        public ComboBox portCom { get; set; }
        public ComboBox baudRate { get; set; }
        public Button refresh { get; set; }
        public Button connect { get; set; }
        public Button disconnect { get; set; }
        public GroupBox groupbox { get; set; }
        public Label status { get; set; }
        public SerialPort port { get; set; }
        public bool connected { get; set; }

        public Serial (ComboBox portCom, ComboBox baudRate, Button refresh, 
                        Button connect, Button disconnect, GroupBox groupbox, Label status, SerialPort port)
        {
            this.portCom = portCom;
            this.baudRate = baudRate;
            this.refresh = refresh;
            this.connect = connect;
            this.disconnect = disconnect;
            this.groupbox = groupbox;
            this.status = status;
            this.port = port;
            connected = false;
        }
        public void makeConnestion()
        {
            try
            {
                
                if (portCom.Text == "" || baudRate.Text == "")
                {
                    status.Text = "Wrong settings";
                    status.BackColor = Color.Red;
                }

                else
                {
                    port.PortName = portCom.Text;
                    port.BaudRate = Convert.ToInt32(baudRate.Text);
                    port.Open();
                    groupbox.Enabled = true;
                    status.Text = "Connected";
                    status.BackColor = Color.LightGreen;
                    connect.Enabled = false;
                    disconnect.Enabled = true;
                    connected = true;
                    
                }
            }
            catch (Exception ex)
            {
                status.Text = "Error";
                status.BackColor = Color.Red;
                Console.WriteLine(ex);
            }

        }

        public void closeConnection()
        {
            port.Close();
            status.Text = "Disconected";
            status.BackColor = Color.Transparent;
            disconnect.Enabled = false;
            connect.Enabled = true;
            connected = false;
        }
        public void getSerialPorts()
        {
            portCom.Items.Clear();
            String[] ports = SerialPort.GetPortNames();
            portCom.Items.AddRange(ports);
        }
    }
}
