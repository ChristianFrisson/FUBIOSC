using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using System.Threading;
using System.Windows.Threading;

using FubiNET;

namespace Fubi_WPF_GUI
{
    /// <summary>
    /// Interaktionslogik für MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private static byte[] buffer = new byte[4 * 640 * 480];

        private Thread fubiThread;
        private bool running = false;

        bool clearRecognizersOnNextUpdate = false;
        bool switchSensorOnNextUpdate = false;
        
        private bool controlMouse = false;

        private Dictionary<uint, bool> currentPostures  = new Dictionary<uint,bool>();
        private bool[] currentPostures1 = new bool[(int)FubiPredefinedGestures.Postures.NUM_POSTURES];

        private FubiMouse FubiMouse = new FubiMouse();
        KeyboardListener kListener = new KeyboardListener();

        private delegate void NoArgDelegate();

        public MainWindow()
        {
            InitializeComponent();

            string[] mods = Enum.GetNames(typeof(FubiUtils.DepthImageModification));
            string selectedName = Enum.GetName(typeof(FubiUtils.DepthImageModification), FubiUtils.DepthImageModification.UseHistogram);
            foreach (string mod in mods)
            {
                int index = comboBox1.Items.Add(mod);
                if (index > -1 && mod == selectedName)
                {
                    comboBox1.SelectedIndex = index;
                }
            }
            
            fubiThread = new Thread(fubiMain);
            running = true;
            fubiThread.Start();
            kListener.KeyUp += new RawKeyEventHandler(this.keyPressedHandler);
        }

        private void initFubi()
        {
            List<string> availableSensors = new List<string>();
            availableSensors.Add(Enum.GetName(typeof(FubiUtils.SensorType), FubiUtils.SensorType.NONE));
            FubiUtils.SensorType type = FubiUtils.SensorType.NONE;
            int avSensors = Fubi.getAvailableSensors();
            if ((avSensors & (int)FubiUtils.SensorType.OPENNI2) != 0)
            {
                type = FubiUtils.SensorType.OPENNI2;
                availableSensors.Add(Enum.GetName(typeof(FubiUtils.SensorType), type));
            }
            if ((avSensors & (int)FubiUtils.SensorType.KINECTSDK) != 0)
            {
                if (type == FubiUtils.SensorType.NONE)
                    type = FubiUtils.SensorType.KINECTSDK;
                availableSensors.Add(Enum.GetName(typeof(FubiUtils.SensorType), FubiUtils.SensorType.KINECTSDK));
            }
            if ((avSensors & (int)FubiUtils.SensorType.OPENNI1) != 0)
            {
                if (type == FubiUtils.SensorType.NONE)
                    type = FubiUtils.SensorType.OPENNI1;
                availableSensors.Add(Enum.GetName(typeof(FubiUtils.SensorType), FubiUtils.SensorType.OPENNI1));
            }
            string selectedName = Enum.GetName(typeof(FubiUtils.SensorType), type);

            foreach (string sType in availableSensors)
            {
                int index = sensorSelectionComboBox.Items.Add(sType);
                if (index > -1 && sType == selectedName)
                {
                    sensorSelectionComboBox.SelectedIndex = index;
                }
            }
            switchSensorOnNextUpdate = false;
                
            Fubi.init(new FubiUtils.SensorOptions(new FubiUtils.StreamOptions(640, 480, 30), new FubiUtils.StreamOptions(640, 480), new FubiUtils.StreamOptions(-1, -1, -1), type));
            Fubi.loadRecognizersFromXML("MouseControlRecognizers.xml");
        }

        private void releaseFubi()
        {
            Fubi.release();
        }

