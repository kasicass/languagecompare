// build:
//  csc MD5Tool.cs

using System;
using System.IO;
using System.Text;
using System.Security.Cryptography;

namespace LanguageCompare.Encryption.MD5Tool
{
	public class MD5Tool
	{
		public static string BytesToHexString(byte[] data)
		{
			StringBuilder builder = new StringBuilder();
			for (int i = 0; i < data.Length; i++)
			{
				builder.Append(data[i].ToString("x2"));
			}
			return builder.ToString();
		}

		public static void Main(string[] args)
		{
			if (args.Length == 1)
			{
				MD5 m = MD5.Create();
				byte[] hashValue = m.ComputeHash(new FileStream(args[0], FileMode.Open));
				Console.WriteLine("{0}", BytesToHexString(hashValue));
			}
			else if (args.Length == 2 && args[0] == "-s")
			{
				MD5 m = MD5.Create();
				byte[] hashValue = m.ComputeHash(Encoding.UTF8.GetBytes(args[1]));
				Console.WriteLine("{0}", BytesToHexString(hashValue));
			}
			else
			{
				Console.WriteLine("usage: ");
				Console.WriteLine("  MDTool.exe -s <string>");
				Console.WriteLine("  MDTool.exe <filename>");
			}
		}
	}
}

