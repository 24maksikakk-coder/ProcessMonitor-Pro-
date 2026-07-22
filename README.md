📊 ProcessMonitor Pro — монитор процессов (использование памяти)
Профессиональный инструмент для мониторинга процессов в реальном времени с отображением использования памяти, CPU, количества потоков, состояния и сортировкой.
Поддерживает цветной вывод, фильтрацию, автообновление и экспорт данных.
Реализован на 7 языках программирования для демонстрации работы с системными API и управлением процессами.

https://img.shields.io/github/repo-size/yourname/processmonitor
https://img.shields.io/github/stars/yourname/processmonitor?style=social
https://img.shields.io/badge/License-MIT-blue.svg

🧠 Концепция
ProcessMonitor Pro — это утилита для отслеживания активности процессов в системе. Она позволяет:

✅ Отображать список всех процессов с PID, именем, использованием памяти (RSS) и CPU.

✅ Сортировать процессы по памяти, CPU, PID или имени.

✅ Фильтровать процессы по имени (поиск).

✅ Автообновление в реальном времени (с настраиваемым интервалом).

✅ Цветовой вывод — выделение процессов с высоким потреблением памяти.

✅ Экспорт данных в CSV и JSON.

✅ Отображение общей статистики — количество процессов, общее использование памяти.

✅ Удобный интерфейс — как в командной строке, так и в GUI (в некоторых версиях).

🚀 Как запустить
Каждая версия использует соответствующие системные библиотеки. Инструкции по установке и запуску:

Python
bash
pip install psutil
python processmonitor_python.py
python processmonitor_python.py --sort memory --filter chrome
C++
bash
# Для Linux требуется компилятор с C++17
g++ -std=c++17 processmonitor_cpp.cpp -o processmonitor
./processmonitor --sort memory --filter chrome
Java
bash
javac processmonitor_java.java && java processmonitor_java
java processmonitor_java --sort memory --filter chrome
C# (.NET Core)
bash
dotnet run -- --sort memory --filter chrome
Go
bash
go get github.com/shirou/gopsutil/v3
go run processmonitor_go.go --sort memory --filter chrome
Rust
bash
cargo add sysinfo
cargo build --release
./target/release/processmonitor_rs --sort memory --filter chrome
JavaScript (Node.js)
bash
npm install systeminformation
node processmonitor_js.js --sort memory --filter chrome
🧩 Пример использования
bash
$ processmonitor --sort memory --filter python
PID   NAME            MEMORY    CPU     THREADS  STATUS
1234  python3         245.6 MB  12.3%   8        running
5678  python3         124.8 MB  3.2%    5        sleeping
9012  python3         89.1 MB   1.1%    3        running
📦 Содержимое репозитория
Файл	Язык	Особенности
processmonitor_python.py	Python	psutil, цветной вывод, автообновление, экспорт
processmonitor_cpp.cpp	C++	системные вызовы, цветной вывод, сортировка
processmonitor_java.java	Java	ManagementFactory, цветной вывод (ANSI)
processmonitor_cs.cs	C#	System.Diagnostics, асинхронное обновление
processmonitor_go.go	Go	gopsutil, горутины, цветной вывод
processmonitor_rs.rs	Rust	sysinfo, цветной вывод, сортировка
processmonitor_js.js	JavaScript	systeminformation, цветной вывод, автообновление
🔮 Расширенные функции
Экспорт в CSV/JSON для анализа.

Уведомления при превышении порога памяти.

Интерактивный режим с клавишами управления (в консольных версиях).

График использования памяти в реальном времени (в некоторых версиях).

📜 Лицензия
MIT — свободно используйте, модифицируйте и распространяйте.

🤝 Вклад
Приветствуются пул-реквесты с улучшениями, поддержкой новых платформ и расширением функциональности.

⭐ Если проект помогает вам контролировать систему — поставьте звёздочку!
