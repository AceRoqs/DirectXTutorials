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

// Managed DirectX 9 spinning textured cube

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
            this.Text = "Cube Demo";

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
        private Direct3D.Texture texture = null;

        private static Direct3D.CustomVertex.PositionColoredTextured[] m_aVertices =
        {
            // front face
            new Direct3D.CustomVertex.PositionColoredTextured(  0.5f, -0.5f, -0.5f, 1000000,      1.0f, 1.0f ), // 0
            new Direct3D.CustomVertex.PositionColoredTextured( -0.5f, -0.5f, -0.5f, 1000000,      0.0f, 1.0f ), // 1
            new Direct3D.CustomVertex.PositionColoredTextured(  0.5f,  0.5f, -0.5f, int.MaxValue, 1.0f, 0.0f ), // 2
            new Direct3D.CustomVertex.PositionColoredTextured( -0.5f,  0.5f, -0.5f, int.MaxValue, 0.0f, 0.0f ), // 3

            // top face
            new Direct3D.CustomVertex.PositionColoredTextured(  0.5f,  0.5f, -0.5f, int.MaxValue, 1.0f, 1.0f ), // 4
            new Direct3D.CustomVertex.PositionColoredTextured( -0.5f,  0.5f, -0.5f, int.MaxValue, 0.0f, 1.0f ), // 5
            new Direct3D.CustomVertex.PositionColoredTextured(  0.5f,  0.5f,  0.5f, int.MaxValue, 1.0f, 0.0f ), // 6
            new Direct3D.CustomVertex.PositionColoredTextured( -0.5f,  0.5f,  0.5f, int.MaxValue, 0.0f, 0.0f ), // 7

            // back face
            new Direct3D.CustomVertex.PositionColoredTextured( -0.5f, -0.5f,  0.5f, 1000000,      1.0f, 1.0f ), // 8
            new Direct3D.CustomVertex.PositionColoredTextured(  0.5f, -0.5f,  0.5f, 1000000,      0.0f, 1.0f ), // 9
            new Direct3D.CustomVertex.PositionColoredTextured(  0.5f,  0.5f,  0.5f, int.MaxValue, 0.0f, 0.0f ), // 10
            new Direct3D.CustomVertex.PositionColoredTextured( -0.5f,  0.5f,  0.5f, int.MaxValue, 1.0f, 0.0f ), // 11

            // left face
            new Direct3D.CustomVertex.PositionColoredTextured(  0.5f, -0.5f,  0.5f, 1000000,      1.0f, 1.0f ), // 12
            new Direct3D.CustomVertex.PositionColoredTextured(  0.5f,  0.5f, -0.5f, int.MaxValue, 0.0f, 0.0f ), // 13
            new Direct3D.CustomVertex.PositionColoredTextured(  0.5f,  0.5f,  0.5f, int.MaxValue, 1.0f, 0.0f ), // 14
            new Direct3D.CustomVertex.PositionColoredTextured(  0.5f, -0.5f, -0.5f, 1000000,      0.0f, 1.0f ), // 15

            // bottom face
            new Direct3D.CustomVertex.PositionColoredTextured(  0.5f, -0.5f,  0.5f, 1000000,      1.0f, 1.0f ), // 16
            new Direct3D.CustomVertex.PositionColoredTextured( -0.5f, -0.5f, -0.5f, 1000000,      0.0f, 0.0f ), // 17
            new Direct3D.CustomVertex.PositionColoredTextured(  0.5f, -0.5f, -0.5f, 1000000,      1.0f, 0.0f ), // 18
            new Direct3D.CustomVertex.PositionColoredTextured( -0.5f, -0.5f,  0.5f, 1000000,      0.0f, 1.0f ), // 19

            // right face
            new Direct3D.CustomVertex.PositionColoredTextured( -0.5f,  0.5f,  0.5f, int.MaxValue, 0.0f, 0.0f ), // 20
            new Direct3D.CustomVertex.PositionColoredTextured( -0.5f,  0.5f, -0.5f, int.MaxValue, 1.0f, 0.0f ), // 21
            new Direct3D.CustomVertex.PositionColoredTextured( -0.5f, -0.5f,  0.5f, 1000000,      0.0f, 1.0f ), // 22
            new Direct3D.CustomVertex.PositionColoredTextured( -0.5f, -0.5f, -0.5f, 1000000,      1.0f, 1.0f ), // 23
        };

        private static ushort[] m_aIndices =
        {
            0,  1,   2,  3,  2,  1,
            4,  5,   6,  5,  7,  6,
            8,  9,  10,  8, 10, 11,
            12, 13, 14, 12, 15, 13,
            16, 17, 18, 16, 19, 17,
            20, 21, 22, 21, 23, 22
        };

        private void InitDirect3D()
        {
            Direct3D.PresentParameters presentParams;
            presentParams = new Direct3D.PresentParameters();

            presentParams.Windowed               = windowed;
            presentParams.EnableAutoDepthStencil = true;
            presentParams.AutoDepthStencilFormat = Direct3D.DepthFormat.D16;

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

            // ZBufferring is on by default
            //dev.RenderState.ZBufferEnable = true;

            if(null != vertexBuffer)
            {
                vertexBuffer.Dispose();
            }

            vertexBuffer = new Direct3D.VertexBuffer(m_aVertices[0].GetType(),
                                                     m_aVertices.Length,
                                                     dev,
                                                     Direct3D.Usage.WriteOnly,
                                                     Direct3D.CustomVertex.PositionColoredTextured.Format,
                                                     Direct3D.Pool.Default);

            vertexBuffer.SetData(m_aVertices, 0, Direct3D.LockFlags.None);

            dev.VertexShader = null;
            dev.VertexFormat = Direct3D.CustomVertex.PositionColoredTextured.Format;
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

            System.Reflection.Assembly assembly = System.Reflection.Assembly.GetExecutingAssembly();
            System.IO.Stream stream = assembly.GetManifestResourceStream("_5_Cube.photo.png");

            // If you want to know the names of all of the embedded resources,
            // you can get an array of strings with the names.
            // string[] resourceNames = assembly.GetManifestResourceNames();

            // If you want to load the texture from a file instead of an embedded resource, you can do this:
            // texture = Direct3D.TextureLoader.FromFile(dev, "photo.png");

            if(null != texture)
            {
                texture.Dispose();
            }

            texture = Direct3D.TextureLoader.FromStream(dev, stream);
            stream.Close();

            dev.SetTexture(0, texture);
        }

        private void DrawScene()
        {
            device.Clear(Direct3D.ClearFlags.Target | Direct3D.ClearFlags.ZBuffer, System.Drawing.Color.CornflowerBlue, 1.0f, 0);
            BuildMatrices();
            device.BeginScene();
            device.DrawIndexedPrimitives(Direct3D.PrimitiveType.TriangleList,
                                         0,
                                         0,
                                         m_aVertices.Length,
                                         0,
                                         m_aIndices.Length / 3);
            device.EndScene();
            device.Present();
        }

        private void BuildMatrices()
        {
            device.Transform.World = Microsoft.DirectX.Matrix.RotationY(System.Environment.TickCount / 500.0f);

            // NOTE: only the World transform matrix needs to be set every frame, but the others
            // are set here anyway to simplify later demos that require the matrices every frame
            // (see 6_ShaderAsm and 7_ShaderHLSL).
            Microsoft.DirectX.Vector3 cameraPosition = new Vector3(0.0f, 1.2f, -2.0f);
            Microsoft.DirectX.Vector3 cameraTarget   = new Vector3(0.0f, 0.0f, 0.0f);
            Microsoft.DirectX.Vector3 cameraUpVector = new Vector3(0.0f, 1.0f, 0.0f);
            device.Transform.View = Matrix.LookAtLH(cameraPosition, cameraTarget, cameraUpVector);
            device.Transform.Projection = Matrix.PerspectiveFovLH((float)System.Math.PI / 4, 4.0f / 3.0f, 1.0f, 100.0f);
        }
    }
}

