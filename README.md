# FCFS CPU Scheduler with GUI

A Windows GUI application that simulates the First-Come-First-Serve (FCFS) CPU scheduling algorithm. This application visualizes how FCFS works and calculates important performance metrics.

## Features

- FCFS scheduling algorithm simulation with graphical interface
- Support for up to 10 processes
- Calculation and display of key metrics:
  - Waiting Time
  - Turnaround Time
  - Response Time
- Smart handling of invalid inputs
- Visual process representation with random colors
- User-friendly interface with animations

## System Requirements

- Windows 10 or later
- C++ compiler with C++11 support
- Windows API (Win32)

## Installation

1. Clone the repository:

```bash
git clone [repository-url]
```

2. Run `run.bat` to compile and launch the application:

```bash
.\run.bat
```

## Usage

1. Enter process information:

   - Process name
   - Arrival Time (must be non-negative)
   - Burst Time (must be positive)

2. Click "Add Process" to add the process to the list

3. View results:

   - Average times for valid processes
   - Detailed process information including status and metrics
   - Clear marking of invalid inputs with reasons

4. Use "Clear All" button to remove all processes and start over

## Code Structure

- `fcfs_win.cpp`: Main file containing Windows GUI code and main loop
- `FCFSScheduler.h`: FCFS algorithm handler class definition
- `Process.h`: Process structure definition
- `run.bat`: Compilation and execution script

## Error Handling

The application handles invalid input cases:

- Negative arrival times
- Non-positive burst times
- Exceeding process limit

Instead of crashing, the application will:

- Clearly mark invalid inputs
- Explain why they are invalid
- Continue calculations for valid processes

## Performance Metrics

The scheduler calculates several important metrics:

1. Waiting Time: Time spent waiting before execution
2. Turnaround Time: Total time from arrival to completion
3. Response Time: Time from arrival to first execution

## Algorithm Details

The FCFS scheduler:

- Processes tasks in order of arrival
- Handles process queuing automatically
- Calculates all metrics in real-time
- Provides visual feedback of scheduling decisions

## Contributing

Contributions are welcome. Please follow these steps:

1. Fork the repository
2. Create a new branch
3. Make your changes
4. Push to your branch
5. Create a Pull Request

## License

[Add your license information here]

## Support

For issues and feature requests, please create an issue in the repository.
