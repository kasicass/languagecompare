// build:
//  csc ReadVersion.cs

using System;
using System.IO;

class ReadVersion
{
	public static void Usage()
	{
		Console.WriteLine("usage: ReadVersion.exe <README>");
		Environment.Exit(1);
	}

	public static string ReadVersionFromFile(string fileName)
	{
		using (StreamReader sr = new StreamReader(fileName))
		{
			while (sr.Peek() >= 0)
			{
				string line = sr.ReadLine();
				if (line.StartsWith("Version="))
				{
					return line.Split('=')[1];
				}
			}
		}
		return "none";
	}

	public static void Main(string[] args)
	{
		if (args.Length != 1)
			Usage();

		string version = ReadVersionFromFile(args[0]);
		Console.WriteLine("{0}", version);
	}
}