        private void updateFubi()
        {
            if (clearRecognizersOnNextUpdate)
            {
                Fubi.clearUserDefinedRecognizers();
                if (Fubi.getNumUserDefinedCombinationRecognizers() == 0 && Fubi.getNumUserDefinedRecognizers() == 0)
                {
                    button3.IsEnabled = false;
                }
                clearRecognizersOnNextUpdate = false;
            }

            if (switchSensorOnNextUpdate)
            {
                Fubi.switchSensor(new FubiUtils.SensorOptions(new FubiUtils.StreamOptions(), new FubiUtils.StreamOptions(), new FubiUtils.StreamOptions(-1, -1, -1),
                        (FubiUtils.SensorType)Enum.Parse(typeof(FubiUtils.SensorType), sensorSelectionComboBox.SelectedItem.ToString())));
                switchSensorOnNextUpdate = false;
            }

            label1.Content = "User Count : " + Fubi.getNumUsers().ToString();

            // Update Fubi and get the debug image
            int width = 0, height = 0;
            FubiUtils.ImageNumChannels channels = FubiUtils.ImageNumChannels.C4;
            FubiUtils.ImageType type = FubiUtils.ImageType.Depth;
            
            uint renderOptions = 0;
            if (shapeCheckBox.IsChecked == true)
                renderOptions |= (uint)FubiUtils.RenderOptions.Shapes;
            if (skeletonCheckBox.IsChecked == true)
                renderOptions |= (uint)FubiUtils.RenderOptions.Skeletons;
            if (userCaptionscheckBox.IsChecked == true)
                renderOptions |= (uint)FubiUtils.RenderOptions.UserCaptions;
            if (localOrientCheckBox.IsChecked == true)
                renderOptions |= (uint)FubiUtils.RenderOptions.LocalOrientCaptions;
            if (globalOrientCheckBox.IsChecked == true)
                renderOptions |= (uint)FubiUtils.RenderOptions.GlobalOrientCaptions;
            if (localPosCheckBox.IsChecked == true)
                renderOptions |= (uint)FubiUtils.RenderOptions.LocalPosCaptions;
            if (globalPosCheckBox.IsChecked == true)
                renderOptions |= (uint)FubiUtils.RenderOptions.GlobalPosCaptions;
            if (backgroundCheckBox.IsChecked == true)
                renderOptions |= (uint)FubiUtils.RenderOptions.Background;
            if (swapRAndBcheckBox.IsChecked == true)
                renderOptions |= (uint)FubiUtils.RenderOptions.SwapRAndB;
            if (fingerShapecheckBox.IsChecked == true)
                renderOptions |= (uint)FubiUtils.RenderOptions.FingerShapes;
            if (detailedFaceCheckBox.IsChecked == true)
                renderOptions |= (uint)FubiUtils.RenderOptions.DetailedFaceShapes;
            if (bodyMeasuresCheckBox.IsChecked == true)
                renderOptions |= (uint)FubiUtils.RenderOptions.BodyMeasurements;

            FubiUtils.DepthImageModification depthMods = (FubiUtils.DepthImageModification) Enum.Parse(typeof(FubiUtils.DepthImageModification), comboBox1.SelectedItem.ToString(), true);
            
            if (checkBox3.IsChecked == true)
            {
                Fubi.getRgbResolution(out width, out height);
                channels = FubiUtils.ImageNumChannels.C3;
                type = FubiUtils.ImageType.Color;
            }
            else
            {
                Fubi.getDepthResolution(out width, out height);
                channels = FubiUtils.ImageNumChannels.C4;
                type = FubiUtils.ImageType.Depth;
            }
            
            // Display the debug image
            if (width > 0 && height > 0)
            {
                WriteableBitmap wb = image1.Source as WriteableBitmap;
                if (wb != null && (wb.Width != width || wb.Height != height || wb.Format.BitsPerPixel != (int)channels * 8))
                {
                    wb = null;
                    image1.Width = width;
                    image1.Height = height;
                    buffer = new byte[(int)channels * width * height];
                }
                if (wb == null)
                {
                    PixelFormat format = PixelFormats.Bgra32;
                    if (channels == FubiUtils.ImageNumChannels.C3)
                        format = PixelFormats.Rgb24;
                    else if (channels == FubiUtils.ImageNumChannels.C1)
                        format = PixelFormats.Gray8;
                    wb = new WriteableBitmap(width, height, 0, 0, format, BitmapPalettes.Gray256);
                    image1.Source = wb;
                }

                Fubi.updateSensor();
                Fubi.getImage(buffer, type, channels, FubiUtils.ImageDepth.D8, renderOptions, depthMods);

                int stride = wb.PixelWidth * (wb.Format.BitsPerPixel / 8);
                wb.WritePixels(new Int32Rect(0, 0, wb.PixelWidth, wb.PixelHeight), buffer, stride, 0);
            }
            
            //Check postures for all users
            for (uint i = 0; i < Fubi.getNumUsers(); i++)
            {
                uint id = Fubi.getUserID(i);
                if (id > 0)
                {
                    bool changedSomething = false;
                    // Print postures
                    if (checkBox1.IsChecked == true)
                    {
                        // Only user defined postures
                        for (uint p = 0; p < Fubi.getNumUserDefinedRecognizers(); ++p)
                        {
                            if (Fubi.recognizeGestureOn(p, id) == FubiUtils.RecognitionResult.RECOGNIZED)
                            {
                                // Posture recognized
                                if (!currentPostures.ContainsKey(p) || !currentPostures[p])
                                {
                                    // Posture start
                                    textBox1.Text += "User" + id + ": START OF " + Fubi.getUserDefinedRecognizerName(p) + " -->\n";
                                    currentPostures[p] = true;
                                    changedSomething = true;
                                }
                            }
                            else if (currentPostures.ContainsKey(p) && currentPostures[p])
                            {
                                // Posture end
                                textBox1.Text += "User" + id + ": --> END OF " + Fubi.getUserDefinedRecognizerName(p) + "\n";                                
                                currentPostures[p] = false;
                                changedSomething = true;
                            }
                        }

                        if (PredefinedCheckBox.IsChecked == true)
                        {
                            for (int p = 0; p < (int)FubiPredefinedGestures.Postures.NUM_POSTURES; ++p)
                            {
                                if (Fubi.recognizeGestureOn((FubiPredefinedGestures.Postures)p, id) == FubiUtils.RecognitionResult.RECOGNIZED)
                                {
                                    if (!currentPostures1[p])
                                    {
                                        // Posture recognized
                                        textBox1.Text += "User" + id + ": START OF" + FubiPredefinedGestures.getPostureName((FubiPredefinedGestures.Postures)p) + "\n";
                                        currentPostures1[p] = true;
                                        changedSomething = true;
                                    }
                                }
                                else if (currentPostures1[p])
                                {
                                    textBox1.Text += "User" + id + ": --> END OF " + FubiPredefinedGestures.getPostureName((FubiPredefinedGestures.Postures)p) + "\n";
                                    currentPostures1[p] = false;
                                    changedSomething = true;
                                }
                            }
                        }

                        if (changedSomething)
                            textBox1.ScrollToEnd();
                    }

                    // Print combinations
                    for (uint pc = 0; pc < Fubi.getNumUserDefinedCombinationRecognizers(); ++pc)
                    {
                        // Only user defined postures
                        if (checkBox2.IsChecked == true)
                        {
                            if (Fubi.getCombinationRecognitionProgressOn(Fubi.getUserDefinedCombinationRecognizerName(pc), id) == FubiUtils.RecognitionResult.RECOGNIZED)
                            {
                                // Posture recognized
                                textBox2.Text += "User" + id + ": " + Fubi.getUserDefinedCombinationRecognizerName(pc) + "\n";
                            }
                            else
                                Fubi.enableCombinationRecognition(Fubi.getUserDefinedCombinationRecognizerName(pc), id, true);
                        }
                        //else
                        //    Fubi.enableCombinationRecognition(Fubi.getUserDefinedCombinationRecognizerName(pc), id, false);
                    }

                    for (uint pc = 0; pc < (uint)FubiPredefinedGestures.Combinations.NUM_COMBINATIONS; ++pc)
                    {
                        if (checkBox2.IsChecked == true && PredefinedCheckBox.IsChecked == true)
                        {
                            if (Fubi.getCombinationRecognitionProgressOn((FubiPredefinedGestures.Combinations)pc, id) == FubiUtils.RecognitionResult.RECOGNIZED)
                            {
                                // Posture recognized
                                textBox2.Text += "User" + id + ": " + FubiPredefinedGestures.getCombinationName((FubiPredefinedGestures.Combinations)pc) + "\n";
                            }
                            else
                                Fubi.enableCombinationRecognition((FubiPredefinedGestures.Combinations)pc, id, true);
                        }
                        //else
                        //    Fubi.enableCombinationRecognition((FubiPredefinedGestures.Combinations)pc, id, false);
                    }
                    if (checkBox2.IsChecked == true)
                        textBox2.ScrollToEnd();
                }
            }

            uint closestId = Fubi.getClosestUserID();
            if (closestId > 0)
            {
                // For printing the user orientation
                //float[] mat = new float[9];
                //float confidence;
                //double timeStamp;
                //Fubi.getCurrentSkeletonJointOrientation(closestId, FubiUtils.SkeletonJoint.Torso, mat, out confidence, out timeStamp);
                //float rx, ry, rz;
                //FubiUtils.Math.rotMatToRotation(mat, out rx, out ry, out rz);
                //label1.Content = "UserOrient:" + String.Format("{0:0.#}", rx) + "/" + String.Format("{0:0.#}", ry) + "/" + String.Format("{0:0.#}", rz);


                if (controlMouse)
                {
                    float x, y;
                    FubiMouse.applyHandPositionToMouse(closestId, out x, out y, leftHandRadioButton.IsChecked == true);
                    label2.Content = "X:" + x + " Y:" + y;
                }
                
                if (checkBox4.IsChecked == true)
                {
                    FubiPredefinedGestures.Combinations activationGesture = FubiPredefinedGestures.Combinations.WAVE_RIGHT_HAND_OVER_SHOULDER;
                    // TODO use left hand waving
                    if (Fubi.getCombinationRecognitionProgressOn(activationGesture, closestId, false) == FubiUtils.RecognitionResult.RECOGNIZED)
                    {
                        if (controlMouse)
                            stopMouseEmulation();
                        else
                            startMouseEmulation();
                    }
                    else
                    {
                        Fubi.enableCombinationRecognition(activationGesture, closestId, true);
                    }
                }
            }
        }

       
        private void fubiMain()
        {
            this.Dispatcher.BeginInvoke(new NoArgDelegate(this.initFubi), null);

            DispatcherOperation updateOp = null;
            while (running)
            {
                updateOp = this.Dispatcher.BeginInvoke(new NoArgDelegate(this.updateFubi), null);
                Thread.Sleep(29); // Time it should at least take to get new data
                while (updateOp.Status != DispatcherOperationStatus.Completed
                    && updateOp.Status != DispatcherOperationStatus.Aborted)
                {
                    Thread.Sleep(2); // If the update unexpectedly takes longer
                }
            }

            this.Dispatcher.BeginInvoke(new NoArgDelegate(this.releaseFubi), null);
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            running = false;

            kListener.Dispose();

            fubiThread.Join(700);
            this.Dispatcher.InvokeShutdown();
        }

