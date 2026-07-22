// processmonitor_cs.cs — монитор процессов на C# (.NET Core)

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading;
using System.Text;

class ProcessMonitor
{
    private class ProcessInfo
    {
        public int Pid { get; set; }
        public string Name { get; set; }
        public double MemoryMB { get; set; }
        public double Cpu { get; set; }
        public int Threads { get; set; }
        public string Status { get; set; }
    }

    public static List<ProcessInfo> GetProcesses()
    {
        var processes = new List<ProcessInfo>();
        foreach (var proc in Process.GetProcesses())
        {
            try
            {
                var info = new ProcessInfo
                {
                    Pid = proc.Id,
                    Name = proc.ProcessName,
                    MemoryMB = proc.WorkingSet64 / (1024.0 * 1024.0),
                    Cpu = proc.TotalProcessorTime.TotalSeconds, // упрощённо
                    Threads = proc.Threads.Count,
                    Status = proc.Responding ? "running" : "not responding"
                };
                processes.Add(info);
            }
            catch { /* ignore */ }
        }
        return processes;
    }

    public static void Display(List<ProcessInfo> processes, string sortBy, string filter)
    {
        // Сортировка
        if (sortBy == "memory")
            processes = processes.OrderByDescending(p => p.MemoryMB).ToList();
        else if (sortBy == "cpu")
            processes = processes.OrderByDescending(p => p.Cpu).ToList();
        else if (sortBy == "pid")
            processes = processes.OrderBy(p => p.Pid).ToList();
        else
            processes = processes.OrderBy(p => p.Name).ToList();

        Console.Clear();
        Console.WriteLine("📊 ProcessMonitor Pro — C# Edition");
        Console.WriteLine($"Процессов: {processes.Count} | Фильтр: {(string.IsNullOrEmpty(filter) ? "все" : filter)}");
        Console.WriteLine(new string('-', 90));
        Console.WriteLine($"{"PID",6} {"ИМЯ",-20} {"ПАМЯТЬ",12} {"CPU",8} {"ПОТОКИ",8} {"СТАТУС",-12}");
        Console.WriteLine(new string('-', 90));

        int count = 0;
        foreach (var p in processes.Take(30))
        {
            if (!string.IsNullOrEmpty(filter) && !p.Name.Contains(filter, StringComparison.OrdinalIgnoreCase))
                continue;
            string color = "\x1b[92m";
            if (p.MemoryMB > 500) color = "\x1b[91m";
            else if (p.MemoryMB > 200) color = "\x1b[93m";
            Console.WriteLine($"{p.Pid,6} {p.Name.Substring(0, Math.Min(20, p.Name.Length)),-20} {color}{p.MemoryMB,10:F1} MB\x1b[0m {p.Cpu,7:F1}% {p.Threads,8} {p.Status,-12}");
        }
        Console.WriteLine(new string('-', 90));
        double totalMemory = processes.Sum(p => p.MemoryMB);
        Console.WriteLine($"Всего памяти: {totalMemory:F1} MB");
    }

    public static void Main(string[] args)
    {
        string sortBy = "memory";
        string filter = "";
        int interval = 2;

        for (int i = 0; i < args.Length; i++)
        {
            if (args[i] == "--sort" && i+1 < args.Length)
                sortBy = args[++i];
            else if (args[i] == "--filter" && i+1 < args.Length)
                filter = args[++i];
            else if (args[i] == "--interval" && i+1 < args.Length)
                interval = int.Parse(args[++i]);
        }

        Console.WriteLine("📊 ProcessMonitor Pro — C# Edition");
        Console.WriteLine("Нажмите Ctrl+C для выхода\n");

        while (true)
        {
            var processes = GetProcesses();
            Display(processes, sortBy, filter);
            Thread.Sleep(interval * 1000);
        }
    }
}
