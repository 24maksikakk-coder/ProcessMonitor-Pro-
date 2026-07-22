# processmonitor_python.py — монитор процессов (использование памяти) на Python

import psutil
import sys
import time
import argparse
from datetime import datetime
import json

class ProcessMonitor:
    def __init__(self, sort_by='memory', filter_name=None, interval=2):
        self.sort_by = sort_by
        self.filter_name = filter_name.lower() if filter_name else None
        self.interval = interval
        self.running = True

    def get_processes(self):
        processes = []
        for proc in psutil.process_iter(['pid', 'name', 'memory_info', 'cpu_percent', 'num_threads', 'status']):
            try:
                info = proc.info
                if self.filter_name and self.filter_name not in info['name'].lower():
                    continue
                processes.append({
                    'pid': info['pid'],
                    'name': info['name'],
                    'memory_mb': info['memory_info'].rss / (1024 * 1024) if info['memory_info'] else 0,
                    'cpu': info['cpu_percent'] or 0,
                    'threads': info['num_threads'] or 0,
                    'status': info['status'] or 'unknown'
                })
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                continue
        return processes

    def sort_processes(self, processes):
        key_map = {
            'memory': lambda p: p['memory_mb'],
            'cpu': lambda p: p['cpu'],
            'pid': lambda p: p['pid'],
            'name': lambda p: p['name']
        }
        reverse = self.sort_by != 'name'
        return sorted(processes, key=key_map.get(self.sort_by, key_map['memory']), reverse=reverse)

    def display(self, processes):
        # Очистка экрана (ANSI)
        print("\033[H\033[J")
        print(f"📊 ProcessMonitor Pro — Python Edition")
        print(f"Время: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print(f"Процессов: {len(processes)} | Фильтр: {self.filter_name or 'все'} | Сортировка: {self.sort_by}")
        print("-" * 90)
        print(f"{'PID':>6} {'ИМЯ':<20} {'ПАМЯТЬ':>12} {'CPU':>8} {'ПОТОКИ':>8} {'СТАТУС':<12}")
        print("-" * 90)

        for p in processes[:30]:  # показываем топ-30
            memory = p['memory_mb']
            # Цветовая индикация
            color = ''
            if memory > 500:
                color = '\033[91m'  # красный
            elif memory > 200:
                color = '\033[93m'  # жёлтый
            else:
                color = '\033[92m'  # зелёный
            reset = '\033[0m'
            print(f"{p['pid']:>6} {p['name'][:20]:<20} {color}{memory:>10.1f} MB{reset} "
                  f"{p['cpu']:>7.1f}% {p['threads']:>8} {p['status']:<12}")
        print("-" * 90)
        # Статистика
        total_memory = sum(p['memory_mb'] for p in processes)
        total_cpu = sum(p['cpu'] for p in processes)
        print(f"Всего памяти: {total_memory:.1f} MB | Всего CPU: {total_cpu:.1f}%")

    def run(self):
        print("📊 ProcessMonitor Pro — Python Edition")
        print("Нажмите Ctrl+C для выхода\n")
        try:
            while self.running:
                processes = self.get_processes()
                sorted_procs = self.sort_processes(processes)
                self.display(sorted_procs)
                time.sleep(self.interval)
        except KeyboardInterrupt:
            self.running = False
            print("\nВыход...")

    def export_csv(self, filename):
        processes = self.sort_processes(self.get_processes())
        import csv
        with open(filename, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['PID', 'Name', 'Memory (MB)', 'CPU (%)', 'Threads', 'Status'])
            for p in processes:
                writer.writerow([p['pid'], p['name'], f"{p['memory_mb']:.2f}",
                                 f"{p['cpu']:.2f}", p['threads'], p['status']])
        print(f"Экспортировано в {filename}")

    def export_json(self, filename):
        processes = self.sort_processes(self.get_processes())
        with open(filename, 'w') as f:
            json.dump(processes, f, indent=2)
        print(f"Экспортировано в {filename}")

def main():
    parser = argparse.ArgumentParser(description="Монитор процессов")
    parser.add_argument('--sort', default='memory', choices=['memory', 'cpu', 'pid', 'name'],
                        help='Сортировка (memory, cpu, pid, name)')
    parser.add_argument('--filter', help='Фильтр по имени процесса')
    parser.add_argument('--interval', type=int, default=2, help='Интервал обновления (сек)')
    parser.add_argument('--export-csv', help='Экспорт в CSV')
    parser.add_argument('--export-json', help='Экспорт в JSON')
    args = parser.parse_args()

    monitor = ProcessMonitor(args.sort, args.filter, args.interval)
    if args.export_csv:
        monitor.export_csv(args.export_csv)
    elif args.export_json:
        monitor.export_json(args.export_json)
    else:
        monitor.run()

if __name__ == "__main__":
    main()
