//=========================================================================
// BasicDemo.cs
// Coder: Thobias Jones
// Purpose: Managed DirectX 9 basic skeleton
// Copyright is given up. This code is released into the
// public domain without warranty.
// Visit our website at http://www.genkisoft.com
//=========================================================================

using Microsoft.DirectX;
using Direct3D = Microsoft.DirectX.Direct3D;

namespace Genkisoft
{
    public class DX9Demo : System.Windows.Forms.Form
    {
        // Required designer variable.
        private System.ComponentModel.Container components = null;

        public DX9Demo()
        {
            // Required for Windows Form Designer support
            InitializeComponent();
        }

        // Clean up any resources being used.
        protected override void Dispose( bool disposing )
        {
            if( disposing )
            {
                if (components != null) 
                {
                    components.Dispose();
                }
            }
            base.Dispose( disposing );
        }

        #region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            // 
            // DX9Demo
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(292, 273);
            this.Name = "DX9Demo";
            this.Text = "Basic Demo";

        }
        #endregion

        [System.STAThread]
        static void Main()
        {
            DX9Demo demo = new DX9Demo();
            demo.InitDirect3D();
            demo.Run();
        }

        public void Run()
        {
            Show();

            while(Created)
            {
                DrawScene();
                System.Windows.Forms.Application.DoEvents();
            }
        }

        //
        // Start of Managed DirectX 9 demo code
        //

        private readonly bool windowed = true;
        private Direct3D.Device device = null;

        private void InitDirect3D()
        {
            Direct3D.PresentParameters presentParams;
            presentParams = new Direct3D.PresentParameters();

            presentParams.Windowed = windowed;
            if(windowed)
            {
                presentParams.SwapEffect = Direct3D.SwapEffect.Copy;
            }
            else
            {
                presentParams.SwapEffect       = Direct3D.SwapEffect.Flip;
                presentParams.BackBufferFormat = Direct3D.Manager.Adapters.Default.CurrentDisplayMode.Format;
                presentParams.BackBufferWidth  = Direct3D.Manager.Adapters.Default.CurrentDisplayMode.Width;
                presentParams.BackBufferHeight = Direct3D.Manager.Adapters.Default.CurrentDisplayMode.Height;
                presentParams.FullScreenRefreshRateInHz = Direct3D.PresentParameters.DefaultPresentRate;
                presentParams.PresentationInterval      = Direct3D.PresentInterval.One;
            }

            device = new Direct3D.Device(0,
                                         Direct3D.DeviceType.Hardware,
                                         this,
                                         Direct3D.CreateFlags.SoftwareVertexProcessing,
                                         presentParams);
        }

        private void DrawScene()
        {
            device.Clear(Direct3D.ClearFlags.Target, System.Drawing.Color.CornflowerBlue, 1.0f, 0);
            device.BeginScene();
            device.EndScene();
            device.Present();
        }
    }
}
