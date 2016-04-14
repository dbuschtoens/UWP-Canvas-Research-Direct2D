using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using NewCPPComponent;
//

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace SurfaceImageSourceCPPExample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();
            //var text = new TextBlock() {
            //    Text = "HelloWorld",
            //    Width = 400,
            //    Height = 100
            //};
            //Background.Children.Add(text);

            //var Direct2DImageSource = new SurfaceImageSourceCPP((int) image.Width,(int) image.Height,false);
            //Direct2DImageSource.doSomething();
            //image.Source = Direct2DImageSource;
            //Direct2DImageSource.DrawSmiley();
            var instance = new Direct2DImageSource((int) image2.Width, (int) image2.Height, false);
            image2.Source = instance;
            //instance.CreateD2DFactory();
            //instance.CreateD3DDevice();
            //instance.CreateDXGIDevice();
            //instance.CreateD2DDevice();
            //instance.CreateD2DContext();

            //instance.SetDxgiDeviceAsNativeImageSource();
            instance.BeginDraw();
            instance.DrawSmiley();
            instance.EndDraw();

            //Background.Children.Add(image1);

            //SwapPanelProperties1:
            //Grid.Column = "1"
            //HorizontalAlignment = "Left"
            //Height = "545"
            //Margin = "943,357,0,0"
            //VerticalAlignment = "Top"
            //Width = "381"
            //RenderTransformOrigin = "0.51,0.173" />

            //SwapPanelProperties2:
            //Grid.Column = "1"
            //HorizontalAlignment = "Left"
            //Height = "545"
            //Margin = "1334,357,0,0"
            //VerticalAlignment = "Top"
            //Width = "381"
            //RenderTransformOrigin = "0.51,0.173" />




        }

        private void textBox_TextChanged(object sender, TextChangedEventArgs e) {

        }

        private void textBox4_TextChanged(object sender, TextChangedEventArgs e) {

        }

        private void textBox1_TextChanged(object sender, TextChangedEventArgs e) {

        }
    }
}
