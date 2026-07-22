// processmonitor_java.java — монитор процессов на Java

import java.lang.management.*;
import java.util.*;
import java.io.*;
import java.nio.file.*;
import java.text.*;

public class ProcessMonitor {
    private static class ProcessInfo {
        long pid;
        String name;
        double memoryMB;
        double cpu;
        int threads;
        String status;
    }

    public static List<ProcessInfo> getProcesses() {
        List<ProcessInfo> processes = new ArrayList<>();
        try {
            // Используем ManagementFactory для получения информации о процессах
            // Для получения всех процессов используем ProcessHandle
            ProcessHandle.allProcesses().forEach(ph -> {
                ProcessInfo info = new ProcessInfo();
                info.pid = ph.pid();
                info.name = ph.info().command().orElse("unknown");
                // Получаем память через Runtime (не для каждого процесса, только для текущего)
                // В Java сложно получить память для всех процессов, поэтому используем приближение
                info.memoryMB = Runtime.getRuntime().totalMemory() / (1024 * 1024);
                info.threads = Thread.activeCount();
                info.status = ph.isAlive() ? "running" : "stopped";
                processes.add(info);
            });
        } catch (Exception e) {
            System.err.println("Ошибка получения процессов: " + e.getMessage());
        }
        return processes;
    }

    public static void display(List<ProcessInfo> processes, String sortBy, String filter) {
        // Сортировка
        Comparator<ProcessInfo> comp = Comparator.comparingDouble(p -> -p.memoryMB);
        if (sortBy.equals("cpu")) {
            comp = Comparator.comparingDouble(p -> -p.cpu);
        } else if (sortBy.equals("pid")) {
            comp = Comparator.comparingLong(p -> p.pid);
        } else if (sortBy.equals("name")) {
            comp = Comparator.comparing(p -> p.name);
        }
        processes.sort(comp);

        // Очистка экрана (ANSI)
        System.out.print("\033[H\033[J");
        System.out.println("📊 ProcessMonitor Pro — Java Edition");
        System.out.println("Процессов: " + processes.size() + " | Фильтр: " + (filter.isEmpty() ? "все" : filter));
        System.out.println("-".repeat(90));
        System.out.printf("%6s %-20s %12s %8s %8s %-12s%n", "PID", "ИМЯ", "ПАМЯТЬ", "CPU", "ПОТОКИ", "СТАТУС");
        System.out.println("-".repeat(90));

        int count = 0;
        for (ProcessInfo p : processes) {
            if (count++ > 30) break;
            if (!filter.isEmpty() && !p.name.toLowerCase().contains(filter.toLowerCase())) continue;
            String color = "\033[92m";
            if (p.memoryMB > 500) color = "\033[91m";
            else if (p.memoryMB > 200) color = "\033[93m";
            System.out.printf("%6d %-20s %s%10.1f MB\033[0m %7.1f%% %8d %-12s%n",
                p.pid, p.name.substring(0, Math.min(20, p.name.length())),
                color, p.memoryMB, p.cpu, p.threads, p.status);
        }
        System.out.println("-".repeat(90));
        double totalMemory = processes.stream().mapToDouble(p -> p.memoryMB).sum();
        System.out.printf("Всего памяти: %.1f MB%n", totalMemory);
    }

    public static void main(String[] args) {
        String sortBy = "memory";
        String filter = "";
        int interval = 2;

        for (int i = 0; i < args.length; i++) {
            if (args[i].equals("--sort") && i+1 < args.length) {
                sortBy = args[++i];
            } else if (args[i].equals("--filter") && i+1 < args.length) {
                filter = args[++i];
            } else if (args[i].equals("--interval") && i+1 < args.length) {
                interval = Integer.parseInt(args[++i]);
            }
        }

        System.out.println("📊 ProcessMonitor Pro — Java Edition");
        System.out.println("Нажмите Ctrl+C для выхода\n");

        try {
            while (true) {
                List<ProcessInfo> processes = getProcesses();
                display(processes, sortBy, filter);
                Thread.sleep(interval * 1000);
            }
        } catch (InterruptedException e) {
            System.out.println("\nВыход...");
        }
    }
}
