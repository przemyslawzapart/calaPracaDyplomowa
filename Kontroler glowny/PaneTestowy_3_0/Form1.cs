using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Threading;
using System.Windows.Forms;
using Button = System.Windows.Forms.Button;
using ComboBox = System.Windows.Forms.ComboBox;
using TextBox = System.Windows.Forms.TextBox;
//https://in.pinterest.com/pin/200199145921297441/ - icona
namespace PaneTestowy_3_0
{
    public partial class Form1 : Form
    {
        List<Button> btnPosList = new List<Button>();

        String[] engineStateNamesArray = { "Out",      "Ready",     "Starting",       "Running",  "Error"};
        Color[] engineStateColorArray = {  Color.Gray, Color.Green, Color.GreenYellow,Color.Blue ,Color.Red};

        List<GroupBox> gbPanelList = new List<GroupBox>();

        List<TextBox> tbxGeneralSettingsList = new List<TextBox>();
        List<TextBox> tbxAnalogSettingsList = new List<TextBox>();
        List<TextBox> tbxDigitalSettingsList = new List<TextBox>();
        List<GroupBox> settingsPanelList = new List<GroupBox>();

        List<DigitalSensor> digitalSensorList = new List<DigitalSensor>();
        List<Button> digitalInButton = new List<Button>();
        List<Label> digitalInLabel = new List<Label>();

        List<DigitalSensor> digitalOutputList = new List<DigitalSensor>();
        List<Button> digitalOutButton = new List<Button>();
        List<Label> digitalOutLabel = new List<Label>();

        List<DigitalSensor> engineStateList = new List<DigitalSensor>();
        List<Button> engineStateButton = new List<Button>();
        List<Label> engineStateLabel = new List<Label>();

        List<AnalogSensor> analogSensorList = new List<AnalogSensor>();

        Serial mySerialPort;
        String incominngData;

        bool[] buttonState = new bool[3];//start stop reset
        bool[] keyStateArray = new bool[3];
        string password = "123";
        bool loginState = false;
        String softwareVersion="0";
        String SerialNumber="0";

        public Form1()
        {
            InitializeComponent();
           
            this.Size = new Size(695, 740);
            mySerialPort = new Serial(cbxSelectCom, cbxSelectBaudRate, btnSerialRefresh,
                    btnSerialConnect, btnSerialDisConnect, gbConnection, lblSerialStatus, serialPort1);
            mySerialPort.getSerialPorts();

            cbxSelectBaudRate.SelectedIndex = 1;

            btnPosList.Add(btnPosHand);
            btnPosList.Add(btnPosOff);
            btnPosList.Add(btnPosAuto);
            setBtnPos(1);
            setSettingsPanel(0);

            gbPanelList.Add(gbValue);
            gbPanelList.Add(gbSettings);            
            gbPanelList.Add(gbConnectionSettings);
            gbPanelList.Add(gbPswd);

            makeControlsList(tbxGeneralSettingsList, gbSettingsGeneral);
            makeControlsList(tbxAnalogSettingsList, gbSettingsAnalog);
            makeControlsList(tbxDigitalSettingsList, gbSettingsDigital);

            settingsPanelList.Add(gbSettingsGeneral);
            settingsPanelList.Add(gbSettingsAnalog);
            settingsPanelList.Add(gbSettingsDigital);

            analogSensorList.Add(new AnalogSensor(lblName1, lblAnalog1, lblUnit1, pbAnalog1));
            analogSensorList.Add(new AnalogSensor(lblName2, lblAnalog2, lblUnit2, pbAnalog2));
            analogSensorList.Add(new AnalogSensor(lblName3, lblAnalog3, lblUnit3, pbAnalog3));
            analogSensorList.Add(new AnalogSensor(lblName4, lblAnalog4, lblUnit4, pbAnalog4));

            makeControlsList(digitalInButton, gbDigitalInput);
            makeControlsList(digitalInLabel, gbDigitalInput);
 
            for (int i = 0; i < digitalInButton.Count; i++)
            {
                int newSize = 5;
                digitalInButton[i].Font = new Font(digitalInButton[i].Font.FontFamily, newSize);
                digitalSensorList.Add(new DigitalSensor(digitalInLabel[i], digitalInButton[i]));
                digitalSensorList[i].setName("Digital input");
            }

            makeControlsList(digitalOutButton, gbDigitalOutput);
            makeControlsList(digitalOutLabel, gbDigitalOutput);


            for (int i = 0; i < digitalOutButton.Count; i++)
            {
                int newSize = 5;
                digitalOutButton[i].Font = new Font(digitalOutButton[i].Font.FontFamily, newSize);
                digitalOutputList.Add(new DigitalSensor(digitalOutLabel[i], digitalOutButton[i], Color.Red, Color.LightCyan));
                digitalOutputList[i].setName("Digital output");
            }

            makeControlsList(engineStateButton, gbEngineMessage);
            makeControlsList(engineStateLabel, gbEngineMessage);


            for (int i = 0; i < engineStateButton.Count; i++)
            {
                int newSize = 5;
                engineStateButton[i].Font = new Font(digitalOutButton[i].Font.FontFamily, newSize);
                //engineStateButton[i].Text = i.ToString();
                //digitalOutLabel[i].BackColor = Color.AliceBlue;
                engineStateList.Add(new DigitalSensor(engineStateLabel[i], engineStateButton[i], Color.Red, Color.LightGreen));
                engineStateList[i].setName("Engine state");
            }


            makeControlsList(tbxAnalogSettingsList, gbSettingsAnalog);
            makeControlsList(tbxDigitalSettingsList, gbSettingsDigital);
            makeControlsList(tbxGeneralSettingsList, gbSettingsGeneral);

        }

