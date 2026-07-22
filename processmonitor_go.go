// processmonitor_go.go — монитор процессов на Go (gopsutil)

package main

import (
	"flag"
	"fmt"
	"os"
	"os/signal"
	"sort"
	"strings"
	"syscall"
	"time"

	"github.com/shirou/gopsutil/v3/process"
)

type ProcessInfo struct {
	Pid     int32
	Name    string
	Memory  float64 // MB
	CPU     float64 // %
	Threads int32
	Status  string
}

func getProcesses(filter string) []ProcessInfo {
	procs, err := process.Processes()
	if err != nil {
		fmt.Println("Ошибка получения процессов:", err)
		return nil
	}
	var result []ProcessInfo
	for _, p := range procs {
		name, _ := p.Name()
		if filter != "" && !strings.Contains(strings.ToLower(name), strings.ToLower(filter)) {
			continue
		}
		mem, _ := p.MemoryInfo()
		memMB := float64(0)
		if mem != nil {
			memMB = float64(mem.RSS) / (1024 * 1024)
		}
		cpu, _ := p.CPUPercent()
		threads, _ := p.NumThreads()
		status, _ := p.Status()
		statusStr := "running"
		if len(status) > 0 {
			statusStr = status[0]
		}
		result = append(result, ProcessInfo{
			Pid:     p.Pid,
			Name:    name,
			Memory:  memMB,
			CPU:     cpu,
			Threads: threads,
			Status:  statusStr,
		})
	}
	return result
}

func display(processes []ProcessInfo, sortBy string, filter string) {
	if sortBy == "memory" {
		sort.Slice(processes, func(i, j int) bool {
			return processes[i].Memory > processes[j].Memory
		})
	} else if sortBy == "cpu" {
		sort.Slice(processes, func(i, j int) bool {
			return processes[i].CPU > processes[j].CPU
		})
	} else if sortBy == "pid" {
		sort.Slice(processes, func(i, j int) bool {
			return processes[i].Pid < processes[j].Pid
		})
	} else {
		sort.Slice(processes, func(i, j int) bool {
			return processes[i].Name < processes[j].Name
		})
	}

	fmt.Print("\033[H\033[2J")
	fmt.Println("📊 ProcessMonitor Pro — Go Edition")
	fmt.Printf("Процессов: %d | Фильтр: %s\n", len(processes), map[bool]string{true: "все", false: filter}[filter == ""])
	fmt.Println(strings.Repeat("-", 90))
	fmt.Printf("%6s %-20s %12s %8s %8s %-12s\n", "PID", "ИМЯ", "ПАМЯТЬ", "CPU", "ПОТОКИ", "СТАТУС")
	fmt.Println(strings.Repeat("-", 90))

	count := 0
	for _, p := range processes {
		if count > 30 {
			break
		}
		count++
		color := "\033[92m"
		if p.Memory > 500 {
			color = "\033[91m"
		} else if p.Memory > 200 {
			color = "\033[93m"
		}
		name := p.Name
		if len(name) > 20 {
			name = name[:20]
		}
		fmt.Printf("%6d %-20s %s%10.1f MB\033[0m %7.1f%% %8d %-12s\n",
			p.Pid, name, color, p.Memory, p.CPU, p.Threads, p.Status)
	}
	fmt.Println(strings.Repeat("-", 90))
	var totalMem float64
	for _, p := range processes {
		totalMem += p.Memory
	}
	fmt.Printf("Всего памяти: %.1f MB\n", totalMem)
}

func main() {
	sortBy := flag.String("sort", "memory", "Сортировка (memory, cpu, pid, name)")
	filter := flag.String("filter", "", "Фильтр по имени")
	interval := flag.Int("interval", 2, "Интервал обновления (сек)")
	flag.Parse()

	fmt.Println("📊 ProcessMonitor Pro — Go Edition")
	fmt.Println("Нажмите Ctrl+C для выхода\n")

	sigChan := make(chan os.Signal, 1)
	signal.Notify(sigChan, syscall.SIGINT, syscall.SIGTERM)

	ticker := time.NewTicker(time.Duration(*interval) * time.Second)
	defer ticker.Stop()

	for {
		select {
		case <-sigChan:
			fmt.Println("\nВыход...")
			return
		case <-ticker.C:
			processes := getProcesses(*filter)
			display(processes, *sortBy, *filter)
		}
	}
}