        private void button1_Click(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();
            dlg.FileName = "SampleRecognizers"; // Default file name 
            dlg.DefaultExt = ".xml"; // Default file extension 
            dlg.Filter = "MXL documents (.xml)|*.xml"; // Filter files by extension 

            // Show open file dialog box 
            Nullable<bool> result = dlg.ShowDialog();

            // Process open file dialog box results 
            if (result == true && dlg.FileName != null)
            {
                // Open document 
                Fubi.loadRecognizersFromXML(dlg.FileName);

                // Enable clear button if we have loaded some recognizers
                button3.IsEnabled = (Fubi.getNumUserDefinedCombinationRecognizers() > 0 || Fubi.getNumUserDefinedRecognizers() > 0);
            }
        }

        private void button2_Click(object sender, RoutedEventArgs e)
        {
            // Enable/Disable mouse emulation
            if (controlMouse)
            {
                stopMouseEmulation();
            }
            else
            {
                startMouseEmulation();
            }
        }

        private void keyPressedHandler(object sender, RawKeyEventArgs args)
        {
            // ESC stops mouse emulation
            if (args.Key == Key.Escape)
            {
                this.Dispatcher.BeginInvoke(new NoArgDelegate(this.stopMouseEmulation), null);
            }
        }

        private void startMouseEmulation()
        {
            controlMouse = true;
            button2.Content = "Stop MouseEmu (ESC)";
            label2.Content = "X:0 Y:0";
            FubiMouse.autoCalibrateMapping(leftHandRadioButton.IsChecked == true);
        }

        private void stopMouseEmulation()
        {
            controlMouse = false;
            button2.Content = "Start Mouse Emulation";
            label2.Content = "";
        }

        private void button3_Click(object sender, RoutedEventArgs e)
        {
            // Clear user defined recognizers
            clearRecognizersOnNextUpdate = true;
        }

        private void button4_Click(object sender, RoutedEventArgs e)
        {
            Fubi.resetTracking();
        }

        private void sensorSelectionComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            switchSensorOnNextUpdate = true;
        }
    }
}