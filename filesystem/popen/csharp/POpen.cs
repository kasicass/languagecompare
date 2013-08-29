using System;
using System.Diagnostics;

namespace LanguageCompare.FileSystem
{
	public class POpen
	{
		public static void Main(string[] args)
		{
			if (args.Length == 0)
			{
				Console.WriteLine("usage:");
				Console.WriteLine("  POpen.exe <cmd> [param1 ...]");
				return;
			}
				
			try
			{
				Process p = new Process();
				p.StartInfo.UseShellExecute = false;
				p.StartInfo.RedirectStandardOutput = true;
				p.StartInfo.FileName  = args[0];
				if (args.Length > 1)
				{
					p.StartInfo.Arguments = string.Join(" ", args, 1, args.Length-1);
				}
				p.Start();
	
				string output = p.StandardOutput.ReadToEnd();
				p.WaitForExit();
	
				Console.WriteLine(output);
			}
			catch (Exception e)
			{
				Console.WriteLine("POpen Failed, msg:{0}, trackback:{1}", e.Message, e.StackTrace);
			}
		}
	}
}

