// processmonitor_js.js — монитор процессов на JavaScript (Node.js)

const si = require('systeminformation');
const readline = require('readline');

const args = process.argv.slice(2);
let sortBy = 'memory';
let filter = '';
let interval = 2;

for (let i = 0; i < args.length; i++) {
    if (args[i] === '--sort' && i+1 < args.length) {
        sortBy = args[++i];
    } else if (args[i] === '--filter' && i+1 < args.length) {
        filter = args[++i];
    } else if (args[i] === '--interval' && i+1 < args.length) {
        interval = parseInt(args[++i]);
    }
}

console.log('📊 ProcessMonitor Pro — JavaScript Edition');
console.log('Нажмите Ctrl+C для выхода\n');

async function getProcesses() {
    try {
        const processes = await si.processes();
        let list = processes.list || [];
        if (filter) {
            list = list.filter(p => p.name && p.name.toLowerCase().includes(filter.toLowerCase()));
        }
        return list.map(p => ({
            pid: p.pid,
            name: p.name || 'unknown',
            memory_mb: (p.mem || 0) / (1024 * 1024),
            cpu: p.cpu || 0,
            threads: p.threads || 0,
            status: p.state || 'running'
        }));
    } catch (err) {
        console.error('Ошибка получения процессов:', err);
        return [];
    }
}

function display(processes) {
    // Сортировка
    if (sortBy === 'memory') {
        processes.sort((a, b) => b.memory_mb - a.memory_mb);
    } else if (sortBy === 'cpu') {
        processes.sort((a, b) => b.cpu - a.cpu);
    } else if (sortBy === 'pid') {
        processes.sort((a, b) => a.pid - b.pid);
    } else {
        processes.sort((a, b) => (a.name || '').localeCompare(b.name || ''));
    }

    console.clear();
    console.log('📊 ProcessMonitor Pro — JavaScript Edition');
    console.log(`Процессов: ${processes.length} | Фильтр: ${filter || 'все'}`);
    console.log('-'.repeat(90));
    console.log(`${'PID'.padStart(6)} ${'ИМЯ'.padEnd(20)} ${'ПАМЯТЬ'.padStart(12)} ${'CPU'.padStart(8)} ${'ПОТОКИ'.padStart(8)} ${'СТАТУС'.padEnd(12)}`);
    console.log('-'.repeat(90));

    let count = 0;
    for (const p of processes) {
        if (count++ > 30) break;
        const color = p.memory_mb > 500 ? '\x1b[91m' : (p.memory_mb > 200 ? '\x1b[93m' : '\x1b[92m');
        const name = (p.name || 'unknown').substring(0, 20);
        console.log(
            `${String(p.pid).padStart(6)} ${name.padEnd(20)} ${color}${p.memory_mb.toFixed(1).padStart(10)} MB\x1b[0m ` +
            `${p.cpu.toFixed(1).padStart(7)}% ${String(p.threads).padStart(8)} ${(p.status || 'unknown').padEnd(12)}`
        );
    }
    console.log('-'.repeat(90));
    const totalMemory = processes.reduce((sum, p) => sum + p.memory_mb, 0);
    console.log(`Всего памяти: ${totalMemory.toFixed(1)} MB`);
}

async function run() {
    while (true) {
        const processes = await getProcesses();
        display(processes);
        await new Promise(resolve => setTimeout(resolve, interval * 1000));
    }
}

run().catch(console.error);
