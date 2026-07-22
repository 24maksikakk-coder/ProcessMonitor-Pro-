// processmonitor_rs.rs — монитор процессов на Rust (sysinfo)

use sysinfo::{System, SystemExt, ProcessExt, PidExt};
use std::thread;
use std::time::Duration;
use std::env;
use std::process;
use termion::{color, style};

struct ProcessInfo {
    pid: i32,
    name: String,
    memory_mb: f64,
    cpu: f32,
    threads: i32,
    status: String,
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let mut sort_by = "memory".to_string();
    let mut filter = String::new();
    let mut interval = 2;

    let mut i = 1;
    while i < args.len() {
        if args[i] == "--sort" && i+1 < args.len() {
            sort_by = args[i+1].clone();
            i += 2;
        } else if args[i] == "--filter" && i+1 < args.len() {
            filter = args[i+1].clone();
            i += 2;
        } else if args[i] == "--interval" && i+1 < args.len() {
            interval = args[i+1].parse().unwrap_or(2);
            i += 2;
        } else {
            i += 1;
        }
    }

    println!("📊 ProcessMonitor Pro — Rust Edition");
    println!("Нажмите Ctrl+C для выхода\n");

    let mut sys = System::new_all();

    loop {
        sys.refresh_all();
        let mut processes: Vec<ProcessInfo> = Vec::new();

        for (pid, proc) in sys.processes() {
            let name = proc.name().to_string_lossy().to_string();
            if !filter.is_empty() && !name.to_lowercase().contains(&filter.to_lowercase()) {
                continue;
            }
            let memory_mb = proc.memory() as f64 / (1024.0 * 1024.0);
            let cpu = proc.cpu_usage();
            let threads = proc.threads().len() as i32;
            let status = format!("{:?}", proc.status());
            processes.push(ProcessInfo {
                pid: pid.as_u32() as i32,
                name,
                memory_mb,
                cpu,
                threads,
                status,
            });
        }

        // Сортировка
        match sort_by.as_str() {
            "cpu" => processes.sort_by(|a, b| b.cpu.partial_cmp(&a.cpu).unwrap_or(std::cmp::Ordering::Equal)),
            "pid" => processes.sort_by(|a, b| a.pid.cmp(&b.pid)),
            "name" => processes.sort_by(|a, b| a.name.cmp(&b.name)),
            _ => processes.sort_by(|a, b| b.memory_mb.partial_cmp(&a.memory_mb).unwrap_or(std::cmp::Ordering::Equal)),
        }

        print!("\x1b[2J\x1b[H");
        println!("{}📊 ProcessMonitor Pro — Rust Edition{}", color::Fg(color::Cyan), style::Reset);
        println!("Процессов: {} | Фильтр: {}", processes.len(), if filter.is_empty() { "все" } else { &filter });
        println!("{}", "-".repeat(90));
        println!("{:>6} {:<20} {:>12} {:>8} {:>8} {:<12}", "PID", "ИМЯ", "ПАМЯТЬ", "CPU", "ПОТОКИ", "СТАТУС");
        println!("{}", "-".repeat(90));

        let mut count = 0;
        for p in processes.iter().take(30) {
            if count > 30 { break; }
            count += 1;
            let color = if p.memory_mb > 500.0 {
                color::Fg(color::Red)
            } else if p.memory_mb > 200.0 {
                color::Fg(color::Yellow)
            } else {
                color::Fg(color::Green)
            };
            let name = if p.name.len() > 20 { &p.name[..20] } else { &p.name };
            println!("{:>6} {:<20} {}{:>10.1} MB{}{:>7.1}% {:>8} {:<12}",
                p.pid, name, color, p.memory_mb, style::Reset, p.cpu, p.threads, p.status);
        }
        println!("{}", "-".repeat(90));
        let total_memory: f64 = processes.iter().map(|p| p.memory_mb).sum();
        println!("Всего памяти: {:.1} MB", total_memory);

        thread::sleep(Duration::from_secs(interval as u64));
    }
}
