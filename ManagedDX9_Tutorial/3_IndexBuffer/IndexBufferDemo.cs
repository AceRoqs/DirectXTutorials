/*
Copyright (C) 2003 by Toby Jones.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

// Managed DirectX 9 index buffer triangle

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
            this.Text = "Index Buffer Demo";

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
        private Direct3D.VertexBuffer vertexBuffer = null;
        private Direct3D.IndexBuffer indexBuffer = null;

        private static Direct3D.CustomVertex.TransformedColored[] m_aVertices =
        {
            new Direct3D.CustomVertex.TransformedColored(120.0f, 120.0f, 1.0f, 1.0f, 0),
            new Direct3D.CustomVertex.TransformedColored(200.0f, 200.0f, 1.0f, 1.0f, 0),
            new Direct3D.CustomVertex.TransformedColored(30.0f,  200.0f, 1.0f, 1.0f, int.MaxValue),
        };

        private static ushort[] m_aIndices = { 0, 1, 2 };

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

            device.DeviceReset += new System.EventHandler(this.OnDeviceReset);

            OnDeviceReset(device, null);
        }

        public void OnDeviceReset(object sender, System.EventArgs e)
        {
            Direct3D.Device dev = (Direct3D.Device)sender;

            dev.RenderState.Lighting = false;

            if(null != vertexBuffer)
            {
                vertexBuffer.Dispose();
            }

            vertexBuffer = new Direct3D.VertexBuffer(m_aVertices[0].GetType(),
                                                     m_aVertices.Length,
                                                     dev,
                                                     Direct3D.Usage.WriteOnly,
                                                     Direct3D.CustomVertex.TransformedColored.Format,
                                                     Direct3D.Pool.Default);

            vertexBuffer.SetData(m_aVertices, 0, Direct3D.LockFlags.None);

            dev.VertexShader = null;
            dev.VertexFormat = Direct3D.CustomVertex.TransformedColored.Format;
            dev.SetStreamSource(0, vertexBuffer, 0);

            if(null != indexBuffer)
            {
                indexBuffer.Dispose();
            }

            indexBuffer = new Direct3D.IndexBuffer(m_aIndices[0].GetType(),
                                                   m_aIndices.Length,
                                                   dev,
                                                   Direct3D.Usage.WriteOnly,
                                                   Direct3D.Pool.Default);
            indexBuffer.SetData(m_aIndices, 0, Direct3D.LockFlags.None);

            dev.Indices = indexBuffer;
        }

        private void DrawScene()
        {
            device.Clear(Direct3D.ClearFlags.Target, System.Drawing.Color.CornflowerBlue, 1.0f, 0);
            device.BeginScene();
            device.DrawIndexedPrimitives(Direct3D.PrimitiveType.TriangleList, 0, 0, m_aVertices.Length, 0, m_aIndices.Length / 3);
            device.EndScene();
            device.Present();
        }
    }
}

