using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WindowsFormsApp10
{
   
    public partial class usbConsole00 : Form
    {
        [DllImport("User32.dll")]
        public static extern bool PtInRect(ref Rectangle r, Point p);
        [DllImport("User32.dll", SetLastError = true)]
        public static extern UInt32 RegisterHotKey(IntPtr hWnd, UInt32 id, UInt32 fsModifiers, UInt32 vk);
        [DllImport("user32.dll", SetLastError = true)]
        public static extern bool UnregisterHotKey(IntPtr hWnd, int id);
        public usbConsole00()
        {
            InitializeComponent();
            RegisterHotKey(Handle, 247692, 0, (UInt32)Keys.F6);
            int x = (SystemInformation.WorkingArea.Width - this.Size.Width);
            this.Location = (Point)new Size(x, 50);
            if (!sp1.IsOpen) { sp1.Open(); }
        }
       
        public static Byte[] sdat = { 0x00,0x12,0x21,0x34,0x43,0x56,0x65,0x78,0x87 };

        private void fO_MouseUp(object sender, MouseEventArgs e)
        {
            if (sp1.IsOpen)
            {
                sp1.Write(sdat,0,1);
            }
            else
            {
                sp1.Open();
                sp1.Write(sdat, 0, 1);
            }
        }

        private void fO__MouseDown(object sender, MouseEventArgs e)
        {
            if (sp1.IsOpen)
            {
                sp1.Write(sdat, 6, 1);
            }
            else
            {
                sp1.Open();
                sp1.Write(sdat, 6, 1);
            }

        }
       
        private void fO_MouseDown(object sender, MouseEventArgs e)
        {
            if (sp1.IsOpen)
            {
                sp1.Write(sdat, 5, 1);
            }
            else
            {
                sp1.Open();
                sp1.Write(sdat, 5, 1);
            }
        }

        private void iR_MouseDown(object sender, MouseEventArgs e)
        {
            if (sp1.IsOpen)
            {
                sp1.Write(sdat, 3, 1);
            }
            else
            {
                sp1.Open();
                sp1.Write(sdat, 3, 1);
            }
        }

        private void iR__MouseDown(object sender, MouseEventArgs e)
        {
            if (sp1.IsOpen)
            {
                sp1.Write(sdat, 4, 1);
            }
            else
            {
                sp1.Open();
                sp1.Write(sdat, 4, 1);
            }
        }

        private void lE_MouseDown(object sender, MouseEventArgs e)
        {
            if (sp1.IsOpen)
            {
                sp1.Write(sdat, 7, 1);
            }
            else
            {
                sp1.Open();
                sp1.Write(sdat, 7, 1);
            }
        }
        private void rI_MouseDown(object sender, MouseEventArgs e)
        {
            if (sp1.IsOpen)
            {
                sp1.Write(sdat, 8, 1);
            }
            else
            {
                sp1.Open();
                sp1.Write(sdat, 8, 1);
            }
        }
        private void dO_MouseDown(object sender, MouseEventArgs e)
        {
            if (sp1.IsOpen)
            {
                sp1.Write(sdat, 1, 1);
            }
            else
            {
                sp1.Open();
                sp1.Write(sdat, 1, 1);
            }
        }
        private void uP_MouseDown(object sender, MouseEventArgs e)
        {
            if (sp1.IsOpen)
            {
                sp1.Write(sdat, 2, 1);
            }
            else
            {
                sp1.Open();
                sp1.Write(sdat, 2, 1);
            }
        }
        private void usbConsole00_Move(object sender, EventArgs e)
        {
            Rectangle r = Screen.GetWorkingArea(this);
            if (this.Right >= r.Right + 10)
                this.Location = (Point)new Size(r.Right - this.Size.Width, this.Top);
            else if (this.Left <= -10)
                this.Location = (Point)new Size(0, this.Top);
            else if ((this.Bottom >= r.Bottom))
                this.Location = (Point)new Size(this.Left, this.Size.Height - 20);
        }
        private Point offset;
        private void usbConsole00_MouseMove(object sender, MouseEventArgs e)
        {

            if (MouseButtons.Left != e.Button) return;
            Point cur = MousePosition;
            this.Location = new Point(cur.X - offset.X, cur.Y - offset.Y);

        }
        protected override bool ProcessDialogKey(Keys keyData)
        {
            if (keyData == Keys.Up || keyData == Keys.Down || keyData == Keys.Left || keyData == Keys.Right || keyData == Keys.I || keyData == Keys.O || keyData == Keys.F || keyData == Keys.G)
                return false;
            else
                return base.ProcessDialogKey(keyData);
        }
        private void usbConsole00_MouseDown(object sender, MouseEventArgs e)
        {
            if (MouseButtons.Left != e.Button) return;
            Point cur = this.PointToScreen(e.Location);
            offset = new Point(cur.X - this.Left, cur.Y - this.Top);
        }
       
        
    
    
        private void timer1_Tick(object sender, EventArgs e)
        {
            System.Drawing.Point pp = new Point(Cursor.Position.X, Cursor.Position.Y);//获取鼠标在屏幕的坐标点
            Rectangle Rects = new Rectangle(this.Left, this.Top, this.Left + this.Width, this.Top + this.Height);//存储当前窗体在屏幕的所在区域
            if ((this.Top < 0) && PtInRect(ref Rects, pp))//当鼠标在当前窗体内，并且窗体的Top属性小于0
                this.Top = 0;//设置窗体的Top属性为0
            else
                //当窗体的上边框与屏幕的顶端的距离小于5时
                if (this.Top > -543 && this.Top < 30 && !PtInRect(ref Rects, pp))
                this.Top = 15 - this.Height;//将窗体隐藏到屏幕的顶端
        }
    
        protected override void WndProc(ref Message m)
        {
            const int WM_HOTKEY = 0x0312;
            if (m.Msg == WM_HOTKEY && m.WParam.ToInt32() == 247692)
            {
                this.Activate();
            }
            base.WndProc(ref m);
        }

        private void usbConsole00_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Up || e.KeyCode == Keys.Down || e.KeyCode == Keys.Left || e.KeyCode == Keys.Right || e.KeyCode == Keys.I || e.KeyCode == Keys.O || e.KeyCode == Keys.F || e.KeyCode == Keys.G)
            {
                fO_MouseUp(null, null);
            }
        }

        private void usbConsole00_FormClosing(object sender, FormClosingEventArgs e)
        {
            UnregisterHotKey(Handle, 247692);
        }

        private void usbConsole00_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Up)
            {
                uP_MouseDown(null, null);
            }
            else if (e.KeyCode == Keys.Down)
            {
                dO_MouseDown(null, null);
            }
            else if (e.KeyCode == Keys.Left)
            {
                lE_MouseDown(null, null);
            }
            else if (e.KeyCode == Keys.Right)
            {
                rI_MouseDown(null, null);
            }
            else if (e.KeyCode == Keys.I)
            {
                iR_MouseDown(null, null);
            }
            else if (e.KeyCode == Keys.O)
            {
                iR__MouseDown(null, null);
            }
            else if (e.KeyCode == Keys.F)
            {
                fO_MouseDown(null, null);
            }
            else if (e.KeyCode == Keys.G)
            {
                fO__MouseDown(null, null);
            }
        }
       
    }
    }
