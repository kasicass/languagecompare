using System;
using System.Net;
using System.ComponentModel;
using System.Threading;

namespace LanguageCompare.Network
{
	public class WGet
	{
		private static void DownloadSimple(string url, string filename)
		{
			WebClient client = new WebClient();
			client.DownloadFile(new Uri(url), filename);
		}

		private static void DownloadVerbose(string url, string filename)
		{
			WebClient client = new WebClient();
			client.DownloadFileCompleted += delegate (Object sender, AsyncCompletedEventArgs e) {
				Console.WriteLine("Completed!");
			};
			client.DownloadProgressChanged += delegate (Object sender, DownloadProgressChangedEventArgs e) {
				Console.WriteLine("Downloading: {0} of {1} bytes ({2}%)",
					e.BytesReceived, e.TotalBytesToReceive, e.ProgressPercentage);
			};
			client.DownloadFileAsync(new Uri(url), filename);

			// Downloading speed is extremely slow. Am I done sth. wrong?
			while (client.IsBusy)
			{
				Thread.Sleep(50);
			}
		}

		public static void Main(string[] args)
		{
			if (args.Length == 0)
			{
				Console.WriteLine("wget.exe [-v|--verbose] <url>");
				return;
			}

			string url = args[args.Length-1];
			string[] result = url.Split('/');
			if (result.Length == 1)
			{
				Console.WriteLine("wget.exe [-v|--verbose] <url>");
				return;
			}

			string filename = result[result.Length-1];
			Console.WriteLine("Downloading {0}", filename);
			if (args.Length > 1 && (args[0] == "-v" || args[0] == "--verbose"))
			{
				DownloadVerbose(url, filename);
			}
			else
			{
				DownloadSimple(url, filename);
			}
		}
	}
}

