using System;
using System.IO;
using System.Text;
using System.Windows;
using System.ComponentModel;
using System.Security.Cryptography;

namespace WinMD5
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        BackgroundWorker _worker;

        public static string BytesToHexString(byte[] data)
        {
            StringBuilder builder = new StringBuilder();
            for (int i = 0; i < data.Length; i++)
            {
                builder.Append(data[i].ToString("x2"));
            }
            return builder.ToString();
        }

        public void ComputeMD5_DoWork(object sender, DoWorkEventArgs e)
        {
            BackgroundWorker bw = sender as BackgroundWorker;
            string file = (string)e.Argument;

            MD5 m = MD5.Create();
            using (var fs = new FileStream(file, FileMode.Open))
            {
                byte[] hashValue = m.ComputeHash(fs);
                e.Result = BytesToHexString(hashValue) + "    " + Path.GetFileName(file);
            }
        }

        public void ComputeMD5_ReportProgress(object sender, ProgressChangedEventArgs e)
        {
        }

        public void ComputeMD5_Completed(object sender, RunWorkerCompletedEventArgs e)
        {
            if (e.Cancelled)
            {
                MessageBox.Show("Cancelled");
            }
            else if (e.Error != null)
            {
                MessageBox.Show("Err: " + e.Error.Message);
            }
            else
            {
                string md5hex = (string)e.Result;
                this.txtResult.Text = md5hex;
            }
        }

        public MainWindow()
        {
            InitializeComponent();

            _worker = new BackgroundWorker();
            _worker.WorkerReportsProgress = true;
            _worker.DoWork += this.ComputeMD5_DoWork;
            _worker.ProgressChanged += this.ComputeMD5_ReportProgress;
            _worker.RunWorkerCompleted += this.ComputeMD5_Completed;
        }

        private void Window_Drop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                if (_worker.IsBusy)
                {
                    return;
                }

                string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
                this.txtCurrent.Text = files[0];

                _worker.RunWorkerAsync(files[0]);
            }
        }

        private void btnQuit_Click(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown();
        }
    }
}