        private void makeControlsList<T>(List<T> list, GroupBox groupBox)
        {
            List<T> tempList = groupBox.Controls.OfType<T>().ToList();
            int size = tempList.Count() - 1; // list 9 to 1
            for (int i = 0; i < tempList.Count(); i++) //reversing list (1 to 9)
            {
                list.Add(tempList[size - i]);
            }
        }
        private List<T> GetControlsInReverseOrder<T>(GroupBox groupBox)
        {
            List<T> tempList = groupBox.Controls.OfType<T>().ToList();
            tempList.Reverse();
            return tempList;
        }
        //----------------------------serial connection
        private void btnSerialRefresh_Click(object sender, EventArgs e)
        {
            mySerialPort.getSerialPorts();
        }

        private void btnSerialConnect_Click(object sender, EventArgs e)
        {
            
            mySerialPort.makeConnestion();
            if (mySerialPort.connected)
            {
                serialPort1.DiscardInBuffer(); // clear buffer
                timerSendData.Enabled = true;
                getAllData();

            }
        }

        private void btnSerialDisConnect_Click(object sender, EventArgs e)
        {
            mySerialPort.closeConnection();
            if (!mySerialPort.connected)
                timerSendData.Enabled = false;
        }

        private void sendSerialdata(string data)
        {
            
            if (mySerialPort.connected)
            {
                try
                {
                
                    serialPort1.Write(data);
                    serialPort1.Write("\n");
                    Console.WriteLine("Sending : {0}", data);

                }
                catch (TimeoutException)
                {
                    Console.WriteLine("nie mozna wysac");
                }
                catch (ArgumentNullException ex)
                {

                    Console.WriteLine("ArgumentNullException :{0}", ex);
                    MessageBox.Show("Exception", "ArgumentNullException", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                }
                catch (IOException ex)
                {

                    Console.WriteLine("Exception :{0}", ex);
                    MessageBox.Show("Urządzenie dołączone do komputera nie działa", "ArgumentNullException", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                }
                catch (InvalidOperationException ex)
                {
                    Console.WriteLine("InvalidOperationException  :{0}", ex);
                    // MessageBox.Show( "niepooprawna operacja." + "\nPleas check connection. ", "InvalidOperationException",MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                    mySerialPort.closeConnection();
                    if (!mySerialPort.connected)
                        timerSendData.Enabled = false;
                    //  while (mySerialPort.connected) {; };



                }
                catch (UnauthorizedAccessException ex)
                {
                    Console.WriteLine("UnauthorizedAccessException: „Odmowa dostępu do portu 'COM9'.”  :{0}", ex);
                    MessageBox.Show("Port Com : " + mySerialPort.portCom + " is closed or failid." + "\nPleas check connection. ", "UnauthorizedAccessException", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                    mySerialPort.closeConnection();
                    if (!mySerialPort.connected)
                        timerSendData.Enabled = false;
                    //     while (mySerialPort.connected) {; };


                }
            }
            else
                MessageBox.Show("No  conection !", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        }

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
                 
            //string indata = serialPort1.ReadExisting();
            try{
                string indata = serialPort1.ReadLine();
                var threadParameters = new ThreadStart(delegate { setString(indata); });
                var thread2 = new Thread(threadParameters);
                thread2.Start();//nowy wątek aby zapisać dane
            }
            catch(UnauthorizedAccessException ex)
            {
                Console.WriteLine("UnauthorizedAccessException: „Odmowa dostępu do portu 'COM9'.” :{0}", ex);
                MessageBox.Show("dsfdsg","cos nie dziala", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            }
            catch (IOException ex)
            {
                Console.WriteLine("IOException: „Odmowa dostępu do portu 'COM9'.” :{0}", ex);
                MessageBox.Show("bla", "cos nie dziala", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            }

        }
        private void setString(String data)
        {
            if (this.InvokeRequired)
            {
                
                try
                {
                    Action safeWrite = delegate { setString(data); };this.Invoke(safeWrite);
                }                
                catch(ObjectDisposedException ex)
                {
                    Console.WriteLine("ObjectDisposedException  : {0} ", ex);
                }
                
            }
            else
            {
                incominngData += data;
                if (incominngData.Contains("*"))
                {
                    checkRecivedData(incominngData);
                    incominngData = ""; 
                }    
            }
        }

        private void checkRecivedData(String data)
        {
            Console.WriteLine("otrzynamo data  : {0} ", data);
            data = data.Trim();
            if (!String.IsNullOrEmpty(data))
            {
                if (data[0] == '#')
                {
                    Console.WriteLine("otrzynamo values  : {0} ", data);
                    SetValues(data);
                }
                else if (data[0] == '$')
                {
                    Console.WriteLine("otrzynamo digital  : {0} ", data);
                    setNamesDigitalIn(data);
                    //setNames(data, digitalSensorList, '$');
                }
                else if (data[0] == '@')
                {
                    Console.WriteLine("otrzynamo analog  : {0} ", data);
                    setNamesAnalog(data);
                }
                else if (data[0] == '%')
                {
                    Console.WriteLine("otrzynamo srn ver  : {0} ", data);
                    setSerialNumberOrVersion(data);


                }
                else if (data[0] == '&')
                {
                    Console.WriteLine("otrzynamo out  : {0} ", data);
                    setNames(data, digitalOutputList, '&');
                    
                }
                else if (data[0] == '!')
                {
                    Console.WriteLine("otrzynamo engineState  : {0} ", data);
                    setNames(data, engineStateList, '!');
                }
            }                
        }
        private void setSerialNumberOrVersion(String data)
        {
            // %/S/222222222*
            // %/V/1.002*

            char[] separator = { '%', '/', '*' };
            string[] splitString = data.Split(separator);

            if (splitString.Length < 4)
                return;
            if (String.IsNullOrEmpty(splitString[2]))
                return;
            if (String.IsNullOrEmpty(splitString[3]))
                return;

            if (String.Compare(splitString[2],"S") == 0){
                SerialNumber = splitString[3];
            }else if (String.Compare(splitString[2],"V") ==0)
                softwareVersion = splitString[3];

        }

        private void setNamesDigitalIn(String data)
        {
            char[] separator = { '$', '/', '*' };
            string[] splitString = data.Split(separator);

            if (splitString.Length < 3)
                return;

            if (String.IsNullOrEmpty(splitString[2]))
                return;
            if (String.IsNullOrEmpty(splitString[3]))
                return;

            int id = getIntValue(splitString[2], 10);
            if (id < 0 || (id > digitalSensorList.Count() - 1))
                return;
            digitalSensorList[id].setName(splitString[3]);
            setNames(data, digitalSensorList, '$');

            getSensrosDigitalName(comboBox5);
            setDigitalSettings(id);
        }
        private void setNames(String data, List<DigitalSensor> list, char sign)
        {
            char[] separator = { sign, '/', '*' };
            string[] splitString = data.Split(separator);

            if (splitString.Length < 3)
                return;

            if (String.IsNullOrEmpty(splitString[2]))
                return;
            if (String.IsNullOrEmpty(splitString[3]))
                return;

            int id = getIntValue(splitString[2], 10);
            if (id < 0 || (id> list.Count() -1))
                return;
            list[id].setName(splitString[3]);

            if(sign == '&')
            {
                getSensrosDigitaOutlName(comboBox2);
                textBox1.Text = splitString[3];
            }
        }

        private void setNamesAnalog(String data)
        {
            char[] separator = { '@', '/', '*' };
            string[] splitString = data.Split(separator);
            if (splitString.Length < 10)
                return;
            if (String.IsNullOrEmpty(splitString[2])) //id
                return;
            if (String.IsNullOrEmpty(splitString[3])) //name
                return;
            if (String.IsNullOrEmpty(splitString[4])) //unit
                return;
            if (String.IsNullOrEmpty(splitString[5])) //min
                return;
            if (String.IsNullOrEmpty(splitString[6])) //max
                return;
            if (String.IsNullOrEmpty(splitString[7])) //range
                return;

            int id = getIntValue(splitString[2], 10);
            if (id < 0)
                return;
            int min = getIntValue(splitString[5], 10);
            if (min < 0)
                return;
            int max = getIntValue(splitString[6], 10);
            if (max < 0)
                return;
            int range = getIntValue(splitString[7], 10);
            if (range < 0)
                return;

            analogSensorList[id].setNewInstelingen(splitString[3], min, max, range, splitString[4]);
            getSensrosAnalogName(comboBox4);
            setAnalogSettings(id);

        }
        private void SetValues(String data)
        {
            char[] separator = { '#','/', '*' };
            string[] splitString = data.Split(separator);
            for (int i = 0; i < splitString.Length; i++)
            {
                Console.WriteLine("incomming splitstring {0} : {1}",i,splitString[i]);
            }

            if (splitString.Count() < 10)
                return;
            if (String.IsNullOrEmpty(splitString[10]))  //cos dalej tutaj nie dziala
                return;
            int checkSum = getIntValue(splitString[10]);
            if (checkSum < 0)
                return;
            if (checkSum != data.Length)
                return;

            if (!String.IsNullOrEmpty(splitString[1]))
                setDateTime(splitString[1]);
            else
                return;
            if (!String.IsNullOrEmpty(splitString[2]))
                setRpm(splitString[2]);
            else
                return;
            if (!String.IsNullOrEmpty(splitString[3]))
            {
                setSensorListState(engineStateList, splitString[3]);
                setNewEngineStatus();
            }
            else
                return;
            if (!String.IsNullOrEmpty(splitString[4]))
                setSensorListState(digitalSensorList, splitString[4]);
            else
                return;
            if (!String.IsNullOrEmpty(splitString[5]))
                setSensorListState(digitalOutputList, splitString[5]);
            else
                return;
            for (int i = 0; i < analogSensorList.Count; i++)
            {
                if (!String.IsNullOrEmpty(splitString[i + 6]))
                    setAnalogValue(splitString[i + 6], i);
                else 
                    return;
            }


        }
        private void setAnalogValue(String analogValue, int index)
        {
            double value = getIntValue(analogValue, 16);
            if (value < 0)
                return;
            value = value / 10;
            Console.WriteLine(value);
            analogSensorList[index].setValue(value);

        }

        private void setSensorListState(List<DigitalSensor> list, string incomingString)
        {
            int state = getIntValue(incomingString);
            if (state >=0 ) 
            {
                for (int i = 0; i < list.Count; i++)
                {
                    if ((state & (1 << i)) != 0)
                    {

                        list[i].setNewState(true);
                    }
                    else
                    {
                        list[i].setNewState(false);

                    }
                }
            }
            else
                Console.WriteLine("Can't convert to int value !!!!!");
        }
        private void setDigitalOutput(string output)
        {
            int state = Convert.ToInt32(output, 16);
            Console.WriteLine("digital input : {0} ", state);
        }

        



        private void setDateTime(String dateTime)
        {
            btnDateTime.Text = "Date / Time  : " + dateTime;
        }
        private void setRpm(String rpm)
        {
            btnRpm.Text = "RPM : " + rpm;
        }
        private void setNewEngineStatus()
        {            
            String state = "Status : ";// + engineStateNamesList[position];
            Color color  = engineStateColorArray[0];
            for (int i = 0; i < engineStateNamesArray.Count(); i++)
            {
                if (engineStateList[i].state)
                {
                   // state += engineStateNamesArray[i];
                   // color = engineStateColorArray[i];
                    btnEngineStatus.BackColor =  engineStateColorArray[i];
                    btnEngineStatus.Text = state +engineStateNamesArray[i];

                    break;
                }                    
            }

           // btnEngineStatus.BackColor = color;
           //btnEngineStatus.Text = state;
        }

        //get name form avr

        private void getAllData()
        {
            //tutaj wyslij zapytanie do czujki +
            sendData("@Q*");
        }

        //------------------------------timers

        private void timerSendData_Tick(object sender, EventArgs e)
        {
            sendButtonState();
        }
        private void loginTimer_Tick(object sender, EventArgs e)
        {
            loginTimer.Enabled = false;
            setloginSettingsState(false);
        }
        //----------------------------functions 

        int getIntValue(string value)
        {
            try
            {
                int number = Convert.ToInt32(value, 16);
                //Console.WriteLine("{0} --> {1}", value, number);
                return number;
            }
            catch (FormatException)
            {
                Console.WriteLine("{0}: FormatException", value);
            }
            catch (OverflowException)
            {
                Console.WriteLine("{0}: OverflowException", value);
            }
            return -1;
        }
        int getIntValue(string value, int baseNumber)
        {
            try
            {
                int number = Convert.ToInt32(value, baseNumber);
                Console.WriteLine("{0} --> {1}", value, number);
                return number;
            }
            catch (FormatException)
            {
                Console.WriteLine("{0}: FormatException", value);
            }
            catch (OverflowException)
            {
                Console.WriteLine("{0}: OverflowException", value);
            }
            return -1;
        }
        private void clearTbxList(List<TextBox> list)
        {
            foreach (var item in list)
            {
                item.Text = "";
            }
        }

        //--------------------------------set buttons
        private void btnStart_Click(object sender, EventArgs e)
        {
            buttonState[0] = true;
        }

        private void btnStop_Click(object sender, EventArgs e)
        {
            buttonState[1] = true;
        }

        private void btnReset_Click(object sender, EventArgs e)
        {
            buttonState[2] = true;
        }

        private void btnPosHand_Click(object sender, EventArgs e)
        {
            if (engineStateList[3].state)//if motor is running do nothing
                return;
            setBtnPos(0);
        }

        private void btnPosOff_Click(object sender, EventArgs e)
        {
            if (engineStateList[3].state)//if motor is running do nothing
                return;
            setBtnPos(1);           
        }

        private void btnPosAuto_Click(object sender, EventArgs e)
        {
            setBtnPos(2);           
        }

        private void setBtnPos(int pos)
        {
            int a = 0;
            foreach (var item in btnPosList)
            {
                if (a == pos)
                {
                    item.Enabled = false;
                    item.Size = new Size(110, 75);
                    item.BackColor = Color.DodgerBlue;
                    keyStateArray[a] = true;
                }
                else
                {
                    item.Enabled = true;
                    item.Size = new Size(100, 65);
                    item.BackColor = Color.LightSkyBlue;
                    keyStateArray[a] = false;
                }
                a++;
            }
            setState(pos);

        }

        private void setState(int pos)
        {
            if (pos == 0)//hand
            {
                setFontSize(btnStart, true);
                setFontSize(btnStop, true);

            }
            else if (pos == 1)//off
            {
                setFontSize(btnStart, false);
                setFontSize(btnStop, false);
            }
            else if (pos == 2)//auto
            {
                setFontSize(btnStart, false);
                setFontSize(btnStop, true);
            }
        }
        private void setFontSize(Button btn, bool state)
        {
            int newSizeSmall = 15;
            int newSizeLarge = 20;

            if (state)
            {
                btn.Enabled = true;
                btn.Font = new Font(btn.Font.FontFamily, newSizeLarge, FontStyle.Bold);
                btn.Size = new Size(220, 70);
            }
            else
            {
                btn.Enabled = false;
                btn.Font = new Font(btn.Font.FontFamily, newSizeSmall, FontStyle.Regular);
                btn.Size = new Size(210, 60);
            }

        }

        private void sendButtonState()
        {
            String data = "!";
            data += setState(buttonState);
            data += setState(keyStateArray);            
            data += "*";
           // Console.WriteLine(data);
            sendData(data);
        }

        private String setState(bool[] buttons)
        {
            int value = 0;
            String data;

            int i = 0;
            foreach (var item in buttons)
            {
                if (item)
                    value |= (1 << i);               
                i++;
            }
            data = value.ToString("X");
            return data;
        }      
        private void sendData(string data)
        {
            sendSerialdata(data);
        }

        private void btnStart_MouseDown(object sender, MouseEventArgs e)
        {
            //startButtonStatus = true;
            buttonState[0] = true;
            sendButtonState();
        }

        private void btnStart_MouseUp(object sender, MouseEventArgs e)
        {
            buttonState[0] = false;
            sendButtonState();
        }

        private void btnStop_MouseDown(object sender, MouseEventArgs e)
        {
            buttonState[1] = true;
            sendButtonState();
        }

        private void btnStop_MouseUp(object sender, MouseEventArgs e)
        {
            buttonState[1] = false;
            sendButtonState();
        }

        private void btnReset_MouseDown(object sender, MouseEventArgs e)
        {
            buttonState[2] = true;
            sendButtonState();
        }

        private void btnReset_MouseUp(object sender, MouseEventArgs e)
        {
            buttonState[2] = false;
            sendButtonState();
        }

       //--------------set buttons end 

        //set panels
        private void btnPanelValues_Click(object sender, EventArgs e)
        {
            setPanel(0);
        }

        private void btnPanelSettings_Click(object sender, EventArgs e)
        {
            setPanel(1);
            setSettingsPanel(0);
            tbxGeneralSettingsList[0].Text = SerialNumber;
            tbxGeneralSettingsList[1].Text = softwareVersion;

            // getSensrosAnalogName(comboBox1);//sensorList
        }

        private void btnConnectionSettings_Click(object sender, EventArgs e)
        {
            setPanel(2);
        }
         private void button8_Click(object sender, EventArgs e)
        {
            setPanel(3);
        }
        private void setPanel(int index)
        {
            int a = 0;
            foreach (var item in gbPanelList)
            {
                if (a++ == index)
                {
                    item.Visible = true;
                    item.Location = new Point(0, 100);
                }
                else
                {
                    item.Visible = false;
                }
            }
            if (loginState && index == 1)//zsalogowany
            {
                lblLogin.Visible = false;
                makeEnabledButtons(true);

            }
            else//nastapilo wylogowanie
            {
                loginState = false;
                makeSettingsPanelState(false);
                lblLogin.Visible = false;
                makeEnabledButtons(false);
            }

        }
        private void makeEnabledButtons(bool state)
        {
            btnSendAnalogSettings.Enabled = state;
            btnSendDigitalSettings.Enabled = state;
            btnSendGeneralSettings.Enabled = state;
        }
        private void makeSettingsPanelState(bool state)
        {
            makeCollectionState(tbxGeneralSettingsList, state);
            makeCollectionState(tbxAnalogSettingsList, state);
            makeCollectionState(tbxDigitalSettingsList, state);
        }
        private void makeCollectionState(List<TextBox> list, bool state)
        {
            foreach (var item in list)
            {
                item.Enabled = state;
            }

        }
        private void btnGeneralSettings_Click(object sender, EventArgs e)
        {
            setSettingsPanel(0);
            tbxGeneralSettingsList[0].Text = SerialNumber;
            tbxGeneralSettingsList[1].Text = softwareVersion;
        }
       

        private void btnDigitalSettings_Click(object sender, EventArgs e)
        {
            setSettingsPanel(2);
            getSensrosDigitalName(comboBox5);
            comboBox5.SelectedIndex = 0;
            string data = "@/D/0*";
            sendData(data);

            getSensrosDigitaOutlName(comboBox2);
            comboBox2.SelectedIndex = 0;
            textBox1.Text = comboBox2.Text;

        }
        private void setSettingsPanel(int pos)
        {
            Console.WriteLine(pos);
            int a = 0;
            foreach (var item in settingsPanelList)
            {
                if (a++ == pos)
                {
                    gbSettings.Controls.Add(item);
                    item.Location = new Point(155, 65);
                    item.Visible = true;

                }
                else
                {
                    item.Visible = false;
                }
            }
            if(pos == 1)
            {

                ;
            }
        }
        private void btnAnalogSettings_Click(object sender, EventArgs e)
        {
            setSettingsPanel(1);
            getSensrosAnalogName(comboBox4);//sensorList
            comboBox4.SelectedIndex = 0;    
        }

        private void getSensrosAnalogName(ComboBox cbox)
        {
            cbox.Items.Clear();
            foreach (var item in analogSensorList)
            {
                cbox.Items.Add(item.name);
            }
        }
        private void setAnalogSettings(int id)
        {
            tbxAnalogSettingsList[0].Text = analogSensorList[id].name;
            tbxAnalogSettingsList[1].Text = analogSensorList[id].unit;
            tbxAnalogSettingsList[2].Text = analogSensorList[id].minValue.ToString();
            tbxAnalogSettingsList[3].Text = analogSensorList[id].maxValue.ToString();
            tbxAnalogSettingsList[4].Text = analogSensorList[id].maxRange.ToString();

        }
        private void setDigitalSettings(int id)
        {
            tbxDigitalSettingsList[0].Text = digitalSensorList[id].name;   
        }
        private void getSensrosDigitalName(ComboBox cbox)
        {
            cbox.Items.Clear();
            foreach (var item in digitalSensorList)
                cbox.Items.Add(item.name);
        }
        private void getSensrosDigitaOutlName(ComboBox cbox)
        {
            cbox.Items.Clear();
            foreach (var item in digitalOutputList)
                cbox.Items.Add(item.name);
        }
        private void bntLogin_Click(object sender, EventArgs e)
        {
            if (!loginState)
            {
                setPanel(3);
                lblPswd.Visible = false;

            }

            else
            {
                lblLogin.Visible = true;
                
            }
               
        }

        private void btnPswdCancel_Click(object sender, EventArgs e)
        {
            setPanel(1);
        }

        private void btnPswdReset_Click(object sender, EventArgs e)
        {
            tbPswd.Text = "";
            lblPswd.Visible = false;
        }

        private void btnPswdLogin_Click(object sender, EventArgs e)
        {
            if (String.Compare(password, tbPswd.Text) == 0)
            {
                tbPswd.Text = "123";
                setloginSettingsState(true);
                setPanel(1);
            }
            else
            {
                lblPswd.Visible = true; 
            }
        }
        private void setloginSettingsState(bool state)
        {
            if (state)
                loginTimer.Enabled = true;
            loginState = state;
            makeSettingsPanelState(state);
        }

        private void btnSendGeneralSettings_Click(object sender, EventArgs e)
        {
            String data = "#/S/" + tbxserialNumber.Text + "/*";
            sendData(data);
        }

        private void btnSendAnalogSettings_Click(object sender, EventArgs e)
        {
            string textToSend = comboBox4.Text;
            int index = comboBox4.SelectedIndex;
            int min = getIntValue(tbxAnalogSettingsList[2].Text, 10);
            int max = getIntValue(tbxAnalogSettingsList[3].Text, 10);
            int range = getIntValue(tbxAnalogSettingsList[4].Text, 10);

            if (min < 0 || max < 0 || range < 0 || range <= max || max <= min || range >100)
            {
                lblSettingsMessage.Visible = true; 
                lblSettingsMessage.Text = "Wrong settings";
                return;
            }
            lblSettingsMessage.Visible = false;
            
            String dataToSend = "#A/"+ index.ToString() + "/" + tbxAnalogSettingsList[0].Text + "/"+ tbxAnalogSettingsList[1].Text + "/"+ tbxAnalogSettingsList[2].Text
                                        + "/"+ tbxAnalogSettingsList[3].Text+ "/"+ tbxAnalogSettingsList[4].Text + "*";

            sendData(dataToSend);            
            comboBox4.Items.Clear();
            comboBox4.Items.Add("Select sensor");
            comboBox4.SelectedIndex = 0;
            clearTbxList(tbxAnalogSettingsList);
        }
        private void btnSendDigitalSettings_Click(object sender, EventArgs e)
        {
            String dataToSend = "#D/" + comboBox5.SelectedIndex.ToString() + "/" + tbxDigitalSettingsList[0].Text + "/0/*";
            sendData(dataToSend);
            comboBox5.Items.Clear();
            comboBox5.Items.Add("Select sensor");
            comboBox5.SelectedIndex = 0;
            textBox27.Text = "";
           // clearTbxList(tbxDigitalSettingsList);

        }

        private void btnGetAnalogValues_Click(object sender, EventArgs e)
        {
            getAllData();
        }

        private void btnSendDefoultAnalogSettings_Click(object sender, EventArgs e)
        {
            string data = "#/Q/*";
            sendData(data);
        }

        private void comboBox4_SelectedIndexChanged(object sender, EventArgs e)
        {
                clearTbxList(tbxAnalogSettingsList);
                setAnalogSettings(comboBox4.SelectedIndex);
                if (loginState)
                {
                    if(comboBox4.SelectedIndex < 2) 
                    { 
                        tbxAnalogSettingsList[0].Enabled = false;
                        tbxAnalogSettingsList[1].Enabled = false;
                    }
                    else 
                    {
                        tbxAnalogSettingsList[0].Enabled = true;
                        tbxAnalogSettingsList[1].Enabled = true;
                    }
                    
                }
            
        }

        private void comboBox5_SelectedIndexChanged(object sender, EventArgs e)
        {
            textBox27.Text = comboBox5.Text;
            if (loginState)
            {
                if (comboBox5.SelectedIndex < 8)
                    textBox27.Enabled = false;
                else
                    textBox27.Enabled = true;
            }

        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (MessageBox.Show("Are you sure you want to Quit?", "Control panel",MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation) == DialogResult.No)
                e.Cancel = true;
            else
            {
                mySerialPort.closeConnection();
                if (!mySerialPort.connected)
                    timerSendData.Enabled = false;
                while (mySerialPort.connected) { };
            }
                
            
            
        }

        private void comboBox2_SelectedIndexChanged(object sender, EventArgs e)
        {            
            textBox1.Text = comboBox2.Text;
            if (loginState)
            {
                if (comboBox2.SelectedIndex < 5)
                    textBox1.Enabled = false;
                else
                    textBox1.Enabled = true;

            }

        }

        private void btnSendDigitalOutSettings_Click(object sender, EventArgs e)
        {
            String dataToSend = "#O/" + comboBox2.SelectedIndex.ToString() + "/" + textBox1.Text + "/0/*";
            sendData(dataToSend);
            comboBox2.Items.Clear();
            comboBox2.Items.Add("Select sensor");
            comboBox2.SelectedIndex = 0;
        }
    }


}
